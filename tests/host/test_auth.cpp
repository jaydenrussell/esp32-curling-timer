#include "doctest.h"

#include "../../external_components/web_editor/auth.h"

using esphome::web_editor::check_password;
using esphome::web_editor::hash_password;
using esphome::web_editor::LoginLimiter;
using esphome::web_editor::MIN_PASSWORD_LENGTH;
using esphome::web_editor::SESSION_SECONDS;
using esphome::web_editor::WebConfig;

// Ground truth generated with python hashlib/hmac (same as pitimer's web.py).
static const std::string STORED_1000 =
    "pbkdf2_sha256$1000$00112233445566778899aabbccddeeff$"
    "8d0e446b97cda4b9cc19d43d230bd4c4cebfacb30bf5b998c9f25ce5650d4d94";
static const std::string STORED_60000 =
    "pbkdf2_sha256$60000$706974696d65722d73616c742d30303031$"
    "f28e9369ae16a625d3e3c930bebe91236c986c1cc6799a4344ccfe39e82a3f25";

TEST_CASE ("check_password matches python pbkdf2 vectors") {
  CHECK(check_password("correct-horse", STORED_1000));
  CHECK(check_password("hunter2", STORED_60000));
  CHECK_FALSE(check_password("correct-horse ", STORED_1000));
  CHECK_FALSE(check_password("Correct-Horse", STORED_1000));
  CHECK_FALSE(check_password("wrong-pass", STORED_1000));
}

TEST_CASE ("check_password rejects malformed stored strings") {
  CHECK_FALSE(check_password("x", ""));
  CHECK_FALSE(check_password("x", "pbkdf2_sha256"));
  CHECK_FALSE(check_password("x", "pbkdf2_sha256$1000$0011"));
  CHECK_FALSE(check_password("x", "pbkdf2_sha256$1000$0011$aa$bb"));  // too many fields
  CHECK_FALSE(check_password("x", "sha1$1000$0011$aa"));  // wrong scheme
  CHECK_FALSE(check_password("x", "pbkdf2_sha256$abc$0011$aa"));  // iterations not a number
  CHECK_FALSE(check_password("x", "pbkdf2_sha256$1000$zz$aa"));  // salt not hex
  CHECK_FALSE(check_password("x", "pbkdf2_sha256$1000$0011"));  // missing digest
}

TEST_CASE ("hash_password round-trips through check_password") {
  const std::string stored = hash_password("correct-horse");
  CHECK(stored.rfind("pbkdf2_sha256$60000$", 0) == 0);
  CHECK(check_password("correct-horse", stored));
  CHECK_FALSE(check_password("correct-hors", stored));
  // two hashes of the same password differ (fresh random salt)
  CHECK(hash_password("correct-horse") != stored);
}

TEST_CASE ("WebConfig set_password enforces minimum length and rotates secret") {
  WebConfig config;
  CHECK_FALSE(config.configured());
  CHECK_FALSE(config.set_password("short"));  // pitimer: MIN_PASSWORD_LENGTH = 6
  CHECK_FALSE(config.configured());
  REQUIRE(config.set_password("hurryhard"));
  CHECK(config.configured());
  CHECK(check_password("hurryhard", config.password_hash()));
  const std::string secret = config.secret();
  CHECK(secret.size() == 64);  // secrets.token_hex(32)
  // a second set_password rotates the secret -> old sessions die
  REQUIRE(config.set_password("hurryhard2"));
  CHECK(config.secret() != secret);
}

TEST_CASE ("WebConfig session make/validate/expiry/tamper") {
  WebConfig config;
  config.set_secret("s3cret");
  const double now = 1780000000.0;
  const std::string session = config.make_session(now);
  // python: f'{expiry}.{hmac.new(secret, expiry).hexdigest()}' — signs the expiry
  CHECK(session == "1782592000.7b56fd9f2961cb55134915d697a37283ba7339bf34db2a5df708143a12e59b26");
  CHECK(SESSION_SECONDS == 30 * 24 * 60 * 60);
  CHECK(config.session_valid(session, now));
  CHECK(config.session_valid(session, 1782591999.9));  // expiry is exclusive
  CHECK_FALSE(config.session_valid(session, 1782592000.0));
  CHECK_FALSE(config.session_valid(session, 1782592000.1));
  CHECK_FALSE(config.session_valid("1782592000.deadbeef", now));  // wrong signature
  CHECK_FALSE(config.session_valid("1782592001.7b56fd9f2961cb55134915d697a37283ba7339bf34db2a5df708143a12e59b26",
                                  now));  // expiry swapped, signature stale
  CHECK_FALSE(config.session_valid("-1.7b56fd9f2961cb55134915d697a37283ba7339bf34db2a5df708143a12e59b26",
                                  now));  // expiry not digits
  CHECK_FALSE(config.session_valid("1782592000", now));  // no dot
  CHECK_FALSE(config.session_valid("", now));
  // a different secret invalidates everything
  config.set_secret("other");
  CHECK_FALSE(config.session_valid(session, now));
}

TEST_CASE ("LoginLimiter blocks after five failures inside the window") {
  LoginLimiter limiter;
  const std::string ip = "192.168.1.50";
  CHECK_FALSE(limiter.blocked(ip, 0.0));
  limiter.failed(ip, 10.0);
  limiter.failed(ip, 20.0);
  limiter.failed(ip, 30.0);
  limiter.failed(ip, 40.0);
  CHECK_FALSE(limiter.blocked(ip, 50.0));
  limiter.failed(ip, 60.0);
  CHECK(limiter.blocked(ip, 70.0));
  CHECK(limiter.blocked(ip, 309.0));  // still inside 300 s of first failure
  CHECK_FALSE(limiter.blocked(ip, 370.0));  // window expired (failure at 70 s gone)
}

TEST_CASE ("LoginLimiter tracks addresses independently and evicts old buckets") {
  LoginLimiter limiter;
  for (int i = 0; i < 5; i++)
    limiter.failed("10.0.0.1", double(i));
  CHECK(limiter.blocked("10.0.0.1", 10.0));
  CHECK_FALSE(limiter.blocked("10.0.0.2", 10.0));
  // fill past MAX_ADDRESSES: oldest bucket is evicted, new addresses still count
  for (int i = 0; i < 16; i++) {
    const std::string ip = "10.1.0." + std::to_string(i);
    for (int j = 0; j < 5; j++)
      limiter.failed(ip, 100.0 + j);
    CHECK(limiter.blocked(ip, 110.0));
  }
  // 10.0.0.1's bucket may have been evicted, but if present it is still blocked
  // or empty (recreated); the new addresses must remain blocked.
  for (int i = 0; i < 16; i++)
    CHECK(limiter.blocked("10.1.0." + std::to_string(i), 110.0));
}
