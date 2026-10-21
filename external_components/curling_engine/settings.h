#pragma once

// Port of pitimer v2.4.4 settings.py: YAML-subset parsing, validation and
// game lookup for the settings file dialect the web UI produces/consumes.
//
// Error style note: ESPHome builds with -fno-exceptions, so every Python
// `raise SettingsError(...)` becomes `return false` + a message in *err.
// Messages are ported verbatim from the Python sources (tests assert on
// several of them). Validity follows Python: valid=false until a parse
// completes, set true only at the end.

#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace esphome {
namespace curling_engine {

// ---------------------------------------------------------------------------
// YAML value model (YAML 1.1 subset used by pitimer settings files)
// ---------------------------------------------------------------------------

enum class YType { Null, Bool, Int, Float, Str, Seq, Map };

struct YamlValue {
  YType type{YType::Null};
  bool b{false};
  long long i{0};
  double d{0};
  std::string s;  // Str payload
  std::vector<YamlValue> seq;
  std::vector<std::pair<std::string, YamlValue>> map;  // insertion order kept

  const YamlValue *find(const std::string &key) const;
  bool has(const std::string &key) const { return find(key) != nullptr; }
};

// Parse settings-file text. On failure returns false and puts a
// "line N: ..." message in *err. Supports the dialect of files written by
// pitimer/the web UI (block maps/seqs, flow collections, quoted/plain/block
// scalars); anchors, aliases, tags and directives are rejected explicitly.
bool yaml_parse(const std::string &text, YamlValue *out, std::string *err);

// Python-style repr of a parsed value, for error messages ('foo', True, 1.5).
std::string yaml_repr(const YamlValue &v);

// ---------------------------------------------------------------------------
// Domain structures (mirror pitimer settings.py)
// ---------------------------------------------------------------------------

static constexpr int MAX_GAME_SECONDS = 3 * 60 * 60;
static constexpr int SECONDS_PER_DAY = 24 * 60 * 60;
static constexpr const char *DAY_NAMES[7] = {"Monday",   "Tuesday", "Wednesday",
                                             "Thursday", "Friday",  "Saturday",
                                             "Sunday"};
static constexpr const char *VALID_RELAYS_REPR =
    "('RELAY1', 'RELAY2', 'RELAY3', 'RELAY4')";

struct PiTimerGame {
  int start{0};
  int buzzer{0};
  std::optional<std::string> message;                // name
  std::optional<std::string> during_buzzer_override; // during_buzzer
  std::optional<int> header_hold_override;
  std::optional<bool> pace_of_play_override;
  std::optional<int> pace_unit_override;
  // live state, set on the copy returned by find_current_game
  std::optional<int> seconds_until_buzzer;
  std::optional<int> seconds_since_start;
};

class PiTimerSettings {
 public:
  PiTimerSettings() { reset(); }

  std::string settings_fn;
  bool enabled{false};
  std::map<std::string, int> font_sizes;
  // headers["text"]["when_idle"]
  std::map<std::string, std::map<std::string, std::string>> headers;
  std::map<std::string, std::string> countdown_colors;
  bool pace_of_play{false};
  std::string pace_color;
  std::string pace_unit_name;
  int pace_unit_duration{0};
  bool date_and_time{false};
  bool game_name{false};
  std::optional<std::vector<std::string>> relays;
  // Optional ring events (config.buzzer): absent/empty lists and a zero
  // countdown minutes_before disable each event — see default_settings.h.
  std::optional<std::vector<std::string>> ring_at_start; // relays pulsed at game start
  int countdown_minutes_before{0};       // minutes before buzzer (0 = off)
  std::vector<std::string> countdown_relays;            // relays pulsed at that moment
  int relay_duration{0};
  int relay_msg_time{0};
  std::map<std::string, std::vector<PiTimerGame>> schedules;
  std::map<std::string, std::vector<PiTimerGame>> exceptions;
  bool valid{false};

  // Restore the pre-parse defaults (also called by the constructor so two
  // instances never share state).
  void reset();

  // Python: PiTimerSettings.header_hold
  int header_hold(const PiTimerGame &game) const;

  // Python: parse_settings_file — suffix check, read, parse, validate.
  bool parse_settings_file(const std::string &path, std::string *err);

  // Same, but for in-memory text (store validation, tests).
  bool parse_settings_text(const std::string &text, const std::string &filename,
                           std::string *err);

  // Python: find_current_game — the game in progress at the given local time,
  // including header hold. weekday: 0 = Monday. date_iso: YYYY-MM-DD.
  std::optional<PiTimerGame> find_current_game(int weekday,
                                               int seconds_since_midnight,
                                               const std::string &date_iso) const;
};

// Filename rule from settings.py (both parsers/stores rely on it).
bool settings_filename_ok(const std::string &path, std::string *err);

}  // namespace curling_engine
}  // namespace esphome
