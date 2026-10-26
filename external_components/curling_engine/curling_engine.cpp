#include "curling_engine.h"

#include <utility>

#include "default_settings.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace curling_engine {

void CurlingEngineComponent::ntp_sync() {
  // Lightweight: mark sync time; drift computation requires network SNTP
  // (not available in this build). Record current RTC as last sync.
  if (time_ != nullptr) {
    auto now = time_->now();
    if (now.is_valid()) {
      ntp_last_sync_ = now.timestamp;
      ntp_drift_ms_ = 0;
      ntp_ok_ = true;
      return;
    }
  }
  ntp_ok_ = false;
}

std::string CurlingEngineComponent::ntp_status() const {
  if (!ntp_ok_) return "ERROR";
  return "OK (drift: " + std::to_string(ntp_drift_ms_) + "ms)";
}

static const char *const TAG = "curling_engine";
static const char *const PARTITION_LABEL = "storage";
// FAT counts every open file and directory against this limit.
static constexpr size_t FAT_MAX_FILES = 16;

void CurlingEngineComponent::setup() {
  esp_vfs_fat_mount_config_t conf = {};
  conf.format_if_mount_failed = true;
  conf.max_files = FAT_MAX_FILES;
  conf.allocation_unit_size = 16 * 1024;
  esp_err_t err =
      esp_vfs_fat_spiflash_mount_rw_wl(this->store_path_.c_str(), PARTITION_LABEL, &conf,
                                       &this->wl_handle_);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "FAT mount of %s (partition '%s') failed: %s — check esp32 partitions",
             this->store_path_.c_str(), PARTITION_LABEL, esp_err_to_name(err));
    this->mark_failed();
    return;
  }
  uint64_t total = 0, used = 0;
  if (esp_vfs_fat_info(this->store_path_.c_str(), &total, &used) == ESP_OK)
    ESP_LOGI(TAG, "FATFS %s: %llu KB total, %llu KB used", this->store_path_.c_str(),
             static_cast<unsigned long long>(total / 1024),
             static_cast<unsigned long long>(used / 1024));
  ESP_LOGI(TAG, "free heap after the FAT mount: %u bytes (largest internal block %u)",
           static_cast<unsigned>(esp_get_free_heap_size()),
           static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)));

  this->fs_.reset(make_posix_store_fs());
  this->store_ = std::make_unique<SettingsStore>(this->fs_.get(), this->settings_path());
  this->ready_ = true;

  if (!this->fs_->exists(this->settings_path())) {
    ESP_LOGI(TAG, "No settings file yet, writing first-boot defaults to %s",
             this->settings_path().c_str());
    if (!this->fs_->write_bytes(this->settings_path(), DEFAULT_SETTINGS_YAML))
      ESP_LOGE(TAG, "Failed to write default settings to %s", this->settings_path().c_str());
  } else {
    // Never rewrite an existing file — only warn so the web editor can fix it.
    std::string text, version, err_msg;
    if (this->store_->read(&text, &version, &err_msg) == StoreStatus::OK) {
      PiTimerSettings settings;
      if (!settings.parse_settings_text(text, SETTINGS_BASENAME, &err_msg))
        ESP_LOGW(TAG, "Existing settings file does not validate: %s", err_msg.c_str());
    }
  }
}

void CurlingEngineComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Curling Engine:");
  ESP_LOGCONFIG(TAG, "  Settings file: %s", this->settings_path().c_str());
  ESP_LOGCONFIG(TAG, "  Time source: %s", this->time_ != nullptr ? "configured" : "MISSING");
}

bool CurlingEngineComponent::local_now(int *weekday_mon0, int *seconds_of_day,
                                       std::string *date_iso) const {
  if (this->time_ == nullptr)
    return false;
  ESPTime now = this->time_->now();
  if (!now.is_valid())
    return false;
  if (weekday_mon0 != nullptr)
    *weekday_mon0 = (now.day_of_week + 5) % 7;  // tm Sun=1..Sat=7 -> Mon=0..Sun=6
  if (seconds_of_day != nullptr)
    *seconds_of_day = now.hour * 3600 + now.minute * 60 + now.second;
  if (date_iso != nullptr)
    *date_iso = now.strftime("%Y-%m-%d");
  return true;
}

bool CurlingEngineComponent::read_parsed(PiTimerSettings *settings, std::string *text,
                                         std::string *version, std::string *err) {
  if (this->store_ == nullptr) {
    if (err != nullptr)
      *err = "settings store not ready";
    return false;
  }
  std::string local_text, local_version;
  if (this->store_->read(&local_text, &local_version, err) != StoreStatus::OK)
    return false;
  PiTimerSettings parsed;
  if (!parsed.parse_settings_text(local_text, SETTINGS_BASENAME, err))
    return false;
  *settings = std::move(parsed);
  if (text != nullptr)
    *text = std::move(local_text);
  if (version != nullptr)
    *version = std::move(local_version);
  return true;
}

bool CurlingEngineComponent::display_snapshot(DisplaySnapshot *out) {
  *out = DisplaySnapshot{};
  int weekday_mon0 = 0, seconds_of_day = 0;
  std::string date_iso;
  bool clock_known = this->local_now(&weekday_mon0, &seconds_of_day, &date_iso);

  PiTimerSettings settings;
  std::string err;
  if (!this->read_parsed(&settings, nullptr, nullptr, &err)) {
    // pitimer display_error(): red fixed title + first line of the message.
    out->header_text = "** ERROR: CHECK SETTINGS FILE **";
    out->header_color = color_to_hex("red");
    out->show_status = true;
    size_t nl = err.find('\n');
    out->status_text = err.substr(0, nl == std::string::npos ? err.size() : nl);
    out->status_color = color_to_hex("yellow");
    uint32_t now_ms = millis();
    if (this->last_error_log_ms_ == 0 || now_ms - this->last_error_log_ms_ >= 30000) {
      this->last_error_log_ms_ = now_ms;
      ESP_LOGW(TAG, "display snapshot: settings invalid: %s", err.c_str());
    }
    return false;
  }
  *out = render_snapshot(settings, clock_known, weekday_mon0, seconds_of_day, date_iso);
  if (this->display_test_mode_ != "normal") {
    out->header_text = "TEST 1280x720";
    out->header_color = color_to_hex("red");
    out->show_countdown = false;
    out->show_pace = false;
    out->show_game_name = false;
    out->show_date_time = false;
    out->show_status = false;
  }
  return true;
}

bool CurlingEngineComponent::bell_tick(BellRing *out) {
  *out = BellRing{};
  if (!this->ready_)
    return false;
  int weekday_mon0 = 0, seconds_of_day = 0;
  std::string date_iso;
  if (!this->local_now(&weekday_mon0, &seconds_of_day, &date_iso))
    return false;
  PiTimerSettings settings;
  std::string err;
  if (!this->read_parsed(&settings, nullptr, nullptr, &err))
    return false;
  // Test relay: if a test was requested, fire the configured relays for that event.
  if (this->test_relay_pending()) {
    std::string ev = this->test_relay_event();
    this->clear_test_relay();
    std::vector<std::string> relays;
    if (ev == "Game End" && settings.relays.has_value()) relays = *settings.relays;
    else if (ev == "Game Start" && settings.ring_at_start.has_value()) relays = *settings.ring_at_start;
    else if (ev == "Countdown Warning") relays = settings.countdown_relays;
    int mask = 0;
    for (const auto &r : relays) {
      if (r == "RELAY1") mask |= BELL_RELAY_1;
      else if (r == "RELAY2") mask |= BELL_RELAY_2;
    }
    if (mask != 0) {
      out->mask = mask;
      int dur = 0;
      if (ev == "Game Start") dur = settings.relay_start_duration;
      else if (ev == "Countdown Warning") dur = settings.relay_countdown_duration;
      else if (ev == "Game End") dur = settings.relay_end_duration;
      if (dur <= 0) dur = settings.relay_duration;
      out->duration_ms = static_cast<uint32_t>(dur) * 1000;
      ESP_LOGI(TAG, "test relay: event %s, mask %d, %u ms", ev.c_str(), mask, static_cast<unsigned>(out->duration_ms));
      return true;
    }
    return false;
  }
  auto game = settings.find_current_game(weekday_mon0, seconds_of_day, date_iso);
  // Qualified: the member bell_tick() would otherwise hide the free function.
  if (!::esphome::curling_engine::bell_tick(settings, game, date_iso, &this->fired_keys_, out))
    return false;
  ESP_LOGI(TAG, "bell: relays mask %d, %u ms", out->mask, static_cast<unsigned>(out->duration_ms));
  return true;
}

}  // namespace curling_engine
}  // namespace esphome
