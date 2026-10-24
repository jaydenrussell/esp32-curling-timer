// Port of rpi-buzzer tests/test_editor.py TestQuickStart + TestCancelQuickStart.
#include "doctest.h"

#include <string>
#include <vector>

#include "../../external_components/curling_engine/model.h"
#include "../../external_components/curling_engine/quickstart.h"
#include "../../external_components/curling_engine/settings.h"
#include "generated_fixtures.h"

using esphome::curling_engine::Entry;
using esphome::curling_engine::ExceptionDay;
using esphome::curling_engine::PiTimerSettings;
using esphome::curling_engine::QUICK_NOTE;
using esphome::curling_engine::ScheduleModel;
using esphome::curling_engine::QuickStartOptions;
using esphome::curling_engine::cancel_quick_start;
using esphome::curling_engine::format_duration;
using esphome::curling_engine::load_model;
using esphome::curling_engine::quick_start;
using esphome::curling_engine::splice;

namespace {

constexpr int HEADER_HOLD = 300;  // config.buzzer.header_hold in the fixture
constexpr const char *MONDAY_ISO = "2026-09-28";  // fixtures.py MONDAY

struct Loaded {
  ScheduleModel model;
  bool ok{false};
  std::string err;
};

Loaded load(const std::string &text) {
  Loaded r;
  r.ok = load_model(text, &r.model, &r.err);
  return r;
}

// at_time(MONDAY, hour, minute, second) + hold=HEADER_HOLD
QuickStartOptions mon(int hour, int minute, int second, int duration) {
  QuickStartOptions o;
  o.date_iso = MONDAY_ISO;
  o.now_sec = hour * 3600 + minute * 60 + second;
  o.duration = duration;
  o.hold = HEADER_HOLD;
  return o;
}

std::vector<std::string> names_of(const std::vector<Entry> &entries) {
  std::vector<std::string> out;
  for (const Entry &e : entries)
    out.push_back(e.name.value_or(""));
  return out;
}

}  // namespace

TEST_CASE("copies weekday schedule into new exception") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, mon(11, 30, 15, 20 * 60), &entry, &err));
  CHECK(entry.start == 41415);
  CHECK(entry.buzzer == 41415 + 1200);
  CHECK(entry.name == "QUICK START");
  CHECK(entry.quick);
  const ExceptionDay *day = m.model.exception_for(MONDAY_ISO);
  REQUIRE(day != nullptr);
  CHECK(day->note == QUICK_NOTE);
  CHECK(names_of(day->entries) == std::vector<std::string>{"MONDAY GAME", "QUICK START",
                                                           "AFTERNOON"});
  // inserted in date order
  CHECK(std::vector<std::string>{"2026-09-28", "2026-10-05", "2026-12-25"} ==
        std::vector<std::string>{m.model.exceptions[0].date, m.model.exceptions[1].date,
                                 m.model.exceptions[2].date});
}

TEST_CASE("appends to existing exception") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  QuickStartOptions opts;
  opts.date_iso = "2026-10-05";
  opts.now_sec = 9 * 3600;
  opts.duration = 60 * 60;
  opts.name = "WARM UP";
  opts.pace_of_play = true;
  opts.hold = HEADER_HOLD;
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, opts, &entry, &err));
  const ExceptionDay *day = m.model.exception_for("2026-10-05");
  REQUIRE(day != nullptr);
  CHECK(day->note == "Bonspiel");
  REQUIRE(day->entries.size() == 2);
  CHECK(day->entries[0].name == "WARM UP");
  REQUIRE(day->entries[0].pace_of_play.has_value());
  CHECK(*day->entries[0].pace_of_play);
  CHECK(day->entries[1].name == "SPECIAL");
  REQUIRE(day->entries[1].pace_of_play.has_value());
  CHECK_FALSE(*day->entries[1].pace_of_play);
}

TEST_CASE("overlap rejected with longest fit") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  // from 12:30, the 300s hold must end before the 13:00 game:
  // 24 min 59 s is the most that fits
  CHECK_FALSE(quick_start(&m.model, mon(12, 30, 0, 25 * 60), &entry, &err));
  CHECK(err.find("AFTERNOON") != std::string::npos);
  CHECK(err.find("longest timer that fits now is 24 min 59 s") != std::string::npos);
  REQUIRE(quick_start(&m.model, mon(12, 30, 0, 24 * 60 + 59), &entry, &err));
}

TEST_CASE("conflict message names an overridden hold") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  m.model.schedule["Monday"][1].header_hold = 30;
  std::string spliced;
  std::string err;
  REQUIRE(splice(TEXT_SETTINGS_TEXT, m.model, &spliced, &err));
  auto model = load(spliced);
  REQUIRE(model.ok);
  Entry entry;
  CHECK_FALSE(quick_start(&model.model, mon(12, 55, 0, 10 * 60), &entry, &err));
  CHECK(err.find("'AFTERNOON' (13:00:00-14:00:00, 30s hold)") != std::string::npos);
}

TEST_CASE("game in progress needs replace") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  // 11:03 is inside the 10:00-11:00 game's header hold
  CHECK_FALSE(quick_start(&m.model, mon(11, 3, 0, 10 * 60), &entry, &err));
  CHECK(err.find("in progress") != std::string::npos);
  CHECK(m.model.exception_for(MONDAY_ISO) == nullptr);
  REQUIRE(quick_start(&m.model, [&] {
    auto o = mon(11, 3, 0, 10 * 60);
    o.replace_conflicts = true;
    return o;
  }(), &entry, &err));
  const ExceptionDay *day = m.model.exception_for(MONDAY_ISO);
  REQUIRE(day != nullptr);
  CHECK(names_of(day->entries) == std::vector<std::string>{"QUICK START", "AFTERNOON"});
}

TEST_CASE("overlap uses each game's own header hold") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  m.model.schedule["Monday"][0].header_hold = 0;
  std::string spliced;
  std::string err;
  REQUIRE(splice(TEXT_SETTINGS_TEXT, m.model, &spliced, &err));
  auto model = load(spliced);
  REQUIRE(model.ok);
  Entry entry;
  // with no hold at all on the 10:00-11:00 game, a timer can start right after its buzzer
  REQUIRE(quick_start(&model.model, mon(11, 0, 30, 10 * 60), &entry, &err));
  CHECK(entry.start == 11 * 3600 + 30);
}

TEST_CASE("longer header hold override keeps a game in progress") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  m.model.schedule["Monday"][0].header_hold = 10 * 60;
  std::string spliced;
  std::string err;
  REQUIRE(splice(TEXT_SETTINGS_TEXT, m.model, &spliced, &err));
  auto model = load(spliced);
  REQUIRE(model.ok);
  Entry entry;
  CHECK_FALSE(quick_start(&model.model, mon(11, 9, 0, 10 * 60), &entry, &err));
  CHECK(err.find("in progress") != std::string::npos);
}

TEST_CASE("header overrides on the new timer") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  auto opts = mon(15, 0, 0, 5 * 60);
  opts.name = "PRACTICE";
  opts.during_buzzer = "--- PRACTICE DONE ---";
  opts.header_hold = 60;
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, opts, &entry, &err));
  CHECK(entry.name == "PRACTICE");
  CHECK(entry.during_buzzer == "--- PRACTICE DONE ---");
  CHECK(entry.header_hold == 60);
  std::string out;
  REQUIRE(splice(TEXT_SETTINGS_TEXT, m.model, &out, &err));
  CHECK(out.find("name: 'PRACTICE', during_buzzer: '--- PRACTICE DONE ---', header_hold: 60, "
                 "pace_of_play: False}  # quick start") != std::string::npos);
}

TEST_CASE("multi line during_buzzer rejected") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  auto opts = mon(15, 0, 0, 5 * 60);
  opts.during_buzzer = "OVER\nNOW";
  Entry entry;
  std::string err;
  CHECK_FALSE(quick_start(&m.model, opts, &entry, &err));
  CHECK(err.find("during_buzzer") != std::string::npos);
}

TEST_CASE("shorter hold leaves room for a longer timer") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  auto opts = mon(12, 30, 0, 28 * 60 + 59);
  opts.header_hold = 60;
  Entry entry;
  std::string err;
  // the timer's own 60s hold, not the 300s default, is what has to clear the 13:00 game
  REQUIRE(quick_start(&m.model, opts, &entry, &err));
  auto fresh = load(TEXT_SETTINGS_TEXT);
  REQUIRE(fresh.ok);
  auto opts2 = mon(12, 30, 0, 29 * 60);
  opts2.header_hold = 60;
  Entry entry2;
  CHECK_FALSE(quick_start(&fresh.model, opts2, &entry2, &err));
  CHECK(err.find("longest timer that fits now is 28 min 59 s") != std::string::npos);
}

TEST_CASE("bad header hold rejected") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  auto opts = mon(15, 0, 0, 5 * 60);
  opts.header_hold = -1;
  Entry entry;
  std::string err;
  CHECK_FALSE(quick_start(&m.model, opts, &entry, &err));
  CHECK(err.find("header_hold") != std::string::npos);
}

TEST_CASE("bad durations rejected") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  CHECK_FALSE(quick_start(&m.model, mon(23, 0, 0, 0), &entry, &err));
  CHECK(err.find("greater than 0") != std::string::npos);
  CHECK_FALSE(quick_start(&m.model, mon(23, 0, 0, 180 * 60), &entry, &err));
  CHECK(err.find("less than 180 min") != std::string::npos);
}

TEST_CASE("seconds precision") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, mon(15, 0, 10, 90), &entry, &err));
  CHECK(entry.start == 54010);
  CHECK(entry.buzzer == 54100);
}

TEST_CASE("format_duration") {
  CHECK(format_duration(1500) == "25 min");
  CHECK(format_duration(1499) == "24 min 59 s");
  CHECK(format_duration(45) == "45 s");
  CHECK(format_duration(60) == "1 min");
  CHECK(format_duration(1) == "1 s");
}

TEST_CASE("result is valid settings") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, mon(15, 0, 0, 45 * 60), &entry, &err));
  std::string out;
  REQUIRE(splice(TEXT_SETTINGS_TEXT, m.model, &out, &err));
  PiTimerSettings s;
  REQUIRE(s.parse_settings_text(out, "quick.yml", &err));
  auto game = s.find_current_game(0, 15 * 3600 + 10 * 60, MONDAY_ISO);
  REQUIRE(game.has_value());
  CHECK(game->message == "QUICK START");
  REQUIRE(game->seconds_until_buzzer.has_value());
  CHECK(*game->seconds_until_buzzer == 35 * 60);
}

TEST_CASE("cancel removes auto created exception") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  REQUIRE(quick_start(&m.model, mon(15, 0, 0, 45 * 60), &entry, &err));
  REQUIRE(cancel_quick_start(&m.model, MONDAY_ISO, entry.start, &err));
  auto fresh = load(TEXT_SETTINGS_TEXT);
  REQUIRE(fresh.ok);
  CHECK(m.model.schedule == fresh.model.schedule);
  CHECK(m.model.exceptions == fresh.model.exceptions);
  CHECK(m.model.inner_comments == fresh.model.inner_comments);
}

TEST_CASE("cancel keeps edited exception") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  Entry entry;
  std::string err;
  auto opts = mon(11, 3, 0, 10 * 60);
  opts.replace_conflicts = true;
  REQUIRE(quick_start(&m.model, opts, &entry, &err));
  REQUIRE(cancel_quick_start(&m.model, MONDAY_ISO, entry.start, &err));
  const ExceptionDay *day = m.model.exception_for(MONDAY_ISO);
  REQUIRE(day != nullptr);
  CHECK(names_of(day->entries) == std::vector<std::string>{"AFTERNOON"});
}

TEST_CASE("only quick entries cancelled") {
  auto m = load(TEXT_SETTINGS_TEXT);
  REQUIRE(m.ok);
  std::string err;
  CHECK_FALSE(cancel_quick_start(&m.model, "2026-10-05", 12 * 3600, &err));
  CHECK(err == "no quick start timer found to cancel");
}
