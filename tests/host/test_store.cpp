#include "doctest.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifndef _WIN32
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "../../external_components/curling_engine/model.h"
#include "../../external_components/curling_engine/settings.h"
#include "../../external_components/curling_engine/store.h"
#include "generated_fixtures.h"

using esphome::curling_engine::BackupInfo;
using esphome::curling_engine::SettingsStore;
using esphome::curling_engine::StoreFs;
using esphome::curling_engine::StoreStatus;
using esphome::curling_engine::file_version;
using esphome::curling_engine::make_std_store_fs;
using esphome::curling_engine::set_enabled;

namespace stdfs = std::filesystem;

namespace {

constexpr const char *SETTINGS_TEXT = TEXT_SETTINGS_TEXT;

std::string flip_enabled(const std::string &text, bool enabled) {
  std::string out, err;
  REQUIRE(set_enabled(text, enabled, &out, &err));
  return out;
}

// Port of test_editor.py TempSettingsCase: a fresh directory with the fixture
// settings file as pitimer_settings.yml, store kept at 3 backups.
struct TempSettingsCase {
  stdfs::path dir;
  stdfs::path settings_path;
  std::unique_ptr<StoreFs> fs;
  std::unique_ptr<SettingsStore> store;

  TempSettingsCase() {
    static int counter = 0;
    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    dir = stdfs::temp_directory_path() /
          ("pitimer_store_" + std::to_string(stamp) + "_" + std::to_string(counter++));
    std::error_code ec;
    stdfs::remove_all(dir, ec);
    stdfs::create_directories(dir, ec);
    settings_path = dir / "pitimer_settings.yml";
    std::ofstream(settings_path, std::ios::binary) << SETTINGS_TEXT;
    fs.reset(make_std_store_fs());
    store = std::make_unique<SettingsStore>(fs.get(), settings_path.string(), 3);
  }

  ~TempSettingsCase() {
    std::error_code ec;
    stdfs::remove_all(dir, ec);
  }

  StoreStatus read(std::string *text, std::string *version) {
    std::string err;
    return store->read(text, version, &err);
  }

  StoreStatus save(const std::string &text, const std::string *expected_version,
                   std::string *new_version) {
    std::string err;
    return store->save(text, expected_version, new_version, &err);
  }

  StoreStatus save_err(const std::string &text, const std::string *expected_version,
                       std::string *new_version, std::string *err) {
    return store->save(text, expected_version, new_version, err);
  }

  StoreStatus backups(std::vector<BackupInfo> *out) {
    std::string err;
    return store->backups(out, &err);
  }

  StoreStatus read_backup(const std::string &name, std::string *text) {
    std::string err;
    return store->read_backup(name, text, &err);
  }

  std::vector<std::string> leftovers() {
    std::vector<std::string> out;
    for (const auto &entry : stdfs::directory_iterator(dir)) {
      std::error_code ec;
      const std::string name = entry.path().filename().string();
  if (entry.is_regular_file(ec) && !ec && name != "pitimer_settings.yml" &&
      name.rfind("pitimer_settings-", 0) != 0)
    out.push_back(name);
    }
    std::sort(out.begin(), out.end());
    return out;
  }
};

#ifndef _WIN32
std::uint64_t inode_of(const stdfs::path &p) {
  struct stat st {};
  if (stat(p.string().c_str(), &st) != 0)
    return 0;
  return static_cast<std::uint64_t>(st.st_ino);
}
#endif

}  // namespace

TEST_CASE ("file_version is sha256hex[:16]") {
  CHECK(file_version("hello") == "2cf24dba5fb0a30e");
  CHECK(file_version("hello").size() == 16);
}

TEST_CASE ("valid save replaces file and backs up") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
#ifndef _WIN32
  std::uint64_t inode_before = inode_of(tc.settings_path);
#endif
  std::string new_version;
  REQUIRE(tc.save(flip_enabled(text, false), &version, &new_version) == StoreStatus::OK);
  CHECK(new_version != version);
  std::string text2, version2;
  REQUIRE(tc.read(&text2, &version2) == StoreStatus::OK);
  CHECK(version2 == new_version);
#ifndef _WIN32
  CHECK(inode_of(tc.settings_path) != inode_before);  // atomic rename, not in-place write
#endif
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  REQUIRE(list.size() == 1);
  std::string backup_text;
  REQUIRE(tc.read_backup(list[0].name, &backup_text) == StoreStatus::OK);
  CHECK(backup_text == text);  // backup holds the pre-save content
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("invalid save leaves file untouched") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
  const std::string from = "buzzer: 11:00:00, name: 'MONDAY GAME'";
  REQUIRE(text.find(from) != std::string::npos);
  std::string bad = text;
  bad.replace(text.find(from), from.size(), "buzzer: 12:58:00, name: 'MONDAY GAME'");
  std::string new_version, err;
  REQUIRE(tc.save_err(bad, &version, &new_version, &err) == StoreStatus::INVALID);
  CHECK(err.find("overlaps") != std::string::npos);
  CHECK(err.find("web-tmp") == std::string::npos);
  std::string text2, version2;
  REQUIRE(tc.read(&text2, &version2) == StoreStatus::OK);
  CHECK(text2 == text);
  CHECK(version2 == version);
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  CHECK(list.empty());
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("stale version conflicts") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
  const std::string stale = "stale";
  std::string new_version;
  REQUIRE(tc.save(flip_enabled(text, false), &stale, &new_version) == StoreStatus::CONFLICT);
  auto change = [](const std::string &t, std::string *out, std::string *e) {
    return set_enabled(t, false, out, e);
  };
  std::string err;
  REQUIRE(tc.store->update(change, &stale, &new_version, &err) == StoreStatus::CONFLICT);
}

TEST_CASE ("update applies change and saves") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
  auto change = [](const std::string &t, std::string *out, std::string *e) {
    return set_enabled(t, false, out, e);
  };
  std::string new_version, err;
  REQUIRE(tc.store->update(change, &version, &new_version, &err) == StoreStatus::OK);
  CHECK(new_version != version);
  std::string text2, version2;
  REQUIRE(tc.read(&text2, &version2) == StoreStatus::OK);
  CHECK(version2 == new_version);
  CHECK(text2.find("enabled: False") != std::string::npos);
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  CHECK(list.size() == 1);
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("unchanged save skips backup") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
  std::string new_version;
  REQUIRE(tc.save(text, &version, &new_version) == StoreStatus::OK);
  CHECK(new_version == version);
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  CHECK(list.empty());
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("backups pruned to backups_kept") {
  TempSettingsCase tc;
  for (int count = 0; count < 5; count++) {
    std::string text, version;
    REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
    std::string new_version;
    REQUIRE(tc.save(flip_enabled(text, count % 2 == 0), &version, &new_version) ==
            StoreStatus::OK);
  }
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  CHECK(list.size() == 3);
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("backup names restricted") {
  TempSettingsCase tc;
  std::error_code ec;
  std::ofstream(tc.dir / "secret.yml", std::ios::binary) << "x";
  for (const std::string &name : {"../secret.yml", "secret.yml", "/etc/passwd"}) {
    CAPTURE(name);
    std::string text;
    std::string err;
    CHECK(tc.store->read_backup(name, &text, &err) == StoreStatus::INVALID);
    CHECK(err.find(name) != std::string::npos);
  }
}

TEST_CASE ("validate never touches live file") {
  TempSettingsCase tc;
  std::string text, version;
  auto before = std::make_pair(std::string{}, std::string{});
  REQUIRE(tc.read(&before.first, &before.second) == StoreStatus::OK);
  std::string err;
  CHECK(tc.store->validate(flip_enabled(SETTINGS_TEXT, false), &err) == StoreStatus::OK);
  CHECK(tc.store->validate("config: {}\n", &err) == StoreStatus::INVALID);
  std::string text2, version2;
  REQUIRE(tc.read(&text2, &version2) == StoreStatus::OK);
  CHECK(text2 == before.first);
  CHECK(version2 == before.second);
  CHECK(tc.leftovers().empty());
}

TEST_CASE ("live file stays readable after save") {
  TempSettingsCase tc;
  std::string text, version;
  REQUIRE(tc.read(&text, &version) == StoreStatus::OK);
  std::string new_version;
  REQUIRE(tc.save(flip_enabled(text, false), &version, &new_version) == StoreStatus::OK);
  std::string text2, version2;
  REQUIRE(tc.read(&text2, &version2) == StoreStatus::OK);
  CHECK(text2.find("enabled: False") != std::string::npos);
#ifndef _WIN32
  CHECK(access(tc.settings_path.string().c_str(), R_OK) == 0);
#endif
}

TEST_CASE ("backup mtime parsed from filename stamp") {
  TempSettingsCase tc;
  std::error_code ec;
  const std::string crafted = "pitimer_settings-19700101-000000-000001.yml";
  std::ofstream(tc.dir / crafted, std::ios::binary) << "x";
  std::vector<BackupInfo> list;
  REQUIRE(tc.backups(&list) == StoreStatus::OK);
  REQUIRE(list.size() == 1);
  CHECK(list[0].name == crafted);
  CHECK(list[0].size == 1);
  CHECK(list[0].mtime == 0);  // 1970-01-01T00:00:00Z
}
