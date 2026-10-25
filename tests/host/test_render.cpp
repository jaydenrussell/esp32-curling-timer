// Port of the pitimer display.py rendering rules (format_countdown,
// pace_status, header_event, colour names, draw_screen gating) that feed the
// LVGL page via CurlingEngineComponent::display_snapshot / render_snapshot.
#include "doctest.h"

#include <string>

#include "../../external_components/curling_engine/default_settings.h"
#include "../../external_components/curling_engine/render.h"
#include "../../external_components/curling_engine/settings.h"
#include "generated_fixtures.h"

using esphome::curling_engine::color_to_hex;
using esphome::curling_engine::format_countdown;
using esphome::curling_engine::header_event;
using esphome::curling_engine::pace_position;
using esphome::curling_engine::PiTimerGame;
using esphome::curling_engine::PiTimerSettings;
using esphome::curling_engine::RAISED_COLON;
using esphome::curling_engine::recolor;
using esphome::curling_engine::render_snapshot;
using esphome::curling_engine::DisplaySnapshot;

namespace {

constexpr const char *COLON = "\u02F8";

PiTimerSettings parse_base() {
  PiTimerSettings s;
  std::string err;
  bool ok = s.parse_settings_text(CASE_BASE, "settings.yml", &err);
  REQUIRE(ok);
  return s;
}

// CASE_BASE: Monday game start 36000 (10:00), buzzer 39600 (11:00), hold 300,
// pace 900s END, footer datetime+entry_name on, colours green/red/white/yellow.

}  // namespace

TEST_CASE("color_to_hex names, case, hex forms, fallback") {
  CHECK(color_to_hex("red") == "#0000FF");
  CHECK(color_to_hex("Red") == "#0000FF");
  CHECK(color_to_hex("GREEN") == "#008000");  // CSS green is 0x80 not 0xFF00
  CHECK(color_to_hex("white") == "#FFFFFF");
  CHECK(color_to_hex("grey") == "#808080");
  CHECK(color_to_hex("yellow") == "#00FFFF");
  CHECK(color_to_hex("#ff0000") == "#0000FF");
  CHECK(color_to_hex("#0f0") == "#00FF00");
  CHECK(color_to_hex("  darkslategray ") == "#4F4F2F");
  CHECK(color_to_hex("no-such-colour") == "#FFFFFF");
  CHECK(color_to_hex("no-such-colour", "#123456") == "#123456");
  CHECK(color_to_hex("#12") == "#FFFFFF");
}

TEST_CASE("format_countdown matches pitimer (raised colon)") {
  CHECK(format_countdown(0) == std::string("00") + COLON + "00");
  CHECK(format_countdown(-42) == std::string("00") + COLON + "00");
  CHECK(format_countdown(5) == std::string("00") + COLON + "05");
  CHECK(format_countdown(65) == std::string("01") + COLON + "05");
  CHECK(format_countdown(599) == std::string("09") + COLON + "59");
  CHECK(format_countdown(3599) == std::string("59") + COLON + "59");
  CHECK(format_countdown(3600) == std::string("1") + COLON + "00" + COLON + "00");
  CHECK(format_countdown(3661) == std::string("1") + COLON + "01" + COLON + "01");
  CHECK(format_countdown(10800) == std::string("3") + COLON + "00" + COLON + "00");
}

TEST_CASE("pace_position matches pitimer edge windows") {
  int unit = 0;
  CHECK(pace_position(0, 900, &unit) == "starting");
  CHECK(unit == 1);
  CHECK(pace_position(179, 900, &unit) == "starting");
  CHECK(pace_position(180, 900, &unit) == "playing");
  CHECK(pace_position(720, 900, &unit) == "playing");
  CHECK(pace_position(721, 900, &unit) == "finishing");
  CHECK(pace_position(899, 900, &unit) == "finishing");
  CHECK(pace_position(900, 900, &unit) == "starting");  // next unit
  CHECK(unit == 2);
  CHECK(pace_position(1000, 900, &unit) == "starting");
  CHECK(unit == 2);
  CHECK(pace_position(50, 0, &unit) == "playing");  // guarded
  CHECK(unit == 1);
}

TEST_CASE("header_event gating") {
  auto s = parse_base();
  std::optional<PiTimerGame> none;
  CHECK(header_event(s, none) == "when_idle");
  s.enabled = false;
  CHECK(header_event(s, none) == "when_disabled");
  s.enabled = true;
  PiTimerGame g;
  g.seconds_until_buzzer = 42;
  CHECK(header_event(s, g) == "counting_down");
  g.seconds_until_buzzer = 0;
  CHECK(header_event(s, g) == "during_buzzer");
  g.seconds_until_buzzer = -100;  // inside header hold
  CHECK(header_event(s, g) == "during_buzzer");
}

TEST_CASE("recolor wraps text for LVGL") {
  CHECK(recolor("#0000FF", "GO") == "#0000FF GO#");
  CHECK(recolor("#00FFFF", "") == "");
  CHECK(recolor("", "GO") == "GO");
}

TEST_CASE("render_snapshot: unknown clock shows waiting status only") {
  auto s = parse_base();
  auto snap = render_snapshot(s, false, 0, 36900, "2026-09-28");
  CHECK(snap.valid);
  CHECK_FALSE(snap.clock_known);
  CHECK_FALSE(snap.show_countdown);
  CHECK_FALSE(snap.show_date_time);
  CHECK(snap.header_text == "IDLE");
  CHECK(snap.header_color == "#FFFFFF");
  REQUIRE(snap.show_status);
  CHECK(snap.status_text == std::string("WAITING FOR TIME") + COLON);
  CHECK(snap.status_color == "#FFFF00");
}

TEST_CASE("render_snapshot: counting down with pace and footer") {
  auto s = parse_base();
  auto snap = render_snapshot(s, true, 0, 36900, "2026-09-28");  // Mon 10:15
  CHECK(snap.valid);
  CHECK(snap.header_text == "COUNTING");
  CHECK(snap.header_color == "#00FFFF");
  REQUIRE(snap.show_countdown);
  CHECK_FALSE(snap.during_buzzer);
  CHECK(snap.seconds_until_buzzer == 2700);
  CHECK(snap.countdown == std::string("45") + COLON + "00");
  CHECK(snap.countdown_color == "#008000");
  REQUIRE(snap.show_pace);
  // since_start 900 -> unit 2, into 0 -> starting
  CHECK(snap.pace_text == "Pace is starting END 2");
  CHECK(snap.pace_color == "#FFFFFF");
  REQUIRE(snap.show_game_name);
  CHECK(snap.game_name == "MONDAY GAME");
  REQUIRE(snap.show_date_time);
  CHECK(snap.date_text == "2026-SEP-28");
  CHECK(snap.time_text == std::string("10") + COLON + "15" + COLON + "00");
}

TEST_CASE("render_snapshot: five-digit countdown at one hour") {
  auto s = parse_base();
  auto snap = render_snapshot(s, true, 0, 36000, "2026-09-28");  // game start
  REQUIRE(snap.show_countdown);
  CHECK(snap.countdown == std::string("1") + COLON + "00" + COLON + "00");
}

TEST_CASE("render_snapshot: during buzzer red + header override window") {
  auto s = parse_base();
  auto snap = render_snapshot(s, true, 0, 39600, "2026-09-28");  // buzzer second
  REQUIRE(snap.show_countdown);
  CHECK(snap.during_buzzer);
  CHECK(snap.seconds_until_buzzer == 0);
  CHECK(snap.countdown == std::string("00") + COLON + "00");
  CHECK(snap.countdown_color == "#0000FF");
  CHECK(snap.header_text == "LAST END");
  CHECK(snap.header_color == "#00FFFF");

  // Inside header hold (39600 + 300): still during_buzzer, negative remaining.
  auto hold = render_snapshot(s, true, 0, 39750, "2026-09-28");
  REQUIRE(hold.show_countdown);
  CHECK(hold.during_buzzer);
  CHECK(hold.seconds_until_buzzer == -150);
}

TEST_CASE("render_snapshot: game window edges (idle after hold)") {
  auto s = parse_base();
  auto before = render_snapshot(s, true, 0, 35989, "2026-09-28");  // Mon 09:59:49
  CHECK_FALSE(before.show_countdown);
  CHECK(before.header_text == "IDLE");
  CHECK_FALSE(before.show_game_name);

  auto after = render_snapshot(s, true, 0, 39901, "2026-09-28");  // past hold
  CHECK_FALSE(after.show_countdown);
  CHECK(after.header_text == "IDLE");

  auto wrong_day = render_snapshot(s, true, 1, 36900, "2026-09-29");  // Tuesday
  CHECK_FALSE(wrong_day.show_countdown);
  CHECK(wrong_day.header_text == "IDLE");
}

TEST_CASE("render_snapshot: exception day overrides schedule") {
  auto s = parse_base();
  // 2026-10-05 is a Monday; exceptions entry start 43200 buzzer 45000 "SPECIAL".
  auto snap = render_snapshot(s, true, 0, 44000, "2026-10-05");
  REQUIRE(snap.show_countdown);
  CHECK(snap.countdown == std::string("16") + COLON + "40");
  REQUIRE(snap.show_game_name);
  CHECK(snap.game_name == "SPECIAL");
}

TEST_CASE("render_snapshot: disabled hides countdown, red header") {
  auto s = parse_base();
  s.enabled = false;
  auto snap = render_snapshot(s, true, 0, 36900, "2026-09-28");
  CHECK_FALSE(snap.show_countdown);
  CHECK_FALSE(snap.show_pace);
  CHECK_FALSE(snap.show_game_name);
  CHECK(snap.header_text == "DISABLED");
  CHECK(snap.header_color == "#0000FF");
}

TEST_CASE("render_snapshot: footer switches off via settings flags") {
  auto s = parse_base();
  s.date_and_time = false;
  s.game_name = false;
  auto snap = render_snapshot(s, true, 0, 36900, "2026-09-28");
  REQUIRE(snap.show_countdown);
  CHECK_FALSE(snap.show_date_time);
  CHECK_FALSE(snap.show_game_name);
  CHECK(snap.show_pace);  // pace flag untouched by footer switches
}

TEST_CASE("render_snapshot: during_buzzer_override replaces header text") {
  PiTimerSettings s;
  s.valid = true;
  s.enabled = true;
  s.headers["text"]["when_idle"] = "IDLE";
  s.headers["text"]["counting_down"] = "COUNTING";
  s.headers["text"]["during_buzzer"] = "LAST END";
  s.headers["color"]["when_idle"] = "white";
  s.headers["color"]["counting_down"] = "yellow";
  s.headers["color"]["during_buzzer"] = "yellow";
  s.countdown_colors["counting_down"] = "green";
  s.countdown_colors["during_buzzer"] = "red";
  PiTimerGame g;
  g.start = 36000;
  g.buzzer = 39600;
  g.during_buzzer_override = "PLAYOFF FLASH";
  s.schedules["Monday"] = {g};
  auto snap = render_snapshot(s, true, 0, 39600, "2026-09-28");
  CHECK(snap.header_text == "PLAYOFF FLASH");
}

TEST_CASE("render_snapshot: invalid settings yields empty snapshot") {
  PiTimerSettings s;  // never parsed
  auto snap = render_snapshot(s, true, 0, 36900, "2026-09-28");
  CHECK_FALSE(snap.valid);
  CHECK_FALSE(snap.show_countdown);
  CHECK(snap.header_text.empty());
}
