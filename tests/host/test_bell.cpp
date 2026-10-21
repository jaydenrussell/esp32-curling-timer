// Port of pitimer tests/test_display.py TestBuzzIfDue: rings every defined
// relay once per game, tolerating a skipped zero second within the grace
// window (bell_check = buzz_if_due core).
#include "doctest.h"

#include <optional>
#include <string>

#include "../../external_components/curling_engine/bell.h"
#include "../../external_components/curling_engine/settings.h"

using esphome::curling_engine::bell_check;
using esphome::curling_engine::BELL_RELAY_1;
using esphome::curling_engine::BELL_RELAY_2;
using esphome::curling_engine::BUZZER_GRACE_SECONDS;
using esphome::curling_engine::PiTimerGame;
using esphome::curling_engine::PiTimerSettings;

namespace {

constexpr const char *TODAY = "2026-10-19";  // a Monday

// pitimer tests fixture: enabled, relays ['RELAY1', 'RELAY3'], duration 2.
PiTimerSettings make_settings(bool enabled = true,
                              const std::optional<std::vector<std::string>> &relays =
                                  std::vector<std::string>{"RELAY1", "RELAY3"},
                              int relay_duration = 2) {
  PiTimerSettings s;
  s.enabled = enabled;
  s.relays = relays;
  s.relay_duration = relay_duration;
  return s;
}

// pitimer make_game(): live game state as find_current_game would fill it.
PiTimerGame make_game(int seconds_until_buzzer, int start = 36000) {
  PiTimerGame g;
  g.start = start;
  g.buzzer = start + 3600;
  g.seconds_until_buzzer = seconds_until_buzzer;
  return g;
}

}  // namespace

TEST_CASE("bell rings every defined relay at the zero second") {
  std::string last;
  uint32_t ms = 0;
  // RELAY3 has no hardware here: only RELAY1 contributes a bit.
  CHECK(bell_check(make_settings(), make_game(0), TODAY, &last, &ms) == BELL_RELAY_1);
  CHECK(ms == 2000);
  // ...and the game is remembered.
  CHECK(bell_check(make_settings(), make_game(0), TODAY, &last, &ms) == 0);
  CHECK(ms == 0);
}

TEST_CASE("bell maps RELAY1+RELAY2 to both bits; [] consumes the game silently") {
  std::string last;
  uint32_t ms = 0;
  auto both = make_settings(true, std::vector<std::string>{"RELAY1", "RELAY2"}, 5);
  CHECK(bell_check(both, make_game(0), TODAY, &last, &ms) == (BELL_RELAY_1 | BELL_RELAY_2));
  CHECK(ms == 5000);

  std::string last2;
  uint32_t ms2 = 99;
  auto none = make_settings(true, std::vector<std::string>{}, 2);
  CHECK(bell_check(none, make_game(0), TODAY, &last2, &ms2) == 0);
  CHECK(ms2 == 0);
  // pitimer ring_buzzer([]) is a no-op but the key is still consumed.
  CHECK(last2 == std::string(TODAY) + "/36000");
  CHECK(bell_check(none, make_game(0), TODAY, &last2, &ms2) == 0);
}

TEST_CASE("bell rings when the exact zero second was skipped (grace window)") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(), make_game(-BUZZER_GRACE_SECONDS), TODAY, &last, &ms) ==
        BELL_RELAY_1);
  CHECK(last == std::string(TODAY) + "/36000");
}

TEST_CASE("bell is silent past the grace window") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(), make_game(-BUZZER_GRACE_SECONDS - 1), TODAY, &last, &ms) ==
        0);
  CHECK(last.empty());
}

TEST_CASE("bell is silent before the buzzer time") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(), make_game(1), TODAY, &last, &ms) == 0);
  CHECK(last.empty());
}

TEST_CASE("bell is silent when disabled or when there is no game") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(false), make_game(0), TODAY, &last, &ms) == 0);
  CHECK(bell_check(make_settings(), std::nullopt, TODAY, &last, &ms) == 0);
  CHECK(last.empty());
}

TEST_CASE("bell rings again for the same start time on a different date") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(), make_game(0), TODAY, &last, &ms) == BELL_RELAY_1);
  CHECK(bell_check(make_settings(), make_game(0), "2026-10-26", &last, &ms) == BELL_RELAY_1);
}

TEST_CASE("bell rings once per game for back-to-back games on the same day") {
  std::string last;
  uint32_t ms = 0;
  CHECK(bell_check(make_settings(), make_game(0, 36000), TODAY, &last, &ms) == BELL_RELAY_1);
  // Doubles night: a second game starting at 37200 (10:20).
  CHECK(bell_check(make_settings(), make_game(0, 37200), TODAY, &last, &ms) == BELL_RELAY_1);
}
