#include "curling_engine.h"

#include <utility>

#include "default_settings.h"
#include "esp_err.h"
#include "esp_vfs_fat.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace curling_engine {

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
  return true;
}

}  // namespace curling_engine
}  // namespace esphome
