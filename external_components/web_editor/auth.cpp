#include "auth.h"

#include <cstdlib>
#include <cstring>
#include <random>

#ifdef ESP_PLATFORM
#include <esp_random.h>
#endif

#include "../curling_engine/sha256.h"

namespace esphome {
namespace web_editor {

static void fill_random_bytes(uint8_t *out, size_t len) {
#ifdef ESP_PLATFORM
  esp_fill_random(out, len);
#else
  std::random_device rd;
  for (size_t i = 0; i < len; i++)
    out[i] = static_cast<uint8_t>(rd());
#endif
}

static std::string to_hex(const uint8_t *data, size_t len) {
  static const char *digits = "0123456789abcdef";
  std::string out;
  out.reserve(len * 2);
  for (size_t i = 0; i < len; i++) {
    out.push_back(digits[data[i] >> 4]);
    out.push_back(digits[data[i] & 0xF]);
  }
  return out;
}

// hex -> bytes like python's bytes.fromhex: even length, ASCII spaces
// ignored, upper/lower case accepted; empty string yields empty bytes.
static bool from_hex(const std::string &text, std::string *out) {
  out->clear();
  int high = -1;
  for (char c : text) {
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
      continue;
    int v;
    if (c >= '0' && c <= '9')
      v = c - '0';
    else if (c >= 'a' && c <= 'f')
      v = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      v = c - 'A' + 10;
    else
      return false;
    if (high < 0) {
      high = v;
    } else {
      out->push_back(static_cast<char>((high << 4) | v));
      high = -1;
    }
  }
  return high < 0;
}

// Constant-time hex string compare (parity: hmac.compare_digest on hex).
static bool hex_equal(const std::string &a, const std::string &b) {
  if (a.size() != b.size())
    return false;
  unsigned char diff = 0;
  for (size_t i = 0; i < a.size(); i++)
    diff |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);
  return diff == 0;
}

// Strict non-negative integer (python int() on the stored field).
static bool parse_iterations(const std::string &text, long *out) {
  if (text.empty())
    return false;
  long value = 0;
  for (char c : text) {
    if (c < '0' || c > '9')
      return false;
    value = value * 10 + (c - '0');
    if (value > 1000000000L)
      return false;
  }
  *out = value;
  return true;
}

std::string hash_password(const std::string &password) {
  uint8_t salt[16];
  fill_random_bytes(salt, sizeof(salt));
  const std::string salt_bytes(reinterpret_cast<const char *>(salt), sizeof(salt));
  long iterations = PASSWORD_ITERATIONS;
  if (iterations <= 0)
    iterations = PASSWORD_ITERATIONS;
  if (iterations > 1000000L)
    iterations = 1000000L;
  const std::string digest =
      curling_engine::pbkdf2_hmac_sha256(password, salt_bytes, iterations, 32);
  return "pbkdf2_sha256$" + std::to_string(iterations) + "$" + to_hex(salt, sizeof(salt)) + "$" +
         to_hex(reinterpret_cast<const uint8_t *>(digest.data()), digest.size());
}

bool check_password(const std::string &password, const std::string &stored) {
  // python: scheme, iterations, salt, digest = stored.split('$')
  size_t p1 = stored.find('$');
  size_t p2 = p1 == std::string::npos ? std::string::npos : stored.find('$', p1 + 1);
  size_t p3 = p2 == std::string::npos ? std::string::npos : stored.find('$', p2 + 1);
  if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos ||
      stored.find('$', p3 + 1) != std::string::npos)
    return false;
  const std::string scheme = stored.substr(0, p1);
  if (scheme != "pbkdf2_sha256")
    return false;
  long iterations = 0;
  if (!parse_iterations(stored.substr(p1 + 1, p2 - p1 - 1), &iterations))
    return false;
  if (iterations <= 0)
    iterations = PASSWORD_ITERATIONS;
  if (iterations > 1000000L)
    iterations = 1000000L;
  std::string salt;
  if (!from_hex(stored.substr(p2 + 1, p3 - p2 - 1), &salt))
    return false;
  if (salt.empty() || salt.size() != 16)
    return false;
  const std::string digest_hex = stored.substr(p3 + 1);
  if (digest_hex.size() != 64)
    return false;
  std::string digest_check;
  if (!from_hex(digest_hex, &digest_check) || digest_check.size() != 32)
    return false;
  const std::string digest = curling_engine::pbkdf2_hmac_sha256(password, salt,
                                                                 static_cast<int>(iterations), 32);
  return hex_equal(to_hex(reinterpret_cast<const uint8_t *>(digest.data()), digest.size()), digest_hex);
}

std::string WebConfig::sign(const std::string &text) const {
  return curling_engine::hmac_sha256_hex(secret_, text);
}

bool WebConfig::set_password(const std::string &password) {
  if (password.size() < MIN_PASSWORD_LENGTH)
    return false;
  password_hash_ = hash_password(password);
  uint8_t bytes[32];
  fill_random_bytes(bytes, sizeof(bytes));
  secret_ = to_hex(bytes, sizeof(bytes));
  return true;
}

std::string WebConfig::make_session(double now) const {
  const std::string expiry = std::to_string(static_cast<long>(now) + SESSION_SECONDS);
  return expiry + "." + sign(expiry);
}

bool WebConfig::session_valid(const std::string &value, double now) const {
  const size_t dot = value.find('.');
  if (dot == std::string::npos)
    return false;
  const std::string expiry = value.substr(0, dot);
  const std::string signature = value.substr(dot + 1);
  for (char c : expiry) {
    if (c < '0' || c > '9')
      return false;
  }
  if (expiry.empty())
    return false;
  char *end = nullptr;
  const long expires_at = std::strtol(expiry.c_str(), &end, 10);
  if (end == nullptr || *end != '\0')
    return false;
  return expires_at > now && hex_equal(signature, sign(expiry));
}

void LoginLimiter::prune(Bucket &bucket, double now) {
  const double cutoff = now - LOGIN_WINDOW_SECONDS;
  int drop = 0;
  while (drop < bucket.count && bucket.stamps[drop] < cutoff)
    drop++;
  for (int i = drop; i < bucket.count; i++)
    bucket.stamps[i - drop] = bucket.stamps[i];
  bucket.count -= drop;
}

LoginLimiter::Bucket *LoginLimiter::find(const std::string &address) {
  for (auto &bucket : buckets_) {
    if (!bucket.address.empty() && bucket.address == address)
      return &bucket;
  }
  return nullptr;
}

LoginLimiter::Bucket *LoginLimiter::find_or_create(const std::string &address, double now) {
  if (auto *bucket = find(address)) {
    bucket->last_used = now;
    return bucket;
  }
  Bucket *slot = nullptr;
  for (auto &bucket : buckets_) {
    if (bucket.address.empty()) {
      slot = &bucket;
      break;
    }
  }
  if (slot == nullptr) {
    slot = &buckets_[0];
    for (auto &bucket : buckets_) {
      if (bucket.last_used < slot->last_used)
        slot = &bucket;
    }
  }
  slot->address = address;
  slot->count = 0;
  slot->last_used = now;
  return slot;
}

bool LoginLimiter::blocked(const std::string &address, double now) {
  auto *bucket = find(address);
  if (bucket == nullptr)
    return false;
  prune(*bucket, now);
  return bucket->count >= LOGIN_FAILURES_ALLOWED;
}

void LoginLimiter::failed(const std::string &address, double now) {
  auto *bucket = find_or_create(address, now);
  prune(*bucket, now);
  if (bucket->count < LOGIN_FAILURES_ALLOWED) {
    bucket->stamps[bucket->count++] = now;
  } else {
    // python deque keeps appending; the login flow stops calling failed()
    // once blocked, so cap at the allowed count with the newest timestamp.
    for (int i = 1; i < LOGIN_FAILURES_ALLOWED; i++)
      bucket->stamps[i - 1] = bucket->stamps[i];
    bucket->stamps[LOGIN_FAILURES_ALLOWED - 1] = now;
  }
}

}  // namespace web_editor
}  // namespace esphome
