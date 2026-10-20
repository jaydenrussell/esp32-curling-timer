// Port of rpi-buzzer tests/test_settings.py against the PyYAML ground-truth
// fixtures in generated_fixtures.h (same case names as the Python subtests).
#include "doctest.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "../../external_components/curling_engine/settings.h"
#include "generated_fixtures.h"

using esphome::curling_engine::PiTimerGame;
using esphome::curling_engine::PiTimerSettings;
using esphome::curling_engine::settings_filename_ok;

namespace {

struct ParseResult {
  PiTimerSettings s;
  bool ok{false};
  std::string err;
};

ParseResult parse_text(const char *text, const char *fn = "settings.yml") {
  ParseResult r;
  r.ok = r.s.parse_settings_text(text, fn, &r.err);
  return r;
}

// fixtures.py constants
constexpr int HEADER_HOLD = 300;  // config.buzzer.header_hold in BASE_SETTINGS
constexpr int MONDAY_SECS_20260928 = 0;  // 2026-09-28 is a Monday (weekday 0)

}  // namespace

TEST_CASE("base settings parse") {
  auto r = parse_text(CASE_BASE);
  REQUIRE(r.ok);
  CHECK(r.s.valid);
  REQUIRE(r.s.relays.has_value());
  CHECK(*r.s.relays == std::vector<std::string>{"RELAY1"});
  CHECK(r.s.font_sizes.at("timer_5_digits") == 816);
  CHECK(r.s.headers.at("text").at("during_buzzer") == "LAST END");
  REQUIRE(r.s.schedules.count("Monday") == 1);
  CHECK(r.s.schedules.at("Monday").size() == 1);
  CHECK(r.s.exceptions.count("2026-10-05") == 1);
}

TEST_CASE("example settings file parses") {
  // exercises parse_settings_file: suffix check + file read (cwd is build/)
  const char *fn = "example_settings.yml";
  FILE *f = fopen(fn, "wb");
  REQUIRE(f != nullptr);
  std::fwrite(TEXT_EXAMPLE_TEXT, 1, std::strlen(TEXT_EXAMPLE_TEXT), f);
  fclose(f);
  PiTimerSettings s;
  std::string err;
  bool ok = s.parse_settings_file(fn, &err);
  CHECK(ok);
  CHECK(s.valid);
  std::remove(fn);
}

TEST_CASE("instances do not share state") {
  PiTimerSettings first, second;
  first.font_sizes["header"] = 99;
  first.schedules["Monday"] = {};
  CHECK(second.font_sizes["header"] == 0);
  CHECK(second.schedules.count("Monday") == 0);
}

TEST_CASE("missing keys rejected") {
  for (const char *c : {CASE_MISSING_CONFIG, CASE_MISSING_SCHEDULE, CASE_MISSING_EXCEPTIONS,
                        CASE_MISSING_ENABLED, CASE_MISSING_TEXT_WHEN_IDLE,
                        CASE_MISSING_COUNTDOWN_COLOR, CASE_MISSING_PACE_ENABLED,
                        CASE_MISSING_PACE_UNIT_DURATION, CASE_MISSING_FOOTER_ENTRY_NAME,
                        CASE_MISSING_BUZZER_HEADER_HOLD, CASE_MISSING_SUNDAY}) {
    auto r = parse_text(c);
    CHECK_FALSE(r.ok);
    CHECK_FALSE(r.s.valid);
  }
}

TEST_CASE("invalid values rejected") {
  for (const char *c : {CASE_INVALID_ENABLED_YES, CASE_INVALID_FONT_SIZE_BOOL,
                        CASE_INVALID_FONT_SIZE_STR, CASE_INVALID_PACE_UNIT_ZERO,
                        CASE_INVALID_RELAY5, CASE_INVALID_RELAYS_STR,
                        CASE_INVALID_SUNDAY_NULL}) {
    auto r = parse_text(c);
    CHECK_FALSE(r.ok);
    CHECK_FALSE(r.s.valid);
  }
}

TEST_CASE("empty file rejected") {
  auto r = parse_text("");
  CHECK_FALSE(r.ok);
  CHECK(r.err.find("is empty or not a YAML mapping") != std::string::npos);
}

TEST_CASE("yaml syntax error rejected") {
  // PyYAML: tab cannot start a token
  auto r = parse_text("config:\n\tenabled: true\n");
  CHECK_FALSE(r.ok);
}

TEST_CASE("wrong suffix rejected") {
  PiTimerSettings s;
  std::string err;
  CHECK_FALSE(s.parse_settings_file("settings.yaml", &err));
  CHECK(err.find("*must* have a .yml suffix") != std::string::npos);
}

TEST_CASE("failed parse leaves settings invalid") {
  PiTimerSettings s;
  std::string err;
  REQUIRE(s.parse_settings_text(CASE_BASE, "settings.yml", &err));
  CHECK(s.valid);
  CHECK_FALSE(s.parse_settings_text("", "settings.yml", &err));
  CHECK_FALSE(s.valid);
}

TEST_CASE("filename rule") {
  std::string err;
  CHECK(settings_filename_ok("settings.yml", &err));
  CHECK(settings_filename_ok("/flash/settings.yml", &err));
  CHECK_FALSE(settings_filename_ok("settings.yaml", &err));
  CHECK_FALSE(settings_filename_ok("settings", &err));
  CHECK_FALSE(settings_filename_ok("settings.yml.txt", &err));
}

// --------------------------------------------------------------------------

TEST_CASE("optional fields parsed") {
  auto r = parse_text(CASE_SUN_OPTIONAL);
  REQUIRE(r.ok);
  REQUIRE(r.s.schedules.count("Sunday") == 1);
  REQUIRE(r.s.schedules.at("Sunday").size() == 1);
  const PiTimerGame &game = r.s.schedules.at("Sunday").at(0);
  REQUIRE(game.message.has_value());
  CHECK(*game.message == "X");
  REQUIRE(game.during_buzzer_override.has_value());
  CHECK(*game.during_buzzer_override == "DONE");
  REQUIRE(game.pace_of_play_override.has_value());
  CHECK(*game.pace_of_play_override == false);
  REQUIRE(game.pace_unit_override.has_value());
  CHECK(*game.pace_unit_override == 720);
  REQUIRE(game.header_hold_override.has_value());
  CHECK(*game.header_hold_override == 30);
  CHECK(r.s.header_hold(game) == 30);
}

TEST_CASE("header hold defaults to config") {
  auto r = parse_text(CASE_SUN_DEFAULT_HOLD);
  REQUIRE(r.ok);
  const PiTimerGame &game = r.s.schedules.at("Sunday").at(0);
  CHECK(game.header_hold_override == std::nullopt);
  CHECK(r.s.header_hold(game) == HEADER_HOLD);
}

TEST_CASE("zero header hold override kept") {
  auto r = parse_text(CASE_SUN_HOLD_ZERO);
  REQUIRE(r.ok);
  CHECK(r.s.header_hold(r.s.schedules.at("Sunday").at(0)) == 0);
}

TEST_CASE("invalid entries rejected") {
  for (const char *c : {CASE_E_MISSING_BUZZER, CASE_E_BUZZER_BEFORE, CASE_E_OVER_3H,
                        CASE_E_STRING_TIME, CASE_E_PACE_ZERO, CASE_E_HOLD_NEG,
                        CASE_E_HOLD_FRAC}) {
    auto r = parse_text(c);
    CHECK_FALSE(r.ok);
  }
}

TEST_CASE("overlapping games rejected") {
  for (const char *c : {CASE_OV_STARTS_DURING, CASE_OV_STARTS_HOLD, CASE_OV_ENDS_DURING,
                        CASE_OV_ENCLOSES, CASE_OV_INSIDE}) {
    auto r = parse_text(c);
    CHECK_FALSE(r.ok);
  }
}

TEST_CASE("back to back games accepted") {
  auto r = parse_text(CASE_OK_BACK_TO_BACK);
  REQUIRE(r.ok);
  CHECK(r.s.schedules.at("Sunday").size() == 2);
}

TEST_CASE("shorter header hold override lets games sit closer") {
  auto ok = parse_text(CASE_OK_SHORTER_HOLD);
  REQUIRE(ok.ok);
  CHECK(ok.s.schedules.at("Sunday").size() == 2);
  // one second of overlap: error must mention the header holds
  auto bad = parse_text(CASE_ERR_SHORTER_HOLD);
  CHECK_FALSE(bad.ok);
  CHECK(bad.err.find("overlaps") != std::string::npos);
}

TEST_CASE("longer header hold override pushes the next game out") {
  auto bad = parse_text(CASE_ERR_LONGER_HOLD);
  CHECK_FALSE(bad.ok);
  CHECK(bad.err.find("header holds") != std::string::npos);
  auto ok = parse_text(CASE_OK_LONGER_HOLD);
  REQUIRE(ok.ok);
  CHECK(ok.s.schedules.at("Sunday").size() == 2);
}

// --------------------------------------------------------------------------

namespace {

// find_current_game at a given weekday/second/date; BASE is Monday2026-09-28
PiTimerSettings base_settings() {
  auto r = parse_text(CASE_BASE);
  REQUIRE(r.ok);
  return r.s;
}

}  // namespace

TEST_CASE("no game before start") {
  PiTimerSettings s = base_settings();
  CHECK(s.find_current_game(0, 36000 - 1, "2026-09-28") == std::nullopt);
}

TEST_CASE("game during countdown") {
  PiTimerSettings s = base_settings();
  auto game = s.find_current_game(0, 36000 + 600, "2026-09-28");
  REQUIRE(game.has_value());
  REQUIRE(game->message.has_value());
  CHECK(*game->message == "MONDAY GAME");
  REQUIRE(game->seconds_since_start.has_value());
  CHECK(*game->seconds_since_start == 600);
  REQUIRE(game->seconds_until_buzzer.has_value());
  CHECK(*game->seconds_until_buzzer == 3000);
}

TEST_CASE("game during header hold counts negative") {
  PiTimerSettings s = base_settings();
  auto game = s.find_current_game(0, 39600 + HEADER_HOLD, "2026-09-28");
  REQUIRE(game.has_value());
  REQUIRE(game->seconds_until_buzzer.has_value());
  CHECK(*game->seconds_until_buzzer == -HEADER_HOLD);
}

TEST_CASE("no game after header hold") {
  PiTimerSettings s = base_settings();
  CHECK(s.find_current_game(0, 39600 + HEADER_HOLD + 1, "2026-09-28") == std::nullopt);
}

TEST_CASE("header hold override shortens the hold") {
  auto r = parse_text(CASE_SHORT_HOLD_MONDAY);
  REQUIRE(r.ok);
  auto game = r.s.find_current_game(MONDAY_SECS_20260928, 39605, "2026-09-28");
  REQUIRE(game.has_value());
  REQUIRE(game->message.has_value());
  CHECK(*game->message == "SHORT HOLD");
  REQUIRE(game->seconds_until_buzzer.has_value());
  CHECK(*game->seconds_until_buzzer == -5);
  CHECK(r.s.find_current_game(MONDAY_SECS_20260928, 39606, "2026-09-28") == std::nullopt);
}

TEST_CASE("no game on empty day") {
  PiTimerSettings s = base_settings();
  CHECK(s.find_current_game(1, 37000, "2026-09-29") == std::nullopt);  // Tuesday
}

TEST_CASE("exception date replaces weekly schedule") {
  PiTimerSettings s = base_settings();
  CHECK(s.find_current_game(0, 37000, "2026-10-05") == std::nullopt);
  auto game = s.find_current_game(0, 43200, "2026-10-05");
  REQUIRE(game.has_value());
  REQUIRE(game->message.has_value());
  CHECK(*game->message == "SPECIAL");
}

TEST_CASE("returned game is a copy") {
  PiTimerSettings s = base_settings();
  auto game = s.find_current_game(0, 37000, "2026-09-28");
  REQUIRE(game.has_value());
  game->message = "CHANGED";
  REQUIRE(s.schedules.at("Monday").size() == 1);
  const PiTimerGame &stored = s.schedules.at("Monday").at(0);
  REQUIRE(stored.message.has_value());
  CHECK(*stored.message == "MONDAY GAME");
  CHECK(stored.seconds_until_buzzer == std::nullopt);
}
