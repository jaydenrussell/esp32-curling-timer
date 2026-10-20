#include "doctest.h"

#include <string>

#include "../../external_components/curling_engine/sha256.h"

using namespace esphome::curling_engine;

TEST_CASE ("sha256 known vectors") {
  CHECK(sha256_hex("") ==
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  CHECK(sha256_hex("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  CHECK(sha256_hex("The quick brown fox jumps over the lazy dog") ==
        "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

TEST_CASE ("sha256 multi-block inputs") {
  CHECK(sha256_hex(std::string(64, 'a')) ==
        "ffe054fe7ae0cb6dc65c3af9b61d5209f439851db43d0ba5997337df154668eb");
  CHECK(sha256_hex(std::string(100, 'a')) ==
        "2816597888e4a0d3a36b82b83316ab32680eb8f00f8cd3b904d681246d285a0e");
}

TEST_CASE ("hmac-sha256 RFC 4231 case 2") {
  CHECK(hmac_sha256_hex("Jefe", "what do ya want for nothing?") ==
        "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");
}

TEST_CASE ("hmac-sha256 long key (> block size)") {
  CHECK(hmac_sha256_hex(std::string(80, 'k'), "message") ==
        "af9806bf3d9e3fe6852a9c8ca3cb7d8a2a7c9ed61592404e59f677512cf9dc63");
}

TEST_CASE ("pbkdf2-hmac-sha256 vectors") {
  CHECK(pbkdf2_hex("password", "salt", 1, 32) ==
        "120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b");
  CHECK(pbkdf2_hex("password", "salt", 1000, 32) ==
        "632c2812e46d4604102ba7618e9d6d7d2f8128f6266b4a03264d2a0460b7dcb3");
  CHECK(pbkdf2_hex("pw", "s", 5, 64) ==
        "64f396d803186a5ff2d9f30ed5cdacf0316c5d02b3e43b652bfd3c95113d4bb0"
        "7aa942ff68ee0d49625a2078ef814b3bc0de445859bceeef604747ca0b99526a");
}

TEST_CASE ("pbkdf2 auth settings (60k iterations like the web editor)") {
  CHECK(pbkdf2_hex("hunter2", "pitimer-salt-0001", 60000, 32) ==
        "f28e9369ae16a625d3e3c930bebe91236c986c1cc6799a4344ccfe39e82a3f25");
}
