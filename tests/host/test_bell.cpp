// Port of pitimer tests/test_display.py TestBuzzIfDue: rings every defined
// relay once per game, tolerating a skipped zero second within the grace
// window (bell_check = buzz_if_due core).
#include "doctest.h"

#include <optional>
#include <set>
#include <string>
#include <vector>

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

// ---------------------------------------------------------------------------
// bell_tick — multi-event extension (start / countdown warning / buzzer)
// ---------------------------------------------------------------------------

using esphome::curling_engine::bell_tick;
using esphome::curling_engine::BellRing;

namespace {

PiTimerSettings tick_settings() {
  PiTimerSettings s;
  s.enabled = true;
  s.relays = std::vector<std::string>{"RELAY1"};
  s.ring_at_start = std::vector<std::string>{"RELAY2"};
  s.countdown_minutes_before = 2;  // target = 120s before buzzer
  s.countdown_relays = {"RELAY1", "RELAY2"};
  s.relay_duration = 3;
  return s;
}

PiTimerGame live_game(int since_start, int until) {
  PiTimerGame g;
  g.start = 36000;
  g.buzzer = 36000 + 3600;
  g.seconds_since_start = since_start;
  g.seconds_until_buzzer = until;
  return g;
}

}  // namespace

TEST_CASE("bell_tick start event rings its own relays once per game") {
  const PiTimerSettings s = tick_settings();
  std::set<std::string> fired;
  BellRing r;
  for (int since_start = 0; since_start <= BUZZER_GRACE_SECONDS; since_start++) {
    const PiTimerGame g = live_game(since_start, 3600 - since_start);
    if (since_start == 0) {
      REQUIRE(bell_tick(s, g, "2026-10-19", &fired, &r));
      CHECK(r.mask == BELL_RELAY_2);  // ring_at_start only
      CHECK(r.duration_ms == 3000);
    } else {
      CHECK_FALSE(bell_tick(s, g, "2026-10-19", &fired, &r));  // already consumed
    }
  }
  // Outside the start window: no fire.
  CHECK_FALSE(bell_tick(s, live_game(6, 3594), "2026-10-19", &fired, &r));
}

TEST_CASE("bell_tick countdown event fires in its grace window only once") {
  const PiTimerSettings s = tick_settings();
  std::set<std::string> fired;
  BellRing r;
  // target = 120: not yet due before the window.
  CHECK_FALSE(bell_tick(s, live_game(3481, 121), "2026-10-19", &fired, &r));
  REQUIRE(bell_tick(s, live_game(3480, 120), "2026-10-19", &fired, &r));
  CHECK(r.mask == (BELL_RELAY_1 | BELL_RELAY_2));
  CHECK(r.duration_ms == 3000);
  // Remaining grace seconds and later: consumed.
  CHECK_FALSE(bell_tick(s, live_game(3485, 115), "2026-10-19", &fired, &r));
  CHECK_FALSE(bell_tick(s, live_game(3500, 100), "2026-10-19", &fired, &r));
}

TEST_CASE("bell_tick buzzer event rings pitimer-style on its own relays") {
  const PiTimerSettings s = tick_settings();
  std::set<std::string> fired;
  BellRing r;
  CHECK_FALSE(bell_tick(s, live_game(3599, 1), "2026-10-19", &fired, &r));
  REQUIRE(bell_tick(s, live_game(3600, 0), "2026-10-19", &fired, &r));
  CHECK(r.mask == BELL_RELAY_1);  // buzzer relays only
  CHECK(r.duration_ms == 3000);
  CHECK_FALSE(bell_tick(s, live_game(3600, 0), "2026-10-19", &fired, &r));  // consumed
  CHECK_FALSE(bell_tick(s, live_game(3603, -3), "2026-10-19", &fired, &r));
  CHECK_FALSE(bell_tick(s, live_game(3607, -7), "2026-10-19", &fired, &r));  // past grace
}

TEST_CASE("bell_tick merges simultaneous events and rings their relays together") {
  PiTimerSettings s = tick_settings();
  // Short game: countdown target (120s) equals the whole game — start and
  // countdown windows coincide.
  std::set<std::string> fired;
  BellRing r;
  REQUIRE(bell_tick(s, live_game(0, 120), "2026-10-19", &fired, &r));
  CHECK(r.mask == (BELL_RELAY_1 | BELL_RELAY_2));
  CHECK(r.duration_ms == 3000);
  // Both events consumed: nothing left for the rest of the game.
  CHECK_FALSE(bell_tick(s, live_game(60, 60), "2026-10-19", &fired, &r));
  // The buzzer event is separate and still fires at its own time.
  REQUIRE(bell_tick(s, live_game(120, 0), "2026-10-19", &fired, &r));
  CHECK(r.mask == BELL_RELAY_1);
}

TEST_CASE("bell_tick events are off when disabled, unmapped, or zero minutes") {
  std::set<std::string> fired;
  BellRing r;
  {
    PiTimerSettings s = tick_settings();
    s.enabled = false;
    CHECK_FALSE(bell_tick(s, live_game(0, 3600), "2026-10-19", &fired, &r));
  }
  {
    PiTimerSettings s = tick_settings();
    s.ring_at_start = std::vector<std::string>{};  // empty list = off
    CHECK_FALSE(bell_tick(s, live_game(0, 3600), "2026-10-19", &fired, &r));
    s.ring_at_start.reset();  // absent = off
    CHECK_FALSE(bell_tick(s, live_game(0, 3600), "2026-10-19", &fired, &r));
  }
  {
    PiTimerSettings s = tick_settings();
    s.countdown_minutes_before = 0;  // countdown off
    s.countdown_relays.clear();
    CHECK_FALSE(bell_tick(s, live_game(3480, 120), "2026-10-19", &fired, &r));
  }
  {
    // Buzzer event with no relays configured: silent (pitimer allows []).
    PiTimerSettings s;
    s.enabled = true;
    s.relays = std::vector<std::string>{};
    CHECK_FALSE(bell_tick(s, live_game(3600, 0), "2026-10-19", &fired, &r));
  }
  CHECK_FALSE(bell_tick(tick_settings(), std::nullopt, "2026-10-19", &fired, &r));
}

TEST_CASE("bell_tick rings again for the same start time on a new date") {
  const PiTimerSettings s = tick_settings();
  std::set<std::string> fired;
  BellRing r;
  REQUIRE(bell_tick(s, live_game(0, 3600), "2026-10-19", &fired, &r));
  CHECK(r.mask == BELL_RELAY_2);
  REQUIRE(bell_tick(s, live_game(0, 3600), "2026-10-20", &fired, &r));
  CHECK(r.mask == BELL_RELAY_2);
}

TEST_CASE("bell_tick keeps the fired-event log bounded") {
  const PiTimerSettings s = tick_settings();
  std::set<std::string> fired;
  BellRing r;
  // Many games over many days must not grow the log without bound.
  for (int day = 1; day <= 100; day++) {
    std::string day_str = std::to_string(day);
    day_str.insert(0, 3 - day_str.size(), '0');  // fixed width: sorts like a date
    const std::string date = "2026-10-" + day_str;
    REQUIRE(bell_tick(s, live_game(0, 3600), date, &fired, &r));
    CHECK(fired.size() <= 64);
  }
  // The current game's key is still there: the newest date sorts last.
  CHECK(fired.count("2026-10-100/36000/start") == 1);
}

TEST_CASE("bell_check still matches pitimer for buzzer-only users") {
  // The legacy entry point drives the same buzzer event; keep it available
  // for buzzer-only integrations and its existing host tests above.
  const PiTimerSettings s = tick_settings();
  std::string last;
  uint32_t duration_ms = 0;
  CHECK(bell_check(s, live_game(3600, 0), "2026-10-19", &last, &duration_ms) == BELL_RELAY_1);
  CHECK(duration_ms == 3000);
}
