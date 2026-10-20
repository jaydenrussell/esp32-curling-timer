#pragma once

#include "esphome/core/component.h"
#include <string>
#include <vector>
#include <map>

namespace esphome {
namespace curling_engine {

// Port of the pitimer v2.4.4 engine (rpi-buzzer):
//   settings.py  — schedule validation + lookup
//   display.py   — per-second tick, buzz-once rules, quick-start logic
//   editor.py    — splice-only config editing
//   web.py       — REST endpoints / auth (separate web-editor component)
//
// Key semantics preserved from the Python firmware:
//   * bell rings once per (date, start) pair, 5s grace window
//   * game duration cap: 3 hours
//   * overlap validation includes per-entry header_hold
//   * no leading zeros in H:MM:SS; hour 0 stored as raw seconds
//   * wake each second inside WAKE_MARGIN_MS of the tick
//   * splice editing keeps non-schedule sections byte-for-byte

static const int BUZZER_GRACE_SECONDS = 5;
static const int WAKE_MARGIN_MS = 5;

struct ExceptionDay {
  std::string date;  // YYYY-MM-DD
  std::string kind;  // closed / custom schedule
};

struct GameEntry {
  std::string start;   // H:MM:SS, no leading zero on H
  int duration_s{0};
  int header_hold_s{0};
};

struct Settings {
  std::vector<GameEntry> schedule;
  std::vector<ExceptionDay> exceptions;
  int version{1};
  // WIP(v1): team names, end-marker config, pace-of-play thresholds,
  // quick-start presets (PRACTICE 5m w/ during_buzzer + header_hold 60;
  // 6-end game 75m; 8-end game 105m)
};

bool validate_times(const Settings &s, std::string *error);
bool has_overlap(const Settings &s, std::string *error);
// WIP(v1): tick(), buzz scheduling state, NVS-backed store with atomic
// fixup (30 rolling backups + version hash), splice editor.

std::string serialize_settings(const Settings &s);
bool parse_settings(const std::string &body, Settings *out, std::string *error);

class CurlingEngineComponent : public Component {
 public:
  void set_store_path(const std::string &path) { store_path_ = path; }
  void setup() override;
  void dump_config() override;

 protected:
  std::string store_path_;
};

}  // namespace curling_engine
}  // namespace esphome
