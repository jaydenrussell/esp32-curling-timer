#pragma once

// Port of pitimer display.py data prep (format_countdown, header_event,
// pace_status, draw_* colour/text selection) as a pure function producing the
// strings/colours the LVGL page renders. No ESPHome dependencies so it stays
// host-testable like settings/store/model.
//
// Colours: settings use pygame colour names or '#rrggbb' (pygame.Color
// accepts both). color_to_hex() normalises to "#RRGGBB" for LVGL's inline
// recolor syntax "#RRGGBB text#".

#include <optional>
#include <string>

#include "settings.h"

namespace esphome {
namespace curling_engine {

// pitimer display.py constants (720p-scaled usage in the LVGL page)
static constexpr const char *RAISED_COLON = "\u02F8";  // modifier letter raised colon
static constexpr const char *FOOTER_COLOR_HEX = "#FFFF00";  // display.py FOOTER_COLOR
static constexpr int PACE_EDGE_SECONDS = 3 * 60;

// pygame.Color-compatible name (CSS3/X11) or '#rgb'/'#rrggbb' -> "#RRGGBB"
// upper-case. Unknown names -> fallback (default white).
std::string color_to_hex(const std::string &name, const std::string &fallback = "#FFFFFF");

// pitimer display.py format_countdown: 00:00 when due, H:MM:SS at >=1h, else
// MM:SS - all with the raised-colon glyph.
std::string format_countdown(int seconds_until_buzzer);

// pitimer display.py pace_status: (position, 1-based unit number).
std::string pace_position(int seconds_since_start, int unit_duration, int *unit_number);

// pitimer display.py header_event: when_disabled | when_idle | counting_down |
// during_buzzer.
std::string header_event(const PiTimerSettings &settings,
                         const std::optional<PiTimerGame> &game);

// LVGL label recolor wrapper: "#RRGGBB text#" (empty text -> empty string).
std::string recolor(const std::string &hex, const std::string &text);

struct DisplaySnapshot {
  bool valid{false};       // settings parsed ok
  bool clock_known{false};  // RTC has a sane local time

  std::string header_text;
  std::string header_color;  // "#RRGGBB"

  bool show_countdown{false};
  bool during_buzzer{false};  // live state for callers (bell work later)
  int seconds_until_buzzer{0};
  std::string countdown;  // "MM:SS" with raised colons
  std::string countdown_color;

  bool show_pace{false};
  std::string pace_text;
  std::string pace_color;

  bool show_game_name{false};
  std::string game_name;  // footer centre (FOOTER_COLOR)

  bool show_date_time{false};
  std::string date_text;  // "2026-OCT-21"
  std::string time_text;  // "HH:MM:SS" raised colons (FOOTER_COLOR)

  // Error line / "waiting for time" line (centre, FOOTER_COLOR unless set).
  bool show_status{false};
  std::string status_text;
  std::string status_color{FOOTER_COLOR_HEX};

  std::string footer_color{FOOTER_COLOR_HEX};  // footer game name/date/time colour
};

// Pure draw_screen() data prep for valid settings: gates exactly like pitimer
// (enabled && game for countdown, header event, pace overrides, footer name,
// date/time flag) and normalises colours. !clock_known keeps the idle header
// and asks for a status line instead of the countdown/clock.
DisplaySnapshot render_snapshot(const PiTimerSettings &settings, bool clock_known,
                                int weekday_mon0, int seconds_of_day,
                                const std::string &date_iso);

}  // namespace curling_engine
}  // namespace esphome
