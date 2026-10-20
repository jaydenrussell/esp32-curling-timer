#pragma once

// SettingsStore: pitimer editor.py SettingsStore port — version-checked,
// validated, backed up, atomic-replace writes of the live settings file.
// Filesystem access goes through StoreFs so store.cpp is platform neutral:
//   make_std_store_fs()  — <filesystem> implementation for host tests
//   make_posix_store_fs() — POSIX/VFS implementation for the device (SPIFFS)

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace esphome {
namespace curling_engine {

constexpr int BACKUPS_KEPT = 30;
constexpr const char *BACKUP_DIR_NAME = "backups";
constexpr const char *CONFLICT_MESSAGE =
    "the settings file changed since you loaded it; reload and try again";

// Content hash version tag: sha256hex(data)[:16] (editor.py file_version).
std::string file_version(const std::string &data);

// BACKUP_NAME_RE = ^[\w.-]+\.yml$ (ASCII \w).
bool backup_name_ok(const std::string &name);

class StoreFs {
 public:
  virtual ~StoreFs() = default;
  virtual bool read_bytes(const std::string &path, std::string *out) = 0;
  virtual bool write_bytes(const std::string &path, const std::string &data) = 0;
  virtual bool exists(const std::string &path) = 0;
  virtual bool remove(const std::string &path) = 0;  // missing ok
  virtual bool rename(const std::string &from, const std::string &to) = 0;
  virtual bool copy(const std::string &from, const std::string &to) = 0;
  virtual bool mkdir_p(const std::string &path) = 0;  // exist ok
  // Plain file names (not paths), regular files only; order irrelevant.
  virtual bool list_files(const std::string &dir, std::vector<std::string> *names) = 0;
  virtual bool file_size(const std::string &path, uint64_t *size) = 0;
};

StoreFs *make_std_store_fs();
StoreFs *make_posix_store_fs();

enum class StoreStatus {
  OK,
  CONFLICT,  // 409
  INVALID,   // 422
  IO,        // 500
};

struct BackupInfo {
  std::string name;
  uint64_t size;
  long long mtime;  // epoch seconds parsed from the backup's filename stamp
};

// change returns false + err when the transformation fails (maps to INVALID,
// mirroring python change() raising EditorError inside update()).
using StoreChangeFn = std::function<bool(const std::string &text, std::string *changed,
                                         std::string *err)>;

class SettingsStore {
 public:
  SettingsStore(StoreFs *fs, const std::string &settings_fn,
                int backups_kept = BACKUPS_KEPT);

  StoreStatus read(std::string *text, std::string *version, std::string *err);
  StoreStatus validate(const std::string &text, std::string *err);
  StoreStatus save(const std::string &text, const std::string *expected_version,
                   std::string *new_version, std::string *err);
  StoreStatus update(const StoreChangeFn &change, const std::string *expected_version,
                     std::string *new_version, std::string *err);
  StoreStatus backups(std::vector<BackupInfo> *out, std::string *err);
  StoreStatus read_backup(const std::string &name, std::string *text, std::string *err);

 private:
  StoreStatus read_unlocked(std::string *text, std::string *version, std::string *err);
  StoreStatus list_backups(std::vector<BackupInfo> *out, std::string *err);
  StoreStatus save_locked(const std::string &text, const std::string *expected_version,
                          std::string *new_version, std::string *err);
  StoreStatus validate_locked(const std::string &text, std::string *err);
  bool backup_locked(std::string *err);
  std::string backup_name() const;

  StoreFs *fs_;
  std::string path_;
  std::string dir_;
  std::string stem_;  // settings file name without the .yml suffix
  std::string tmp_path_;
  std::string backup_dir_;
  int backups_kept_;
  std::mutex lock_;
};

}  // namespace curling_engine
}  // namespace esphome
