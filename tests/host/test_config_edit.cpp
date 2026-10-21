#include "doctest.h"

#include <cstdio>
#include <string>
#include <vector>

#include "../../external_components/curling_engine/config_edit.h"
#include "../../external_components/curling_engine/settings.h"
#include "generated_fixtures.h"

using esphome::curling_engine::apply_config_values;
using esphome::curling_engine::ConfigValue;
using esphome::curling_engine::PiTimerSettings;
using esphome::curling_engine::yaml_bool;
using esphome::curling_engine::yaml_int;
using esphome::curling_engine::yaml_quote;
using esphome::curling_engine::yaml_relays;

TEST_CASE ("encoders match the pitimer YAML dialect") {
  CHECK(yaml_bool(true) == "True");
  CHECK(yaml_bool(false) == "False");
  CHECK(yaml_int(-5) == "-5");
  CHECK(yaml_quote("CLUB NAME") == "'CLUB NAME'");
  CHECK(yaml_quote("it's") == "'it''s'");
  CHECK(yaml_relays({}) == "[]");
  CHECK(yaml_relays({"RELAY1"}) == "['RELAY1']");
  CHECK(yaml_relays({"RELAY1", "RELAY4"}) == "['RELAY1', 'RELAY4']");
}

TEST_CASE ("apply_config_values edits scalars and keeps the file valid") {
  std::string out, err;
  const std::vector<ConfigValue> values = {
      {"config.enabled", yaml_bool(false)},
      {"config.header.font_size", yaml_int(256)},
      {"config.header.text.when_idle", yaml_quote("O'MALLEY'S")},
      {"config.header.color.counting_down", yaml_quote("blue")},
      {"config.countdown.font_size_4_digits", yaml_int(1408)},
      {"config.countdown.color.during_buzzer", yaml_quote("red")},
      {"config.pace_of_play.enabled", yaml_bool(false)},
      {"config.pace_of_play.unit_duration", yaml_int(720)},
      {"config.footer.datetime", yaml_bool(false)},
      {"config.buzzer.relays", yaml_relays({"RELAY2", "RELAY1"})},
      {"config.buzzer.header_hold", yaml_int(60)},
  };
  REQUIRE(apply_config_values(TEXT_SETTINGS_TEXT, values, &out, &err));
  CHECK(out != TEXT_SETTINGS_TEXT);

  PiTimerSettings s;
  if (!s.parse_settings_text(out, "pitimer_settings.yml", &err)) {
    std::printf("ERR: %s\n", err.c_str());
    const size_t pos = out.find("config:");
    const size_t n = pos == std::string::npos ? 0 : std::min<size_t>(900, out.size() - pos);
    std::printf("OUT:\n%.*s\n", static_cast<int>(n), out.c_str() + pos);
    FAIL("parse failed");
  }
  CHECK_FALSE(s.enabled);
  CHECK(s.font_sizes["header"] == 256);
  CHECK(s.headers["text"]["when_idle"] == "O'MALLEY'S");
  CHECK(s.headers["color"]["counting_down"] == "blue");
  CHECK(s.font_sizes["timer_4_digits"] == 1408);
  CHECK(s.countdown_colors["during_buzzer"] == "red");
  CHECK_FALSE(s.pace_of_play);
  CHECK(s.pace_unit_duration == 720);
  CHECK_FALSE(s.date_and_time);
  REQUIRE(s.relays.has_value());
  CHECK(*s.relays == std::vector<std::string>{"RELAY2", "RELAY1"});
  CHECK(s.relay_msg_time == 60);
  // schedule/exceptions untouched
  CHECK(s.schedules.size() == 7);
  CHECK(s.exceptions.size() == 2);
}

TEST_CASE ("apply_config_values keeps trailing comments and indentation") {
  const std::string text =
      "config:\n"
      "    enabled: True # global switch\n"
      "    buzzer:\n"
      "        header_hold: 299   # hold the LAST END header\n"
      "schedule:\n"
      "    Monday: []\n";
  std::string out, err;
  REQUIRE(apply_config_values(text, {{"config.enabled", yaml_bool(false)},
                                     {"config.buzzer.header_hold", yaml_int(120)}},
                              &out, &err));
  CHECK(out ==
        "config:\n"
        "    enabled: False # global switch\n"
        "    buzzer:\n"
        "        header_hold: 120   # hold the LAST END header\n"
        "schedule:\n"
        "    Monday: []\n");
}

TEST_CASE ("apply_config_values scopes keys by parent (pace enabled vs config enabled)") {
  const std::string text =
      "config:\n"
      "    enabled: True\n"
      "    pace_of_play:\n"
      "        enabled: True\n"
      "schedule:\n"
      "    Monday: []\n";
  std::string out, err;
  REQUIRE(apply_config_values(text, {{"config.pace_of_play.enabled", yaml_bool(false)}}, &out, &err));
  CHECK(out ==
        "config:\n"
        "    enabled: True\n"
        "    pace_of_play:\n"
        "        enabled: False\n"
        "schedule:\n"
        "    Monday: []\n");
  REQUIRE(apply_config_values(text, {{"config.enabled", yaml_bool(false)}}, &out, &err));
  CHECK(out ==
        "config:\n"
        "    enabled: False\n"
        "    pace_of_play:\n"
        "        enabled: True\n"
        "schedule:\n"
        "    Monday: []\n");
}

TEST_CASE ("apply_config_values rejects unknown paths without touching the text") {
  std::string out, err;
  CHECK_FALSE(apply_config_values(TEXT_SETTINGS_TEXT,
                                  {{"config.nope.missing", "1"}}, &out, &err));
  CHECK(out.empty());
  CHECK(err == "no config setting 'config.nope.missing' in the settings file");
  CHECK_FALSE(apply_config_values(TEXT_SETTINGS_TEXT, {{"enabled", "False"}}, &out, &err));
  CHECK(err == "no config setting 'enabled' in the settings file");
  CHECK_FALSE(apply_config_values("schedule:\n    Monday: []\n",
                                  {{"config.enabled", "False"}}, &out, &err));
  CHECK(err == "settings file has no top level 'config:' section");
}

TEST_CASE ("apply_config_values preserves CRLF and inner schedule comments") {
  std::string text = TEXT_SETTINGS_TEXT;
  // fixture uses \n; make a CRLF copy with an inner comment
  text.push_back('\n');
  std::string crlf;
  for (char c : text) {
    if (c == '\n')
      crlf += "\r\n";
    else
      crlf.push_back(c);
  }
  std::string out, err;
  REQUIRE(apply_config_values(crlf, {{"config.footer.font_size", yaml_int(128)}}, &out, &err));
  CHECK(out.find("\r\n") != std::string::npos);
  PiTimerSettings s;
  REQUIRE(s.parse_settings_text(out, "pitimer_settings.yml", &err));
  CHECK(s.font_sizes["footer"] == 128);
}

TEST_CASE("apply_config_values inserts newer keys missing from older files") {
  // TEXT_SETTINGS_TEXT predates the ring events: buzzer has no ring_at_start
  // and no countdown block. Saving the new fields must insert them rather
  // than fail with "no config setting".
  std::string out, err;
  const std::vector<ConfigValue> values = {
      {"config.buzzer.ring_at_start", yaml_relays({"RELAY2"})},
      {"config.buzzer.countdown.minutes_before", yaml_int(2)},
      {"config.buzzer.countdown.relays", yaml_relays({"RELAY1", "RELAY2"})},
      {"config.buzzer.duration", yaml_int(3)},
  };
  REQUIRE(apply_config_values(TEXT_SETTINGS_TEXT, values, &out, &err));
  CHECK(out != TEXT_SETTINGS_TEXT);
  // New keys must sit inside the buzzer block, before the schedule section.
  const size_t buzzer = out.find("buzzer:");
  const size_t start = out.find("ring_at_start: ['RELAY2']");
  const size_t minutes = out.find("minutes_before: 2");
  const size_t relays = out.find("relays: ['RELAY1', 'RELAY2']");
  const size_t schedule = out.find("schedule:");
  REQUIRE(buzzer != std::string::npos);
  CHECK(start > buzzer);
  CHECK(minutes > buzzer);
  CHECK(relays > buzzer);
  REQUIRE(schedule != std::string::npos);
  CHECK(start < schedule);
  CHECK(minutes < schedule);
  CHECK(relays < schedule);

  PiTimerSettings s;
  if (!s.parse_settings_text(out, "pitimer_settings.yml", &err)) {
    std::printf("ERR: %s\n", err.c_str());
    FAIL("parse failed");
  }
  REQUIRE(s.ring_at_start.has_value());
  CHECK(*s.ring_at_start == std::vector<std::string>{"RELAY2"});
  CHECK(s.countdown_minutes_before == 2);
  CHECK(s.countdown_relays == std::vector<std::string>({"RELAY1", "RELAY2"}));
  CHECK(s.relay_duration == 3);
  // Existing keys still editable in the same save.
  CHECK(s.relay_msg_time == 300);
}

TEST_CASE("apply_config_values inserts into a buzzer block with a trailing comment") {
  // Complete settings file (CASE_BASE) with an outdented comment injected
  // between the buzzer block and the schedule section.
  std::string text(CASE_BASE);
  const size_t sched = text.find("schedule:");
  REQUIRE(sched != std::string::npos);
  text.insert(sched, "# ---- schedule below ----\n");
  std::string out, err;
  REQUIRE(apply_config_values(text, {{"config.buzzer.ring_at_start", yaml_relays({"RELAY2"})},
                                     {"config.buzzer.countdown.minutes_before", yaml_int(1)}},
                              &out, &err));
  const size_t hold = out.find("header_hold: 300");
  const size_t start = out.find("ring_at_start: ['RELAY2']");
  const size_t minutes = out.find("minutes_before: 1");
  REQUIRE(hold != std::string::npos);
  REQUIRE(start != std::string::npos);
  REQUIRE(minutes != std::string::npos);
  // Inserted after the block's last real key, before the outdented comment.
  CHECK(start > hold);
  CHECK(minutes > hold);
  CHECK(out.find("# ---- schedule below ----") > minutes);
  // Indentation matches the buzzer block's existing children (4-space style).
  const size_t line_start = out.rfind('\n', start) + 1;
  CHECK(out.compare(line_start, 4, "    ") == 0);
  PiTimerSettings s;
  if (!s.parse_settings_text(out, "settings.yml", &err)) {
    std::printf("ERR: %s\n", err.c_str());
    FAIL("parse failed");
  }
  REQUIRE(s.ring_at_start.has_value());
  CHECK(s.countdown_minutes_before == 1);
  // The countdown section needs both keys: the missing one is completed from
  // the current file (no countdown section yet -> 0 minutes, no relays).
  CHECK(s.countdown_relays.empty());
}
