// POSIX/VFS StoreFs for the device (SPIFFS). Only compiled by the ESPHome
// build (plain POSIX APIs; host tests link fs_std.cpp instead).

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <string>
#include <vector>

#include "store.h"

namespace esphome {
namespace curling_engine {

namespace {

class PosixStoreFs : public StoreFs {
 public:
  bool read_bytes(const std::string &path, std::string *out) override {
    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0)
      return false;
    out->clear();
    char buf[4096];
    ssize_t n;
    bool ok = true;
    while ((n = ::read(fd, buf, sizeof(buf))) > 0)
      out->append(buf, static_cast<size_t>(n));
    if (n < 0)
      ok = false;
    ::close(fd);
    return ok;
  }

  bool write_bytes(const std::string &path, const std::string &data) override {
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
      return false;
    size_t off = 0;
    bool ok = true;
    while (off < data.size()) {
      ssize_t n = ::write(fd, data.data() + off, data.size() - off);
      if (n <= 0) {
        ok = false;
        break;
      }
      off += static_cast<size_t>(n);
    }
    if (ok)
      ::fsync(fd);  // best effort durability (python os.fsync parity)
    ::close(fd);
    return ok;
  }

  bool exists(const std::string &path) override {
    struct stat st {};
    return ::stat(path.c_str(), &st) == 0;
  }

  bool remove(const std::string &path) override {
    return ::unlink(path.c_str()) == 0 || errno == ENOENT;
  }

  bool rename(const std::string &from, const std::string &to) override {
    return ::rename(from.c_str(), to.c_str()) == 0;
  }

  bool copy(const std::string &from, const std::string &to) override {
    std::string data;
    if (!read_bytes(from, &data))
      return false;
    return write_bytes(to, data);
  }

  bool mkdir_p(const std::string &path) override {
    if (path.empty())
      return false;
    std::string prefix;
    size_t i = 0;
    if (path[0] == '/') {
      prefix = "/";
      i = 1;
    }
    while (i <= path.size()) {
      size_t slash = path.find('/', i);
      if (slash == std::string::npos)
        slash = path.size();
      if (slash > i) {
        prefix += path.substr(i, slash - i);
        if (slash != path.size() || path.back() != '/') {
          if (::mkdir(prefix.c_str(), 0755) != 0 && errno != EEXIST)
            return false;
        }
      }
      i = slash + 1;
    }
    struct stat st {};
    return ::stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
  }

  bool list_files(const std::string &dir, std::vector<std::string> *names) override {
    names->clear();
    DIR *d = ::opendir(dir.c_str());
    if (!d)
      return false;
    while (struct dirent *ent = ::readdir(d)) {
      std::string name = ent->d_name;
      if (name == "." || name == "..")
        continue;
      struct stat st {};
      if (::stat((dir + "/" + name).c_str(), &st) == 0 && S_ISREG(st.st_mode))
        names->push_back(std::move(name));
    }
    ::closedir(d);
    return true;
  }

  bool file_size(const std::string &path, uint64_t *size) override {
    struct stat st {};
    if (::stat(path.c_str(), &st) != 0)
      return false;
    *size = static_cast<uint64_t>(st.st_size);
    return true;
  }
};

}  // namespace

StoreFs *make_posix_store_fs() { return new PosixStoreFs(); }

}  // namespace curling_engine
}  // namespace esphome
