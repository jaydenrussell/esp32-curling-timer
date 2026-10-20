// Host-only <filesystem> StoreFs — guarded out of device builds (ESP-IDF has
// no std::filesystem; the device links fs_posix.cpp instead).
#ifndef ESP_PLATFORM

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "store.h"

namespace fs = std::filesystem;

namespace esphome {
namespace curling_engine {

namespace {

class StdStoreFs : public StoreFs {
 public:
  bool read_bytes(const std::string &path, std::string *out) override {
    std::ifstream file(fs::path(path), std::ios::binary);
    if (!file)
      return false;
    out->assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return true;
  }

  bool write_bytes(const std::string &path, const std::string &data) override {
    std::ofstream file(fs::path(path), std::ios::binary | std::ios::trunc);
    if (!file)
      return false;
    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    file.flush();
    return file.good();
  }

  bool exists(const std::string &path) override {
    std::error_code ec;
    return fs::exists(fs::path(path), ec);
  }

  bool remove(const std::string &path) override {
    std::error_code ec;
    fs::remove(fs::path(path), ec);
    return !ec;
  }

  bool rename(const std::string &from, const std::string &to) override {
    std::error_code ec;
    fs::rename(fs::path(from), fs::path(to), ec);
    return !ec;
  }

  bool copy(const std::string &from, const std::string &to) override {
    std::error_code ec;
    fs::copy_file(fs::path(from), fs::path(to),
                  fs::copy_options::overwrite_existing, ec);
    return !ec;
  }

  bool mkdir_p(const std::string &path) override {
    std::error_code ec;
    fs::create_directories(fs::path(path), ec);
    if (ec)
      return false;
    return fs::is_directory(fs::path(path), ec);
  }

  bool list_files(const std::string &dir, std::vector<std::string> *names) override {
    names->clear();
    std::error_code ec;
    fs::directory_iterator it(fs::path(dir), ec);
    if (ec)
      return false;
    for (const auto &entry : it) {
      std::error_code ec2;
      if (entry.is_regular_file(ec2) && !ec2)
        names->push_back(entry.path().filename().string());
    }
    return true;
  }

  bool file_size(const std::string &path, uint64_t *size) override {
    std::error_code ec;
    auto n = fs::file_size(fs::path(path), ec);
    if (ec)
      return false;
    *size = static_cast<uint64_t>(n);
    return true;
  }
};

}  // namespace

StoreFs *make_std_store_fs() { return new StdStoreFs(); }

}  // namespace curling_engine
}  // namespace esphome

#endif  // !ESP_PLATFORM
