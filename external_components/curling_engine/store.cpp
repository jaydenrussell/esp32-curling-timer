#include "store.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>

#include "settings.h"
#include "sha256.h"

namespace esphome {
namespace curling_engine {

namespace {

bool ascii_word(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
         c == '_' || c == '.' || c == '-';
}

std::string dirname_of(const std::string &p) {
  auto i = p.find_last_of("/\\");
  return i == std::string::npos ? std::string(".") : p.substr(0, i);
}

std::string basename_of(const std::string &p) {
  auto i = p.find_last_of("/\\");
  return i == std::string::npos ? p : p.substr(i + 1);
}

std::string stem_of(const std::string &p) {
  std::string base = basename_of(p);
  auto i = base.find_last_of('.');
  if (i == std::string::npos || i == 0)
    return base;
  return base.substr(0, i);
}

// Days since 1970-01-01 for a proleptic Gregorian date (Howard Hinnant).
long long days_from_civil(int y, unsigned m, unsigned d) {
  y -= m <= 2;
  const long long era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const int doy = (153 * (static_cast<int>(m) + (m > 2 ? -3 : 9)) + 2) / 5 + static_cast<int>(d) - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + static_cast<unsigned>(doy);
  return era * 146097 + static_cast<long long>(doe) - 719468;
}

bool digits(const std::string &s, size_t pos, size_t n) {
  if (pos + n > s.size())
    return false;
  for (size_t i = pos; i < pos + n; i++)
    if (s[i] < '0' || s[i] > '9')
      return false;
  return true;
}

int to_int(const std::string &s, size_t pos, size_t n) {
  int v = 0;
  for (size_t i = pos; i < pos + n; i++)
    v = v * 10 + (s[i] - '0');
  return v;
}

// mtime epoch seconds from a backup filename stamp (UTC):
// {stem}-{YYYYmmdd}-{HHMMSS}-{ffffff}.yml — parse the trailing stamp so the
// value works on SPIFFS, which has no file times.
bool stamp_mtime(const std::string &name, long long *mtime) {
  if (name.size() < 9 || name.compare(name.size() - 4, 4, ".yml") != 0)
    return false;
  std::string s = name.substr(0, name.size() - 4);
  if (s.size() < 24)
    return false;
  size_t off = s.size() - 22;
  if (s[off - 1] != '-')
    return false;
  if (!digits(s, off, 8) || s[off + 8] != '-' || !digits(s, off + 9, 6) ||
      s[off + 15] != '-' || !digits(s, off + 16, 6))
    return false;
  int year = to_int(s, off, 4);
  int mon = to_int(s, off + 4, 2);
  int day = to_int(s, off + 6, 2);
  int hour = to_int(s, off + 9, 2);
  int min = to_int(s, off + 11, 2);
  int sec = to_int(s, off + 13, 2);
  if (mon < 1 || mon > 12 || day < 1 || day > 31 || hour > 23 || min > 59 || sec > 60)
    return false;
  *mtime = days_from_civil(year, static_cast<unsigned>(mon), static_cast<unsigned>(day)) * 86400 +
           hour * 3600 + min * 60 + sec;
  return true;
}

}  // namespace

std::string file_version(const std::string &data) { return sha256_hex(data).substr(0, 16); }

bool backup_name_ok(const std::string &name) {
  if (name.size() < 5 || name.compare(name.size() - 4, 4, ".yml") != 0)
    return false;
  for (char c : name)
    if (!ascii_word(c))
      return false;
  return true;
}

SettingsStore::SettingsStore(StoreFs *fs, const std::string &settings_fn, int backups_kept)
    : fs_(fs),
      path_(settings_fn),
      dir_(dirname_of(settings_fn)),
      stem_(stem_of(settings_fn)),
      tmp_path_(dir_ + "/." + stem_ + ".web-tmp.yml"),
      backup_dir_(dir_),
      backups_kept_(backups_kept) {}

StoreStatus SettingsStore::read_unlocked(std::string *text, std::string *version,
                                         std::string *err) {
  std::string data;
  if (!fs_->read_bytes(path_, &data)) {
    if (err)
      *err = "cannot read settings file " + path_;
    return StoreStatus::IO;
  }
  *text = data;
  *version = file_version(data);
  return StoreStatus::OK;
}

StoreStatus SettingsStore::read(std::string *text, std::string *version, std::string *err) {
  // Lock-free like python SettingsStore.read(): writes rename atomically, so
  // the reader always sees a complete old or new file.
  return read_unlocked(text, version, err);
}

StoreStatus SettingsStore::validate_locked(const std::string &text, std::string *err) {
  std::string local_err;
  PiTimerSettings settings;
  if (!settings.parse_settings_text(text, basename_of(path_), &local_err)) {
    if (err)
      *err = local_err;
    return StoreStatus::INVALID;
  }
  return StoreStatus::OK;
}

StoreStatus SettingsStore::validate(const std::string &text, std::string *err) {
  std::lock_guard<std::mutex> guard(lock_);
  return validate_locked(text, err);
}

bool SettingsStore::backup_locked(std::string *err) {
  if (!fs_->mkdir_p(backup_dir_)) {
    if (err)
      *err = "cannot create backup directory " + backup_dir_;
    return false;
  }
  std::string name = backup_name();
  if (!fs_->copy(path_, backup_dir_ + "/" + name)) {
    if (err)
      *err = "cannot write backup " + name;
    return false;
  }
  std::vector<BackupInfo> list;
  if (list_backups(&list, nullptr) == StoreStatus::OK) {
    for (size_t i = static_cast<size_t>(backups_kept_); i < list.size(); i++)
      fs_->remove(backup_dir_ + "/" + list[i].name);
  }
  return true;
}

StoreStatus SettingsStore::save_locked(const std::string &text,
                                       const std::string *expected_version,
                                       std::string *new_version, std::string *err) {
  std::string current;
  std::string version;
  StoreStatus st = read_unlocked(&current, &version, err);
  if (st != StoreStatus::OK)
    return st;
  if (expected_version != nullptr && version != *expected_version) {
    if (err)
      *err = CONFLICT_MESSAGE;
    return StoreStatus::CONFLICT;
  }
  if (text == current) {
    *new_version = version;
    return StoreStatus::OK;
  }
  st = validate_locked(text, err);
  if (st != StoreStatus::OK)
    return st;
  if (!backup_locked(err))
    return StoreStatus::IO;
  if (!fs_->write_bytes(tmp_path_, text)) {
    fs_->remove(tmp_path_);
    if (err)
      *err = "cannot write " + tmp_path_;
    return StoreStatus::IO;
  }
  fs_->remove(path_);
  if (!fs_->rename(tmp_path_, path_)) {
    fs_->remove(tmp_path_);
    if (err)
      *err = "cannot replace " + path_;
    return StoreStatus::IO;
  }
  *new_version = file_version(text);
  return StoreStatus::OK;
}

StoreStatus SettingsStore::save(const std::string &text, const std::string *expected_version,
                                std::string *new_version, std::string *err) {
  std::lock_guard<std::mutex> guard(lock_);
  return save_locked(text, expected_version, new_version, err);
}

StoreStatus SettingsStore::update(const StoreChangeFn &change,
                                  const std::string *expected_version, std::string *new_version,
                                  std::string *err) {
  std::lock_guard<std::mutex> guard(lock_);
  std::string text, version;
  StoreStatus st = read_unlocked(&text, &version, err);
  if (st != StoreStatus::OK)
    return st;
  if (expected_version != nullptr && version != *expected_version) {
    if (err)
      *err = CONFLICT_MESSAGE;
    return StoreStatus::CONFLICT;
  }
  std::string changed;
  if (!change(text, &changed, err))
    return StoreStatus::INVALID;
  return save_locked(changed, &version, new_version, err);
}

StoreStatus SettingsStore::list_backups(std::vector<BackupInfo> *out, std::string *err) {
  out->clear();
  if (!fs_->exists(backup_dir_))
    return StoreStatus::OK;
  std::vector<std::string> names;
  if (!fs_->list_files(backup_dir_, &names)) {
    if (err)
      *err = "cannot list backup directory " + backup_dir_;
    return StoreStatus::IO;
  }
  const std::string prefix = stem_ + "-";
  for (const auto &name : names) {
    if (name.size() < prefix.size() + 4 || name.compare(0, prefix.size(), prefix) != 0 ||
        name.compare(name.size() - 4, 4, ".yml") != 0)
      continue;
    BackupInfo info;
    info.name = name;
    if (!fs_->file_size(backup_dir_ + "/" + name, &info.size)) {
      if (err)
        *err = "cannot stat backup " + name;
      return StoreStatus::IO;
    }
    info.mtime = 0;
    stamp_mtime(name, &info.mtime);
    out->push_back(std::move(info));
  }
  std::sort(out->begin(), out->end(),
            [](const BackupInfo &a, const BackupInfo &b) { return a.name > b.name; });
  return StoreStatus::OK;
}

StoreStatus SettingsStore::backups(std::vector<BackupInfo> *out, std::string *err) {
  return list_backups(out, err);
}

StoreStatus SettingsStore::read_backup(const std::string &name, std::string *text,
                                       std::string *err) {
  if (!backup_name_ok(name)) {
    if (err)
      *err = "no backup named '" + name + "'";
    return StoreStatus::INVALID;
  }
  std::vector<BackupInfo> list;
  StoreStatus st = list_backups(&list, err);
  if (st != StoreStatus::OK)
    return st;
  bool found = false;
  for (const auto &b : list)
    if (b.name == name) {
      found = true;
      break;
    }
  if (!found) {
    if (err)
      *err = "no backup named '" + name + "'";
    return StoreStatus::INVALID;
  }
  if (!fs_->read_bytes(backup_dir_ + "/" + name, text)) {
    if (err)
      *err = "cannot read backup " + name;
    return StoreStatus::IO;
  }
  return StoreStatus::OK;
}

std::string SettingsStore::backup_name() const {
  using namespace std::chrono;
  system_clock::time_point now = system_clock::now();
  system_clock::time_point tp_secs = time_point_cast<seconds>(now);
  long long us = duration_cast<microseconds>(now - tp_secs).count();
  std::time_t t = system_clock::to_time_t(now);
  std::tm tmv{};
#ifdef _WIN32
  gmtime_s(&tmv, &t);
#else
  gmtime_r(&t, &tmv);
#endif
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y%m%d-%H%M%S", &tmv);
  char stamp[64];
  std::snprintf(stamp, sizeof(stamp), "%s-%06lld", buf, us);
  return stem_ + "-" + stamp + ".yml";  // bare name inside backup_dir_
}

}  // namespace curling_engine
}  // namespace esphome
