#include "store.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>

#include "settings.h"
#include "sha256.h"

#if __has_include("esp_log.h")
#include "esp_log.h"
#endif
#ifndef ESP_LOGD
#define ESP_LOGD(...) ((void)0)
#endif

namespace esphome {
namespace curling_engine {

static const char *const TAG = "curling_store";

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

// CRC-32 (IEEE 802.3, reflected input, poly 0xEDB88320, init 0xFFFFFFFF, xorout 0xFFFFFFFF)
static uint32_t crc32_compute(const uint8_t *data, size_t len) {
  static uint32_t table[256];
  static bool table_init = false;
  if (!table_init) {
    for (uint32_t i = 0; i < 256; i++) {
      uint32_t crc = i;
      for (int j = 0; j < 8; j++) {
        if (crc & 1u)
          crc = (crc >> 1) ^ 0xEDB88320u;
        else
          crc >>= 1;
      }
      table[i] = crc;
    }
    table_init = true;
  }
  uint32_t crc = 0xFFFFFFFFu;
  for (size_t i = 0; i < len; i++) {
    crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
  }
  return crc ^ 0xFFFFFFFFu;
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

namespace {

// Trailer appended to the on-disk settings file so partial/corrupt writes can
// be detected. YAML comment => invisible to the parser and to the web editor
// (read() strips it). Format: final line "# crc32: <8 hex>".
constexpr const char *CRC_TRAILER_PREFIX = "# crc32: ";

// Split raw on-disk bytes into the YAML body and an optional stored CRC.
// Returns false when a CRC trailer is present but malformed (treat as corrupt).
bool split_crc_trailer(const std::string &raw, std::string *body, uint32_t *stored_crc,
                       bool *has_crc) {
  *has_crc = false;
  // Trailer is the last non-empty line of the file.
  size_t end = raw.find_last_not_of("\r\n");
  if (end == std::string::npos) {
    *body = raw;
    return true;
  }
  size_t line_start = raw.rfind('\n', end);
  line_start = (line_start == std::string::npos) ? 0 : line_start + 1;
  std::string last = raw.substr(line_start, end - line_start + 1);
  const size_t plen = std::char_traits<char>::length(CRC_TRAILER_PREFIX);
  if (last.size() < plen || last.compare(0, plen, CRC_TRAILER_PREFIX) != 0) {
    *body = raw;  // legacy file, no checksum
    return true;
  }
  std::string hex = last.substr(plen);
  if (hex.size() != 8) {
    return false;
  }
  uint32_t v = 0;
  for (char c : hex) {
    v <<= 4;
    if (c >= '0' && c <= '9')
      v |= static_cast<uint32_t>(c - '0');
    else if (c >= 'a' && c <= 'f')
      v |= static_cast<uint32_t>(c - 'a' + 10);
    else if (c >= 'A' && c <= 'F')
      v |= static_cast<uint32_t>(c - 'A' + 10);
    else
      return false;
  }
  *body = raw.substr(0, line_start);
  // Drop the single trailing newline the writer adds before the trailer so the
  // body matches exactly what save()/file_version() operate on.
  if (!body->empty() && body->back() == '\n')
    body->pop_back();
  if (!body->empty() && body->back() == '\r')
    body->pop_back();
  *stored_crc = v;
  *has_crc = true;
  return true;
}

// Append the CRC trailer to a body for on-disk storage.
std::string with_crc_trailer(const std::string &body) {
  char line[32];
  std::snprintf(line, sizeof(line), "\n%s%08x\n", CRC_TRAILER_PREFIX,
                static_cast<unsigned>(crc32_compute(
                    reinterpret_cast<const uint8_t *>(body.data()), body.size())));
  return body + line;
}

}  // namespace

StoreStatus SettingsStore::read_unlocked(std::string *text, std::string *version,
                                         std::string *err) {
  ESP_LOGD(TAG, "read %s", path_.c_str());
  std::string raw;
  if (!fs_->read_bytes(path_, &raw)) {
    if (err)
      *err = "cannot read settings file " + path_;
    return StoreStatus::IO;
  }
  std::string body;
  uint32_t stored = 0;
  bool has_crc = false;
  if (!split_crc_trailer(raw, &body, &stored, &has_crc)) {
    ESP_LOGW(TAG, "malformed CRC trailer — restoring from backup");
    if (restore_latest_backup(err))
      return read_unlocked(text, version, err);
    if (err)
      *err = "malformed settings checksum and no valid backup";
    return StoreStatus::IO;
  }
  if (has_crc) {
    uint32_t computed =
        crc32_compute(reinterpret_cast<const uint8_t *>(body.data()), body.size());
    if (stored != computed) {
      ESP_LOGW(TAG, "settings CRC mismatch (stored=0x%08X computed=0x%08X) — restoring backup",
               stored, computed);
      if (restore_latest_backup(err))
        return read_unlocked(text, version, err);
      if (err)
        *err = "settings CRC mismatch and no valid backup";
      return StoreStatus::IO;
    }
  }
  *text = body;
  *version = file_version(body);
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
    for (size_t i = static_cast<size_t>(backups_kept_); i < list.size(); i++) {
      ESP_LOGD(TAG, "remove backup %s", (backup_dir_ + "/" + list[i].name).c_str());
      fs_->remove(backup_dir_ + "/" + list[i].name);
    }
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
  // ---- atomic write: write to a temporary file, then rename ----
  // This guarantees that the settings file never disappears mid-write,
  // even if power is lost between the two operations.
  std::string payload = with_crc_trailer(text);
  ESP_LOGD(TAG, "write %s (%u bytes) to temp file %s", path_.c_str(),
           (unsigned)payload.size(), tmp_path_.c_str());
  if (!fs_->write_bytes(tmp_path_, payload)) {
    if (err)
      *err = "cannot write temp " + tmp_path_;
    return StoreStatus::IO;
  }
  ESP_LOGD(TAG, "rename temp -> %s", path_.c_str());
  if (!fs_->rename(tmp_path_, path_)) {
    if (err)
      *err = "cannot rename " + path_;
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
  std::string raw;
  if (!fs_->read_bytes(backup_dir_ + "/" + name, &raw)) {
    if (err)
      *err = "cannot read backup " + name;
    return StoreStatus::IO;
  }
  std::string body;
  uint32_t stored = 0;
  bool has_crc = false;
  if (!split_crc_trailer(raw, &body, &stored, &has_crc)) {
    if (err)
      *err = "corrupt backup " + name;
    return StoreStatus::IO;
  }
  if (has_crc) {
    uint32_t computed = crc32_compute(reinterpret_cast<const uint8_t *>(body.data()), body.size());
    if (stored != computed) {
      if (err)
        *err = "backup checksum mismatch";
      return StoreStatus::IO;
    }
  }
  *text = body;
  return StoreStatus::OK;
}

bool SettingsStore::restore_latest_backup(std::string *err) {
  std::vector<BackupInfo> list;
  StoreStatus st = list_backups(&list, err);
  if (st != StoreStatus::OK)
    return false;
  for (const auto &b : list) {
    std::string backup_text;
    std::string local_err;
    if (!fs_->read_bytes(backup_dir_ + "/" + b.name, &backup_text)) {
      ESP_LOGW(TAG, "cannot read backup %s, trying next", b.name.c_str());
      continue;
    }
    // Validate the backup's CRC before restoring
    std::string body;
    uint32_t stored = 0;
    bool has_crc = false;
    if (!split_crc_trailer(backup_text, &body, &stored, &has_crc)) {
      ESP_LOGW(TAG, "malformed CRC trailer in backup %s, trying next", b.name.c_str());
      continue;
    }
    if (has_crc) {
      uint32_t computed = crc32_compute(reinterpret_cast<const uint8_t *>(body.data()), body.size());
      if (stored != computed) {
        ESP_LOGW(TAG, "backup %s CRC mismatch, skipping", b.name.c_str());
        continue;
      }
    }
    ESP_LOGI(TAG, "restoring settings from backup %s", b.name.c_str());
    std::string payload = with_crc_trailer(body);
    if (!fs_->write_bytes(path_ + ".tmp", payload)) {
      if (err)
        *err = "cannot write restored settings to temp";
      continue;
    }
    if (!fs_->rename(path_ + ".tmp", path_)) {
      if (err)
        *err = "cannot rename restored settings";
      continue;
    }
    return true;
  }
  if (err)
    *err = "no valid backup found";
  return false;
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
