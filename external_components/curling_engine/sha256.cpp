#include "sha256.h"

#include <array>
#include <cstring>
#include <cstddef>
 #if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
#include <esp_task_wdt.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

namespace esphome {
namespace curling_engine {

namespace {

inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }

const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
    0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
    0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
    0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
    0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
    0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
    0xc67178f2};

struct Sha256Ctx {
  uint32_t h[8];
  uint64_t total_len;
  uint8_t buf[64];
  size_t buf_len;
};

#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
static bool s_wdt_added = false;
static void wdt_ensure() {
  if (s_wdt_added)
    return;
  esp_err_t err = esp_task_wdt_add(NULL);
  if (err == ESP_OK || err == ESP_ERR_INVALID_STATE)
    s_wdt_added = true;
}
#endif

void ctx_init(Sha256Ctx *c) {
  c->h[0] = 0x6a09e667;
  c->h[1] = 0xbb67ae85;
  c->h[2] = 0x3c6ef372;
  c->h[3] = 0xa54ff53a;
  c->h[4] = 0x510e527f;
  c->h[5] = 0x9b05688c;
  c->h[6] = 0x1f83d9ab;
  c->h[7] = 0x5be0cd19;
  c->total_len = 0;
  c->buf_len = 0;
}

void process_block(Sha256Ctx *c, const uint8_t *p) {
  uint32_t w[64];
  for (int i = 0; i < 16; i++)
    w[i] = (uint32_t(p[i * 4]) << 24) | (uint32_t(p[i * 4 + 1]) << 16) |
           (uint32_t(p[i * 4 + 2]) << 8) | uint32_t(p[i * 4 + 3]);
  for (int i = 16; i < 64; i++) {
    uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
    uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }
  uint32_t a = c->h[0], b = c->h[1], cc = c->h[2], d = c->h[3];
  uint32_t e = c->h[4], f = c->h[5], g = c->h[6], h = c->h[7];
  for (int i = 0; i < 64; i++) {
    uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
    uint32_t ch = (e & f) ^ (~e & g);
    uint32_t t1 = h + s1 + ch + K[i] + w[i];
    uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
    uint32_t maj = (a & b) ^ (a & cc) ^ (b & cc);
    uint32_t t2 = s0 + maj;
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = cc;
    cc = b;
    b = a;
    a = t1 + t2;
    if ((i & 15) == 15) {
#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
      esp_task_wdt_reset();
#endif
    }
  }
  c->h[0] += a;
  c->h[1] += b;
  c->h[2] += cc;
  c->h[3] += d;
  c->h[4] += e;
  c->h[5] += f;
  c->h[6] += g;
  c->h[7] += h;
}

void ctx_update(Sha256Ctx *c, const uint8_t *data, size_t len) {
#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
  if (esp_task_wdt_status(NULL) != ESP_OK)
    esp_task_wdt_add(NULL);
#endif
  c->total_len += len;
  while (len > 0) {
 #if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
    esp_task_wdt_reset();
#endif
    size_t n = 64 - c->buf_len;
    if (n > len)
      n = len;
    std::memcpy(c->buf + c->buf_len, data, n);
    c->buf_len += n;
    data += n;
    len -= n;
    if (c->buf_len == 64) {
      process_block(c, c->buf);
      c->buf_len = 0;
#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
      vTaskDelay(1);
#endif
    }
  }
}

void ctx_final(Sha256Ctx *c, uint8_t out[32]) {
  uint64_t bits = c->total_len * 8;
  uint8_t pad = 0x80;
  ctx_update(c, &pad, 1);
  uint8_t zero = 0;
  while (c->buf_len % 64 != 56)
    ctx_update(c, &zero, 1);
  uint8_t len_be[8];
  for (int i = 0; i < 8; i++)
    len_be[i] = uint8_t(bits >> (56 - i * 8));
  ctx_update(c, len_be, 8);
  for (int i = 0; i < 8; i++) {
    out[i * 4] = uint8_t(c->h[i] >> 24);
    out[i * 4 + 1] = uint8_t(c->h[i] >> 16);
    out[i * 4 + 2] = uint8_t(c->h[i] >> 8);
    out[i * 4 + 3] = uint8_t(c->h[i]);
  }
}

}  // namespace

void sha256(const uint8_t *data, size_t len, uint8_t out[32]) {
  Sha256Ctx c;
  ctx_init(&c);
  ctx_update(&c, data, len);
  ctx_final(&c, out);
}

static std::string to_hex(const uint8_t *digest, size_t n) {
  static const char *hexd = "0123456789abcdef";
  std::string out;
  out.resize(n * 2);
  for (size_t i = 0; i < n; i++) {
    out[i * 2] = hexd[digest[i] >> 4];
    out[i * 2 + 1] = hexd[digest[i] & 0xf];
  }
  return out;
}

std::string sha256_hex(const std::string &data) {
  uint8_t digest[32];
  sha256(reinterpret_cast<const uint8_t *>(data.data()), data.size(), digest);
  return to_hex(digest, 32);
}

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *msg, size_t msg_len,
                 uint8_t out[32]) {
  uint8_t k[64];
  std::memset(k, 0, sizeof(k));
  if (key_len > 64)
    sha256(key, key_len, k);
  else
    std::memcpy(k, key, key_len);
  uint8_t ipad[64], opad[64];
  for (int i = 0; i < 64; i++) {
    ipad[i] = k[i] ^ 0x36;
    opad[i] = k[i] ^ 0x5c;
  }
  uint8_t inner[32];
  Sha256Ctx c;
  ctx_init(&c);
  ctx_update(&c, ipad, 64);
  ctx_update(&c, msg, msg_len);
  ctx_final(&c, inner);
  ctx_init(&c);
  ctx_update(&c, opad, 64);
  ctx_update(&c, inner, 32);
  ctx_final(&c, out);
}

std::string hmac_sha256_hex(const std::string &key, const std::string &msg) {
  uint8_t out[32];
  hmac_sha256(reinterpret_cast<const uint8_t *>(key.data()), key.size(),
              reinterpret_cast<const uint8_t *>(msg.data()), msg.size(), out);
  return to_hex(out, 32);
}

std::string pbkdf2_hmac_sha256(const std::string &password, const std::string &salt,
                               int iterations, size_t dk_len) {
#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
  if (esp_task_wdt_status(NULL) != ESP_OK)
    esp_task_wdt_add(NULL);
#endif
  std::string out;
  out.reserve(dk_len);
  uint32_t block_index = 1;
  size_t produced = 0;
  while (produced < dk_len) {
    uint8_t idx_be[4] = {uint8_t(block_index >> 24), uint8_t(block_index >> 16),
                         uint8_t(block_index >> 8), uint8_t(block_index)};
    std::string u_prev(salt.data(), salt.size());
    u_prev.append(reinterpret_cast<const char *>(idx_be), 4);
    std::array<uint8_t, 32> u;
    hmac_sha256(reinterpret_cast<const uint8_t *>(password.data()), password.size(),
                reinterpret_cast<const uint8_t *>(u_prev.data()), u_prev.size(), u.data());
    std::array<uint8_t, 32> t = u;
    for (int i = 1; i < iterations; i++) {
#if defined(ESP_PLATFORM) || defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32P4)
      esp_task_wdt_reset();
#endif
      hmac_sha256(reinterpret_cast<const uint8_t *>(password.data()), password.size(), u.data(),
                  32, u.data());
      for (int j = 0; j < 32; j++)
        t[j] ^= u[j];
    }
    size_t n = 32;
    if (n > dk_len - produced)
      n = dk_len - produced;
    out.append(reinterpret_cast<const char *>(t.data()), n);
    produced += n;
    block_index++;
  }
  return out;
}

std::string pbkdf2_hex(const std::string &password, const std::string &salt, int iterations,
                       size_t dk_len) {
  std::string raw = pbkdf2_hmac_sha256(password, salt, iterations, dk_len);
  return to_hex(reinterpret_cast<const uint8_t *>(raw.data()), raw.size());
}

}  // namespace curling_engine
}  // namespace esphome