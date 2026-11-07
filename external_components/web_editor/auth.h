#pragma once

// Port of pitimer.web's auth layer (hash_password / check_password /
// WebConfig sessions / LoginLimiter) — pure C++, host-testable.
// NVS persistence lives in web_editor.cpp; this file has no ESP includes.

#include <cstddef>
#include <cstdint>
#include <string>

namespace esphome {
namespace web_editor {

// pitimer/web.py constants
inline constexpr int PASSWORD_ITERATIONS = 60000;
inline constexpr size_t MIN_PASSWORD_LENGTH = 6;
inline constexpr int LOGIN_FAILURES_ALLOWED = 5;
inline constexpr float LOGIN_WINDOW_SECONDS = 300;
inline constexpr int SESSION_SECONDS = 30 * 24 * 60 * 60;
inline constexpr const char *SESSION_COOKIE = "pitimer_session";
inline constexpr size_t MAX_BODY_BYTES = 64 * 1024;

// salted pbkdf2 hash in pitimer's stored form:
//   pbkdf2_sha256$${iterations}$${salt_hex}$${digest_hex}
std::string hash_password(const std::string &password);

// True if password matches a stored value made by hash_password; any
// malformed stored string returns False (parity with python's ValueError).
bool check_password(const std::string &password, const std::string &stored);

class WebConfig {
 public:
  const std::string &password_hash() const { return password_hash_; }
  const std::string &secret() const { return secret_; }
  void set_password_hash(std::string hash) { password_hash_ = std::move(hash); }
  void set_secret(std::string secret) { secret_ = std::move(secret); }
  bool configured() const { return !password_hash_.empty() && !secret_.empty(); }

  // New hash + new secret; a new secret logs out every existing session
  // (pitimer set_password behaviour). False if password is too short.
  bool set_password(const std::string &password);

  // `${expiry}.${hmac_sha256_hex(secret, expiry)}` with expiry = now + 30 days.
  std::string make_session(double now) const;

  // True for an unexpired value made with this secret.
  bool session_valid(const std::string &value, double now) const;

 private:
  std::string sign(const std::string &text) const;

  std::string password_hash_;
  std::string secret_;
};

// Refuses logins from an address after LOGIN_FAILURES_ALLOWED failures
// within LOGIN_WINDOW_SECONDS (par pitimer's LoginLimiter).
class LoginLimiter {
 public:
  bool blocked(const std::string &address, double now);
  void failed(const std::string &address, double now);

 private:
  struct Bucket {
    std::string address;
    double stamps[LOGIN_FAILURES_ALLOWED] = {};
    int count = 0;
    double last_used = 0.0;
  };

  static constexpr int MAX_ADDRESSES = 16;

  static void prune(Bucket &bucket, double now);
  Bucket *find(const std::string &address);
  Bucket *find_or_create(const std::string &address, double now);

  Bucket buckets_[MAX_ADDRESSES]{};
};

}  // namespace web_editor
}  // namespace esphome
