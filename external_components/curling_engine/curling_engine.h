#pragma once

#include <memory>
#include <string>

#include "bell.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/component.h"
#include "render.h"
#include "settings.h"
#include "store.h"

namespace esphome {
namespace curling_engine {

// Data half of the pitimer engine on-device:
//   settings.py  — parse/validate/status lookup   (settings.h/.cpp)
//   editor.py    — splice/model/quick start       (model.* / quickstart.*)
//   SettingsStore — versioned atomic file saves   (store.* + fs_posix.cpp)
//   web.py       — REST + auth                    (web_editor component)
// Display tick: display_snapshot (render.* = pitimer display.py draw_screen
// prep). Bell tick: bell_due (bell.* = buzz_if_due), driving the Unit 2Relay
// GPIO switches from the yaml side.
//
// Storage layout: partition label "storage" (added from __init__.py) mounted
// at store_path_ (default /flash) holding pitimer_settings.yml + backups/.
// FAT via wear levelling (SPIFFS VFS cannot mkdir/opendir for backups).

static constexpr const char *SETTINGS_BASENAME = "pitimer_settings.yml";

class CurlingEngineComponent : public Component {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_store_path(const std::string &path) { store_path_ = path; }
  void set_time(time::RealTimeClock *time) { time_ = time; }

  void setup() override;
  void dump_config() override;

  // --- interface for web_editor ---
  SettingsStore *get_store() { return store_.get(); }
  StoreFs *get_store_fs() { return fs_.get(); }
  bool ready() const { return ready_; }
  std::string settings_path() const { return store_path_ + "/" + SETTINGS_BASENAME; }
  time::RealTimeClock *get_time() { return time_; }

  // Local wall-clock snapshot: weekday Mon=0, seconds since midnight,
  // YYYY-MM-DD. False until the RTC has a sane time (login + quick-start gate).
  bool local_now(int *weekday_mon0, int *seconds_of_day, std::string *date_iso) const;

  // Convenience: read + version + parse the live settings in one shot.
  // Returns false on IO (store err) or invalid text (parse err).
  bool read_parsed(PiTimerSettings *settings, std::string *text, std::string *version,
                   std::string *err);

  // 1 Hz display tick (LVGL interval action): fresh read+parse of the live
  // settings + pitimer draw_screen data prep (render.cpp). Always fills *out;
  // returns false when the settings file is unreadable/invalid (out then
  // carries the red/yellow error screen state).
  bool display_snapshot(DisplaySnapshot *out);

  // 1 Hz bell tick (pitimer buzz_if_due): rings once per (date, start) game
  // when seconds_until_buzzer enters [-BUZZER_GRACE_SECONDS, 0]. Returns the
  // BELL_RELAY_* bitmask from settings buzzer.relays (RELAY3/4 ignored), 0
  // when nothing is due; on a non-zero return bell_duration_ms() is the
  // buzzer.duration pulse length from the same settings read.
  int bell_due();
  uint32_t bell_duration_ms() const { return this->bell_duration_ms_; }

 protected:
  std::string store_path_{"/flash"};
  time::RealTimeClock *time_{nullptr};
  std::unique_ptr<StoreFs> fs_;
  std::unique_ptr<SettingsStore> store_;
  bool ready_{false};
  int32_t wl_handle_{-1};  // wl_handle_t; FAT mount handle for unmount
  uint32_t last_error_log_ms_{0};  // throttle for repeated snapshot errors
  std::string last_buzzed_key_;    // pitimer last_buzzed: (date, start) game
  uint32_t bell_duration_ms_{0};   // set by bell_due() when it fires
};

}  // namespace curling_engine
}  // namespace esphome
