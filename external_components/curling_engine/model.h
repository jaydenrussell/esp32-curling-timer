#pragma once

// Port of pitimer v2.4.4 editor.py (model layer): schedule/exception editing
// over the settings-file TEXT, so comments and the config section survive a
// save byte for byte. Error style: -fno-exceptions, EditorError -> false +
// message in *err, messages ported verbatim (tests assert several).
//
// Scope note: structured load/splice/set_enabled live here; quick_start and
// cancel_quick_start live in quickstart.h; SettingsStore in store.h.

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "settings.h"

namespace esphome {
namespace curling_engine {

static constexpr const char *QUICK_NOTE = "quick start via web UI";
static constexpr const char *QUICK_COMMENT = "# quick start";
static constexpr const char *QUICK_DEFAULT_NAME = "QUICK START";

struct Entry {
  int start{0};
  int buzzer{0};
  std::optional<std::string> name;
  std::optional<std::string> during_buzzer;
  std::optional<int> header_hold;
  std::optional<bool> pace_of_play;
  std::optional<int> pace_unit_duration;
  // entry added by a quick start (marked with a trailing comment in the file)
  bool quick{false};

  bool operator==(const Entry &) const = default;
};

struct ExceptionDay {
  std::string date;
  std::string note;
  std::vector<Entry> entries;

  bool operator==(const ExceptionDay &) const = default;
};

struct ScheduleModel {
  std::map<std::string, std::vector<Entry>> schedule;
  std::vector<ExceptionDay> exceptions;
  // comment lines inside the two sections, which a structured save drops
  int inner_comments{0};

  const ExceptionDay *exception_for(const std::string &date) const;
  ExceptionDay *exception_for(const std::string &date);
  // the entries in effect on date_iso (its exception, else its weekday list)
  std::vector<Entry> entries_for(const std::string &date_iso) const;
};

// Parse the schedule and exceptions out of settings file text. Fails when the
// file cannot be edited structurally (use the raw editor then).
bool load_model(const std::string &text, ScheduleModel *out, std::string *err);

// Return text with its schedule and exceptions sections regenerated from
// model. Everything outside those sections is kept byte for byte, as is each
// exception date line whose note and empty/non-empty state did not change.
bool splice(const std::string &text, const ScheduleModel &model, std::string *out,
            std::string *err);

// Return text with config.enabled set, keeping any comment on that line.
bool set_enabled(const std::string &text, bool enabled, std::string *out, std::string *err);

// Seconds since midnight as the YAML 1.1 sexagesimal H:MM:SS the file uses
// (midnight hour times are plain seconds; hours are space padded).
std::string format_time(int seconds);

// One entry as the one-line flow mapping used in the settings file.
std::string render_entry(const Entry &entry);

// 0 = Monday; date_iso must be a real YYYY-MM-DD (same weekday convention as
// PiTimerSettings::find_current_game), -1 when unparsable.
int weekday_mon0_from_iso(const std::string &date_iso);

// How long entry holds its 'during_buzzer' header: its own override, else the
// file wide hold (editor.py _hold_for).
int entry_hold(const Entry &entry, int default_hold);

// Same overlap rule as PiTimerSettings.parse_schedules: each end counts its
// own entry's header hold (editor.py _overlaps).
bool entries_overlap(const Entry &first, const Entry &second, int default_hold);

// Shared text utilities (used by quickstart/store and the tests).
std::vector<std::string> split_lines(const std::string &text, std::string *newline);
bool section_span(const std::vector<std::string> &lines, const std::string &key, size_t *start,
                  size_t *end, std::string *err);

}  // namespace curling_engine
}  // namespace esphome
