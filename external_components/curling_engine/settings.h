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

struct LeagueProfile {
  std::string name;
  int game_duration_minutes{0};
  int pregame_countdown_seconds{0};
  std::string header_text;
  bool pace_enabled{false};
  std::vector<std::string> buzzer_relays;
  int countdown_minutes_before{0};
  std::string format;
  int team_size{0};
  // per-end game configuration for progress bar display
  int stones_per_end{8};
  int end_count{8};
  int untimed_ends{0};
  bool show_progress_through_end{true};
  // per-event output override for games in this league
  std::vector<std::string> start_relays;
  int start_duration{0};
  std::vector<std::string> end_relays;
  int end_duration{0};
  // manual push-button trigger required for games in this league
  bool manual_trigger{false};
};

struct PiTimerGame {
  int start{0};
  int buzzer{0};
  std::optional<std::string> message;                // name
  std::optional<std::string> game_completed_override; // game_completed
  std::optional<int> header_hold_override;
  std::optional<bool> pace_of_play_override;
  std::optional<int> pace_unit_override;
  // per-entry output override: which relays ring at game start / buzzer, and
  // for how many seconds (absent = use the settings file's buzzer config)
  std::optional<std::vector<std::string>> start_relays_override;
  std::optional<std::vector<std::string>> end_relays_override;
  std::optional<int> start_duration_override;
  std::optional<int> end_duration_override;
  // quick-start game details
  std::optional<int> end_count;
  std::optional<int> stones_per_end;
  std::optional<int> untimed_ends;
  std::optional<bool> show_progress_through_end;
  // per-game pre-game countdown override (seconds; absent = use global)
  std::optional<int> pregame_countdown_override;
  // live state, set on the copy returned by find_current_game
  std::optional<int> seconds_until_buzzer;
  std::optional<int> seconds_since_start;
  // manual push-button trigger required for this game (default rule:
  // start >= 17:00, overridable per league or per entry)
  bool manual_trigger{false};
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
  int relay_start_duration{0};
  int relay_countdown_duration{0};
  int relay_end_duration{0};
  // Per-relay output duration overrides (seconds). When set for a relay,
  // that relay rings for its own duration instead of the event-level value.
  int relay_start_duration_1{0};
  int relay_start_duration_2{0};
  int relay_start_duration_3{0};
  int relay_start_duration_4{0};
  int relay_countdown_duration_1{0};
  int relay_countdown_duration_2{0};
  int relay_countdown_duration_3{0};
  int relay_countdown_duration_4{0};
  int relay_end_duration_1{0};
  int relay_end_duration_2{0};
  int relay_end_duration_3{0};
  int relay_end_duration_4{0};
  int relay_msg_time{0};
  int pregame_countdown{0};
  bool pregame_blink{false};
  bool allow_midnight_wrap{false};
  int pregame_font_size{0};
  int mirror_scale{100};
  std::string pregame_header_color{};
  std::string pregame_countdown_color{};
  int date_font_size{52};
  std::string date_color{};
  int time_font_size{52};
  std::string time_color{};
  std::string footer_color{};
  std::string background_color{"black"};
  std::string pregame_header_text{"Get ready"};
  bool relays_enabled{true};
  int relay_count{4};
  int manual_trigger_gpio{0};
  std::string relay1_function{"None"};
  std::string relay2_function{"None"};
  std::string relay3_function{"None"};
  std::string relay4_function{"None"};
  bool relay1_enabled{true};
  bool relay2_enabled{true};
  bool relay3_enabled{true};
  bool relay4_enabled{true};
  std::map<std::string, std::vector<PiTimerGame>> schedules;
  std::map<std::string, std::vector<PiTimerGame>> exceptions;
  std::vector<LeagueProfile> leagues;
  int log_level{3};
  bool valid{false};

  // Add-on hardware enable flags — web UI configurable, persisted in the
  // settings file under config.addons.*. Compiled-in drivers are gated at
  // runtime by these flags so a pre-built firmware works with any subset
  // of addon hardware.
  struct AddonConfig {
    bool rtc{false};        // M5Stack RTC Unit (HYM8563, SKU U126)
    bool relays{true};      // Unit 2Relay outputs
    bool pushbutton{true};  // GPIO manual trigger button
    bool pahub{false};      // PaHub v2.1 I2C hub
    bool wifi{false};       // ESP32-C6 SDIO Wi-Fi failover
    bool uart{false};       // Hat2-Bus UART0 serial port
  };
  AddonConfig addons;

  // Runtime add-on hardware detection (recomputed every boot in
  // CurlingEngineComponent::detect_addons(); not persisted). Effective enable:
  //   auto-detect add-ons (rtc, pahub, wifi): user_enabled && detected
  //   manual add-ons (relays, pushbutton, uart): user_enabled only
  struct AddonDetection {
    bool rtc{false};
    bool relays{false};     // manual only — always false
    bool pushbutton{false}; // manual only — always false
    bool pahub{false};
    bool wifi{false};
    bool uart{false};       // manual only — always false
    uint32_t detected_at_ms{0};
  };

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
