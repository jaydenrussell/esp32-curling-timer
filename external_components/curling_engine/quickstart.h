#pragma once

// Port of pitimer v2.4.4 editor.py quick start section: add/remove a timer
// starting now on today's exception. Error style: false + message in *err,
// messages ported verbatim (tests assert several).

#include <optional>
#include <string>

#include "model.h"

namespace esphome {
namespace curling_engine {

struct QuickStartOptions {
  std::string date_iso;  // today (YYYY-MM-DD)
  int now_sec{0};        // seconds since midnight: the new game starts here
  int duration{0};
  std::optional<std::string> name;
  bool pace_of_play{false};
  bool replace_conflicts{false};
  std::optional<std::string> during_buzzer;
  std::optional<int> header_hold;
  int hold{0};  // file wide config.buzzer.header_hold
  int pregame_countdown{0};
  bool allow_midnight_wrap{false};  // seconds of countdown before game starts
};

// Add a game starting now lasting duration seconds to today's exception in
// model (copying the weekday schedule into a new exception first, because an
// exception replaces the whole day). Returns the new entry.
bool quick_start(ScheduleModel *model, const QuickStartOptions &opts, Entry *out,
                 std::string *err);

// Remove the quick start entry starting at start from date_iso's exception;
// drops the exception date itself when it only repeats the weekday schedule.
bool cancel_quick_start(ScheduleModel *model, const std::string &date_iso, int start,
                        std::string *err);

// e.g. '25 min', '24 min 59 s', '45 s'
std::string format_duration(int seconds);

}  // namespace curling_engine
}  // namespace esphome
