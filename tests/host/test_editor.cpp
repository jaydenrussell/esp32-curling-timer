// Port of rpi-buzzer tests/test_editor.py (structured editing) against the
// PyYAML ground-truth fixtures in generated_fixtures.h (same case names).
// from_json cases stay with Phase C; quick start with test_quickstart.cpp.
#include "doctest.h"

#include <algorithm>
#include <string>
#include <vector>

#include "../../external_components/curling_engine/model.h"
#include "../../external_components/curling_engine/settings.h"
#include "generated_fixtures.h"

using esphome::curling_engine::Entry;
using esphome::curling_engine::ExceptionDay;
using esphome::curling_engine::PiTimerSettings;
using esphome::curling_engine::ScheduleModel;
using esphome::curling_engine::format_time;
using esphome::curling_engine::load_model;
using esphome::curling_engine::set_enabled;
using esphome::curling_engine::splice;
using esphome::curling_engine::split_lines;

namespace {

// fixtures.py constants (HEADER_HOLD = 300 lives in the config text)
constexpr const char *SETTINGS_TEXT = TEXT_SETTINGS_TEXT;
constexpr const char *HEADER = TEXT_HEADER;
constexpr const char *CONFIG = TEXT_CONFIG;

struct ModelResult {
  ScheduleModel model;
  bool ok{false};
  std::string err;
};

struct SpliceResult {
  std::string out;
  bool ok{false};
  std::string err;
};

ModelResult load(const std::string &text) {
  ModelResult r;
  r.ok = load_model(text, &r.model, &r.err);
  return r;
}

SpliceResult do_splice(const std::string &text, const ScheduleModel &model) {
  SpliceResult r;
  r.ok = splice(text, model, &r.out, &r.err);
  return r;
}

// TempSettingsCase.parse — validate spliced text with the real parser
PiTimerSettings parse(const std::string &text) {
  PiTimerSettings s;
  std::string err;
  bool ok = s.parse_settings_text(text, "check.yml", &err);
  REQUIRE(ok);
  return s;
}

std::string to_crlf(std::string text) {
  size_t at = 0;
  while ((at = text.find('\n', at)) != std::string::npos) {
    text.insert(at, "\r");
    at += 2;
  }
  return text;
}

size_t count_occurrences(const std::string &haystack, const std::string &needle) {
  size_t n = 0, at = 0;
  while ((at = haystack.find(needle, at)) != std::string::npos) {
    n++;
    at += needle.size();
  }
  return n;
}

}  // namespace

TEST_CASE("example settings file is unchanged") {
  std::string text(TEXT_EXAMPLE_TEXT);
  auto model = load(text);
  REQUIRE(model.ok);
  auto r = do_splice(text, model.model);
  REQUIRE(r.ok);
  CHECK(r.out == text);
}

TEST_CASE("fixture is unchanged") {
  std::string text(SETTINGS_TEXT);
  auto model = load(text);
  REQUIRE(model.ok);
  auto r = do_splice(text, model.model);
  REQUIRE(r.ok);
  CHECK(r.out == text);
}

TEST_CASE("crlf line endings kept") {
  std::string text = to_crlf(SETTINGS_TEXT);
  auto model = load(text);
  REQUIRE(model.ok);
  Entry entry;
  entry.start = 36000;
  entry.buzzer = 39600;
  entry.name = "NEW";
  model.model.schedule["Tuesday"].push_back(entry);
  auto r = do_splice(text, model.model);
  REQUIRE(r.ok);
  // every '\n' must be part of a '\r\n'
  std::string leftover = r.out;
  size_t at = 0;
  while ((at = leftover.find("\r\n", at)) != std::string::npos)
    leftover.erase(at, 2);
  CHECK(leftover.find('\n') == std::string::npos);
  CHECK(r.out.find("        - {start: 10:00:00, buzzer: 11:00:00, name: 'NEW'}\r\n") !=
        std::string::npos);
}

TEST_CASE("notes and empty dates loaded") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  REQUIRE(model.model.exceptions.size() == 2);
  CHECK(model.model.exceptions[0].date == "2026-10-05");
  CHECK(model.model.exceptions[0].note == "Bonspiel");
  CHECK(model.model.exceptions[0].entries.size() == 1);
  CHECK(model.model.exceptions[1].date == "2026-12-25");
  CHECK(model.model.exceptions[1].note == "Closed");
  CHECK(model.model.exceptions[1].entries.empty());
  REQUIRE(model.model.exceptions[0].entries[0].pace_of_play.has_value());
  CHECK_FALSE(*model.model.exceptions[0].entries[0].pace_of_play);
}

TEST_CASE("changes keep everything else") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  Entry friday;
  friday.start = 19 * 3600;
  friday.buzzer = 20 * 3600 + 1800;
  friday.name = "O'BRIEN CUP";
  friday.during_buzzer = "LAST";
  friday.header_hold = 30;
  friday.pace_unit_duration = 720;
  model.model.schedule["Friday"] = {friday};
  model.model.exceptions[0].note = "Renamed";
  model.model.exceptions.push_back(ExceptionDay{"2027-01-01", "", {}});
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);

  std::string prefix = std::string(HEADER) + CONFIG + "\n# schedule - weekly games\nschedule:\n";
  CHECK(r.out.compare(0, prefix.size(), prefix) == 0);
  CHECK(r.out.find("\n# exceptions - specific dates\nexceptions:\n") != std::string::npos);
  CHECK(r.out.find("        - {start: 19:00:00, buzzer: 20:30:00, name: 'O''BRIEN CUP', "
                   "during_buzzer: 'LAST', header_hold: 30, pace_unit_duration: 720}\n") !=
        std::string::npos);
  CHECK(r.out.find("    2026-10-05: # Renamed\n") != std::string::npos);
  CHECK(r.out.find("    2026-12-25: [] # Closed\n") != std::string::npos);
  CHECK(r.out.find("    2027-01-01: []") != std::string::npos);
  auto settings = parse(r.out);
  REQUIRE(settings.schedules.count("Friday") == 1);
  REQUIRE(settings.schedules.at("Friday").size() == 1);
  CHECK(settings.schedules.at("Friday")[0].message == "O'BRIEN CUP");
  REQUIRE(settings.schedules.at("Friday")[0].header_hold_override.has_value());
  CHECK(*settings.schedules.at("Friday")[0].header_hold_override == 30);
  CHECK(settings.schedules.at("Friday")[0].start == 19 * 3600);
  REQUIRE(settings.exceptions.count("2027-01-01") == 1);
  CHECK(settings.exceptions.at("2027-01-01").empty());
}

TEST_CASE("no exceptions writes valid empty mapping") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  model.model.exceptions.clear();
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);
  CHECK(r.out.find("\nexceptions: {}") != std::string::npos);
  auto settings = parse(r.out);
  CHECK(settings.exceptions.empty());

  // and dates can be added back to an inline empty section
  model.model.exceptions.push_back(ExceptionDay{"2027-01-01", "Back", {}});
  auto again = do_splice(r.out, model.model);
  REQUIRE(again.ok);
  CHECK(again.out.find("\nexceptions:\n    2027-01-01: [] # Back") != std::string::npos);
  auto settings2 = parse(again.out);
  CHECK(settings2.exceptions.count("2027-01-01") == 1);
}

TEST_CASE("midnight hour written as seconds") {
  CHECK(format_time(1800) == "1800");
  CHECK(format_time(9 * 3600 + 5) == " 9:00:05");
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  Entry late;
  late.start = 1800;
  late.buzzer = 3 * 3600;
  late.name = "LATE";
  model.model.schedule["Sunday"] = {late};
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);
  auto settings = parse(r.out);
  REQUIRE(settings.schedules.count("Sunday") == 1);
  REQUIRE(settings.schedules.at("Sunday").size() == 1);
  CHECK(settings.schedules.at("Sunday")[0].start == 1800);
  CHECK(settings.schedules.at("Sunday")[0].buzzer == 3 * 3600);
}

TEST_CASE("games written in time order") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  auto &monday = model.model.schedule["Monday"];
  std::reverse(monday.begin(), monday.end());
  Entry late;
  late.start = 50000;
  late.buzzer = 51000;
  late.name = "LATE";
  Entry early;
  early.start = 36000;
  early.buzzer = 37000;
  early.name = "EARLY";
  model.model.exceptions[1].entries = {late, early};
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);
  CHECK(r.out.find("'MONDAY GAME'") < r.out.find("'AFTERNOON'"));
  CHECK(r.out.find("'EARLY'") < r.out.find("'LATE'"));
  // already ordered files are unchanged
  auto again = do_splice(SETTINGS_TEXT, load(SETTINGS_TEXT).model);
  REQUIRE(again.ok);
  CHECK(again.out == SETTINGS_TEXT);
}

TEST_CASE("quick entries marked and reloaded") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  Entry qs;
  qs.start = 36000;
  qs.buzzer = 37800;
  qs.name = "QS";
  qs.quick = true;
  model.model.exceptions[1].entries.push_back(qs);
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);
  CHECK(r.out.find("- {start: 10:00:00, buzzer: 10:30:00, name: 'QS'}  # quick start\n") !=
        std::string::npos);
  CHECK(r.out.find("    2026-12-25: # Closed\n") != std::string::npos);
  auto reloaded = load(r.out);
  REQUIRE(reloaded.ok);
  REQUIRE(reloaded.model.exceptions.size() == 2);
  REQUIRE(reloaded.model.exceptions[1].entries.size() == 1);
  CHECK(reloaded.model.exceptions[1].entries[0].quick);
}

TEST_CASE("zero header hold override round trips") {
  auto model = load(SETTINGS_TEXT);
  REQUIRE(model.ok);
  Entry no_hold;
  no_hold.start = 36000;
  no_hold.buzzer = 39600;
  no_hold.name = "NO HOLD";
  no_hold.header_hold = 0;
  model.model.schedule["Sunday"] = {no_hold};
  auto r = do_splice(SETTINGS_TEXT, model.model);
  REQUIRE(r.ok);
  CHECK(r.out.find("- {start: 10:00:00, buzzer: 11:00:00, name: 'NO HOLD', header_hold: 0}\n") !=
        std::string::npos);
  auto reloaded = load(r.out);
  REQUIRE(reloaded.ok);
  REQUIRE(reloaded.model.schedule.count("Sunday") == 1);
  REQUIRE(reloaded.model.schedule.at("Sunday").size() == 1);
  REQUIRE(reloaded.model.schedule.at("Sunday")[0].header_hold.has_value());
  CHECK(*reloaded.model.schedule.at("Sunday")[0].header_hold == 0);
  auto settings = parse(r.out);
  REQUIRE(settings.schedules.count("Sunday") == 1);
  REQUIRE(settings.schedules.at("Sunday").size() == 1);
  REQUIRE(settings.schedules.at("Sunday")[0].header_hold_override.has_value());
  CHECK(*settings.schedules.at("Sunday")[0].header_hold_override == 0);
}

TEST_CASE("inner comments counted") {
  std::string text(SETTINGS_TEXT);
  const std::string from = "    Tuesday: []";
  const std::string to = "    # Tuesday: old league\n    Tuesday: []";
  size_t at = text.find(from);
  REQUIRE(at != std::string::npos);
  text.replace(at, from.size(), to);
  auto model = load(text);
  REQUIRE(model.ok);
  CHECK(model.model.inner_comments == 1);
  auto baseline = load(SETTINGS_TEXT);
  REQUIRE(baseline.ok);
  CHECK(baseline.model.inner_comments == 0);
}

TEST_CASE("only top level enabled changes") {
  std::string result;
  std::string err;
  REQUIRE(set_enabled(SETTINGS_TEXT, false, &result, &err));
  std::string nl_a, nl_b;
  auto a = split_lines(SETTINGS_TEXT, &nl_a);
  auto b = split_lines(result, &nl_b);
  REQUIRE(a.size() == b.size());
  int changed = 0;
  for (size_t i = 0; i < a.size(); i++) {
    if (a[i] != b[i]) {
      changed++;
      CHECK(a[i] == "    enabled: true");
      CHECK(b[i] == "    enabled: False");
    }
  }
  CHECK(changed == 1);
  std::string flipped;
  REQUIRE(set_enabled(result, true, &flipped, &err));
  CHECK(count_occurrences(flipped, "    enabled: True") == 1);
}
