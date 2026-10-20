#pragma once

// Pure C++ SHA-256 (FIPS 180-4), HMAC-SHA256 (RFC 2104) and PBKDF2-HMAC-SHA256
// (RFC 8018) — deliberately NOT mbedtls: host-testable, no IDF dependency.
// Used by file_version (store) and the web editor auth (pbkdf2/hmac/session).

#include <cstddef>
#include <cstdint>
#include <string>

namespace esphome {
namespace curling_engine {

// One-shot SHA-256: out receives 32 bytes.
void sha256(const uint8_t *data, size_t len, uint8_t out[32]);

// SHA-256 of a byte string as lowercase hex.
std::string sha256_hex(const std::string &data);

// RFC 2104 HMAC-SHA256: out receives 32 bytes.
void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *msg, size_t msg_len,
                 uint8_t out[32]);

// HMAC-SHA256 as lowercase hex.
std::string hmac_sha256_hex(const std::string &key, const std::string &msg);

// PBKDF2-HMAC-SHA256: returns dk_len raw bytes (password/salt are byte strings;
// iterations >= 1).
std::string pbkdf2_hmac_sha256(const std::string &password, const std::string &salt,
                               int iterations, size_t dk_len);

// PBKDF2-HMAC-SHA256 as lowercase hex.
std::string pbkdf2_hex(const std::string &password, const std::string &salt, int iterations,
                       size_t dk_len);

}  // namespace curling_engine
}  // namespace esphome
