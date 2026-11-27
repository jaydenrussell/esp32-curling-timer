#include "curling_engine.h"
#include "config_edit.h"

#include <utility>

#include "default_settings.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esp_sntp.h"

#if defined(ESP_PLATFORM)
#include <inttypes.h>
#include "driver/i2c_master.h"
#endif

namespace esphome {
namespace curling_engine {

static const char *const TAG = "curling_engine";

void CurlingEngineComponent::ntp_sync() {
  if (time_ != nullptr) {
    const char *srv = esp_sntp_getservername(0);
    if (srv != nullptr && srv[0] != '\0')
      ntp_server_ = srv;
    auto now = time_->now();
    if (now.is_valid()) {
      if (ntp_last_sync_ > 0) {
        ntp_drift_ms_ = (int32_t)((now.timestamp - ntp_last_sync_) * 1000LL);
      }
      ntp_last_sync_ = now.timestamp;
      ntp_ok_ = true;
      return;
    }
  }
  // SNTP failed or unavailable: fall back to RTC via direct I2C.
  int y, mo, d, h, mi, s;
  if (this->get_rtc_datetime(&y, &mo, &d, &h, &mi, &s)) {
    // Compute epoch from YMDHMS.
    int era = (y <= 0 ? y - 399 : y) / 400;
    int yoe = y - era * 400;
    int mp = (mo + 9) % 12;
    int doe = (153 * mp + 2) / 5 + d - 1;
    int days = era * 146097 + yoe * 365 + yoe / 4 - yoe / 100 + doe;
    int64_t epoch = static_cast<int64_t>(days) * 86400 +
                    static_cast<int64_t>(h) * 3600 +
                    static_cast<int64_t>(mi) * 60 +
                    static_cast<int64_t>(s);
    ntp_last_sync_ = epoch;
    ntp_ok_ = true;  // RTC is a valid time source
    ESP_LOGI(TAG, "ntp: using RTC fallback time %04d-%02d-%02d %02d:%02d:%02d",
             y, mo, d, h, mi, s);
    return;
  }
  ntp_ok_ = false;
}

std::string CurlingEngineComponent::ntp_status() const {
  if (!ntp_ok_) return "ERROR";
  return "OK (drift: " + std::to_string(ntp_drift_ms_) + "ms)";
}

static const char *const PARTITION_LABEL = "storage";
// FAT counts every open file and directory against this limit.
static constexpr size_t FAT_MAX_FILES = 16;

void CurlingEngineComponent::detect_addons() {
  this->addon_detected = PiTimerSettings::AddonDetection{};
  this->addon_detected.detected_at_ms = millis();

  // RTC: probe the PCF8563-compatible device via the RealTimeClock component.
  // now().is_valid() is true when the hardware has answered at least one
  // I2C read (pcf8563 performs an initial read in its own setup()).
  if (this->rtc_time_ != nullptr) {
    ESPTime now = this->rtc_time_->now();
    this->addon_detected.rtc = now.is_valid();
    ESP_LOGI(TAG, "addon rtc detected=%d", this->addon_detected.rtc ? 1 : 0);
  }
#if defined(ESP_PLATFORM)
  // PaHub v2.1 I2C hub detection: probe default address 0x70.
  // The PaHub (M5Stack PaHub Unit v2.1) is an I2C multiplexer (PCA9548A)
  // at address 0x70. We initialize a temporary I2C master bus, probe for
  // the hub itself, then scan channels 0-5 for connected devices.
  // Channel selection: write control byte (1 << channel) to 0x70.
  //
  // GPIO pin map: M5Stack Unit PoE-P4 (U213) Grove Port A =
  //   SDA = GPIO 21, SCL = GPIO 22.
  // See docs/wiring-U213.md for the full pin map.
  static constexpr uint8_t PAHUB_ADDR = 0x70;
  static constexpr uint8_t RTC_ADDR = 0x51;  // PCF8563 / HYM8563
  static constexpr int PAHUB_CHANNELS = 6;   // channels 0-5
  // Common I2C addresses to probe on each PaHub channel.
  static constexpr uint8_t kCommonAddrs[] = {RTC_ADDR, 0x25, 0x26, 0x38, 0x39};

  bool pahub_found = false;
  i2c_master_bus_config_t bus_config = {};
  bus_config.i2c_port = 0;
  bus_config.sda_io_num = GPIO_NUM_21;
  bus_config.scl_io_num = GPIO_NUM_22;
  bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  bus_config.glitch_ignore_cnt = 7;
  bus_config.flags.enable_internal_pullup = true;

  i2c_master_bus_handle_t bus_handle = nullptr;
  esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);
  if (err == ESP_OK) {
    // Create device handle for PaHub at 0x70.
    i2c_master_dev_handle_t pahub_dev = nullptr;
    err = i2c_master_bus_add_device(bus_handle,
        &(i2c_device_config_t{
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = PAHUB_ADDR,
            .scl_speed_hz = 100000,
        }),
        &pahub_dev);
    if (err == ESP_OK && pahub_dev != nullptr) {
      // Probe the PaHub hub address itself.
      err = i2c_master_probe(pahub_dev, PAHUB_ADDR, -1);
      if (err == ESP_OK) {
        pahub_found = true;
        // Scan each PaHub channel for connected devices.
        for (int ch = 0; ch < PAHUB_CHANNELS; ch++) {
          // Select channel: write control byte with bit channel set.
          uint8_t cmd = static_cast<uint8_t>(1 << ch);
          esp_err_t sel_err =
              i2c_master_transmit(pahub_dev, &cmd, 1, -1);
          if (sel_err != ESP_OK) {
            continue;  // channel select failed, skip this channel
          }
          // Small delay for the mux to settle.
          esp_rom_delay_us(100);
          // Probe common addresses on this channel.
          for (uint8_t addr : kCommonAddrs) {
            i2c_master_dev_handle_t dev = nullptr;
            esp_err_t dev_err = i2c_master_bus_add_device(bus_handle,
                &(i2c_device_config_t{
                    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
                    .device_address = addr,
                    .scl_speed_hz = 100000,
                }),
                &dev);
            if (dev_err == ESP_OK && dev != nullptr) {
              if (i2c_master_probe(dev, addr, -1) == ESP_OK) {
                this->addon_detected.pahub_channel_mask |= (1u << ch);
                this->addon_detected.pahub_channel_addr[ch] = addr;
                if (addr == RTC_ADDR) {
                  this->addon_detected.pahub_rtc_channel = ch;
                  this->addon_detected.rtc = true;
                }
                break;  // one device per channel is enough for reporting
              }
              i2c_master_bus_remove_device(dev);
            }
          }
        }
      }
      i2c_master_bus_remove_device(pahub_dev);
    }
    i2c_del_master_bus(bus_handle);
  }
  this->addon_detected.pahub = pahub_found;
  ESP_LOGI(TAG, "addon pahub detected=%d channel_mask=0x%04" PRIX32 " rtc_channel=%d",
           this->addon_detected.pahub ? 1 : 0,
           this->addon_detected.pahub_channel_mask,
           this->addon_detected.pahub_rtc_channel);
#else
  // Host test build: no I2C hardware available, PaHub cannot be auto-detected.
  this->addon_detected.pahub = false;
  this->addon_detected.pahub_channel_mask = 0;
  this->addon_detected.pahub_rtc_channel = -1;
  for (int i = 0; i < 6; i++) {
    this->addon_detected.pahub_channel_addr[i] = 0;
  }
  ESP_LOGI(TAG, "addon pahub detected=%d", this->addon_detected.pahub ? 1 : 0);
#endif
  // Wi-Fi failover (ESP32-C6 SDIO) detection is handled separately;
  // Pahub presence does not imply Wi-Fi is present.
}

// PCF8563 register map
static constexpr uint8_t PCF8563_ADDR = 0x51;
static constexpr uint8_t PCF8563_REG_CTRL1 = 0x00;
static constexpr uint8_t PCF8563_REG_CTRL2 = 0x01;  // bit7 = STOP oscillator
static constexpr uint8_t PCF8563_REG_SECONDS = 0x02;
static constexpr uint8_t PCF8563_REG_MINUTES = 0x03;
static constexpr uint8_t PCF8563_REG_HOURS = 0x04;
static constexpr uint8_t PCF8563_REG_DAY = 0x05;
static constexpr uint8_t PCF8563_REG_WEEKDAY = 0x06;
static constexpr uint8_t PCF8563_REG_MONTH = 0x07;
static constexpr uint8_t PCF8563_REG_YEAR = 0x08;

static uint8_t dec_to_bcd(uint8_t val) {
  return static_cast<uint8_t>((val / 10) * 16 + (val % 10));
}
static uint8_t bcd_to_dec(uint8_t val) {
  return static_cast<uint8_t>((val / 16) * 10 + (val % 16));
}

// Select a PaHub channel (write control byte 1<<ch to 0x70).
static esp_err_t select_pahub_channel(i2c_master_dev_handle_t pahub_dev, int ch) {
  uint8_t cmd = static_cast<uint8_t>(1 << ch);
  return i2c_master_transmit(pahub_dev, &cmd, 1, -1);
}

// Read a single register from PCF8563.
static esp_err_t pcf8563_read_reg(i2c_master_dev_handle_t rtc_dev, uint8_t reg, uint8_t *val) {
  return i2c_master_transmit_receive(rtc_dev, &reg, 1, val, 1, -1);
}

// Write a single register to PCF8563.
static esp_err_t pcf8563_write_reg(i2c_master_dev_handle_t rtc_dev, uint8_t reg, uint8_t val) {
  uint8_t buf[2] = {reg, val};
  return i2c_master_transmit(rtc_dev, buf, 2, -1);
}

bool CurlingEngineComponent::set_rtc_datetime(int year, int month, int day,
                                               int hour, int minute, int second) {
#if !defined(ESP_PLATFORM)
  return false;
#endif
  // Create I2C bus (same pins as detect_addons: Grove Port A GPIO21/22).
  i2c_master_bus_config_t bus_config = {};
  bus_config.i2c_port = 0;
  bus_config.sda_io_num = GPIO_NUM_21;
  bus_config.scl_io_num = GPIO_NUM_22;
  bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  bus_config.glitch_ignore_cnt = 7;
  bus_config.flags.enable_internal_pullup = true;

  i2c_master_bus_handle_t bus_handle = nullptr;
  esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "rtc set: i2c bus init failed: %s", esp_err_to_name(err));
    return false;
  }

  // Select PaHub channel if RTC is behind a PaHub.
  i2c_master_dev_handle_t pahub_dev = nullptr;
  int rtc_ch = this->addon_detected.pahub_rtc_channel;
  if (rtc_ch >= 0) {
    err = i2c_master_bus_add_device(bus_handle,
        &(i2c_device_config_t{
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = PAHUB_ADDR,
            .scl_speed_hz = 100000,
        }),
        &pahub_dev);
    if (err != ESP_OK || pahub_dev == nullptr) {
      ESP_LOGE(TAG, "rtc set: pahub device handle failed: %s", esp_err_to_name(err));
      i2c_del_master_bus(bus_handle);
      return false;
    }
    esp_err_t sel_err = select_pahub_channel(pahub_dev, rtc_ch);
    if (sel_err != ESP_OK) {
      ESP_LOGE(TAG, "rtc set: pahub channel select failed: %s", esp_err_to_name(sel_err));
      i2c_master_bus_remove_device(pahub_dev);
      i2c_del_master_bus(bus_handle);
      return false;
    }
    esp_rom_delay_us(100);
  }

  // Create device handle for PCF8563 at 0x51.
  i2c_master_dev_handle_t rtc_dev = nullptr;
  err = i2c_master_bus_add_device(bus_handle,
      &(i2c_device_config_t{
          .dev_addr_length = I2C_ADDR_BIT_LEN_7,
          .device_address = PCF8563_ADDR,
          .scl_speed_hz = 100000,
        }),
      &rtc_dev);
  if (err != ESP_OK || rtc_dev == nullptr) {
    ESP_LOGE(TAG, "rtc set: rtc device handle failed: %s", esp_err_to_name(err));
    if (pahub_dev) i2c_master_bus_remove_device(pahub_dev);
    i2c_del_master_bus(bus_handle);
    return false;
  }

  // Stop oscillator before writing.
  uint8_t ctrl2 = 0;
  if (pcf8563_read_reg(rtc_dev, PCF8563_REG_CTRL2, &ctrl2) != ESP_OK) {
    ESP_LOGE(TAG, "rtc set: read ctrl2 failed");
    i2c_del_master_bus(bus_handle);
    return false;
  }
  ctrl2 |= 0x80;  // STOP = 1
  if (pcf8563_write_reg(rtc_dev, PCF8563_REG_CTRL2, ctrl2) != ESP_OK) {
    ESP_LOGE(TAG, "rtc set: write ctrl2 (stop) failed");
    i2c_del_master_bus(bus_handle);
    return false;
  }

  // Write time registers.
  if (pcf8563_write_reg(rtc_dev, PCF8563_REG_SECONDS, dec_to_bcd(static_cast<uint8_t>(second))) != ESP_OK ||
      pcf8563_write_reg(rtc_dev, PCF8563_REG_MINUTES, dec_to_bcd(static_cast<uint8_t>(minute))) != ESP_OK ||
      pcf8563_write_reg(rtc_dev, PCF8563_REG_HOURS, dec_to_bcd(static_cast<uint8_t>(hour))) != ESP_OK ||
      pcf8563_write_reg(rtc_dev, PCF8563_REG_DAY, dec_to_bcd(static_cast<uint8_t>(day))) != ESP_OK ||
      pcf8563_write_reg(rtc_dev, PCF8563_REG_MONTH, dec_to_bcd(static_cast<uint8_t>(month))) != ESP_OK ||
      pcf8563_write_reg(rtc_dev, PCF8563_REG_YEAR, dec_to_bcd(static_cast<uint8_t>(year % 100))) != ESP_OK) {
    ESP_LOGE(TAG, "rtc set: write time registers failed");
    i2c_del_master_bus(bus_handle);
    return false;
  }

  // Start oscillator.
  ctrl2 &= 0x7F;  // STOP = 0
  if (pcf8563_write_reg(rtc_dev, PCF8563_REG_CTRL2, ctrl2) != ESP_OK) {
    ESP_LOGE(TAG, "rtc set: write ctrl2 (start) failed");
    i2c_del_master_bus(bus_handle);
    return false;
  }

  i2c_del_master_bus(bus_handle);
  ESP_LOGI(TAG, "rtc set: datetime set to %04d-%02d-%02d %02d:%02d:%02d",
           year, month, day, hour, minute, second);
  return true;
}

bool CurlingEngineComponent::get_rtc_datetime(int *year, int *month, int *day,
                                               int *hour, int *minute, int *second) {
#if !defined(ESP_PLATFORM)
  return false;
#endif
  i2c_master_bus_config_t bus_config = {};
  bus_config.i2c_port = 0;
  bus_config.sda_io_num = GPIO_NUM_21;
  bus_config.scl_io_num = GPIO_NUM_22;
  bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  bus_config.glitch_ignore_cnt = 7;
  bus_config.flags.enable_internal_pullup = true;

  i2c_master_bus_handle_t bus_handle = nullptr;
  esp_err_t err = i2c_new_master_bus(&bus_config, &bus_handle);
  if (err != ESP_OK) {
    return false;
  }

  int rtc_ch = this->addon_detected.pahub_rtc_channel;
  if (rtc_ch >= 0) {
    esp_err_t sel_err = select_pahub_channel(bus_handle, rtc_ch);
    if (sel_err != ESP_OK) {
      i2c_del_master_bus(bus_handle);
      return false;
    }
    esp_rom_delay_us(100);
  }

  uint8_t reg = PCF8563_REG_SECONDS;
  uint8_t vals[7];
  if (i2c_master_transmit_receive(rtc_dev, &reg, 1, vals, 7, -1) != ESP_OK) {
    i2c_del_master_bus(bus_handle);
    return false;
  }

  *second = bcd_to_dec(vals[0] & 0x7F);
  *minute = bcd_to_dec(vals[1] & 0x7F);
  *hour = bcd_to_dec(vals[2] & 0x3F);
  *day = bcd_to_dec(vals[3] & 0x3F);
  *month = bcd_to_dec(vals[5] & 0x1F);
  *year = 2000 + bcd_to_dec(vals[6]);

  i2c_del_master_bus(bus_handle);
  return true;
}

int64_t CurlingEngineComponent::get_current_epoch() {
#if defined(ESP_PLATFORM)
  // Try SNTP/NTP first.
  if (this->time_ != nullptr) {
    auto now = this->time_->now();
    if (now.is_valid()) {
      this->ntp_ok_ = true;
      this->ntp_last_sync_ = now.timestamp;
      return now.timestamp;
    }
  }
  // Fall back to RTC via direct I2C (handles PaHub channel selection).
  int y, mo, d, h, mi, s;
  if (this->get_rtc_datetime(&y, &mo, &d, &h, &mi, &s)) {
    // Compute epoch from YMDHMS.
    // Simple days-from-civil algorithm (Howard Hinnant).
    int era = (y <= 0 ? y - 399 : y) / 400;
    int yoe = y - era * 400;
    int mp = (mo + 9) % 12;
    int doe = (153 * mp + 2) / 5 + d - 1;
    int days = era * 146097 + yoe * 365 + yoe / 4 - yoe / 100 + doe;
    int64_t epoch = static_cast<int64_t>(days) * 86400 +
                    static_cast<int64_t>(h) * 3600 +
                    static_cast<int64_t>(mi) * 60 +
                    static_cast<int64_t>(s);
    // Mark NTP as not OK so callers know we're on RTC fallback.
    this->ntp_ok_ = false;
    return epoch;
  }
#endif
  return 0;
}

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
  this->register_log_callback();

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
       if (settings.parse_settings_text(text, SETTINGS_BASENAME, &err_msg)) {
         if (logger::global_logger != nullptr)
           logger::global_logger->set_log_level(settings.log_level);
       } else {
         ESP_LOGW(TAG, "Existing settings file does not validate: %s", err_msg.c_str());
      }
    }
    this->detect_addons();

    // Plug-and-play: auto-enable detected add-ons that the user hasn't
    // explicitly disabled. Auto-detect add-ons (rtc, pahub, wifi) gate on
    // detection at runtime; manual add-ons (relays, pushbutton, uart) are
    // user-controlled only and never auto-enabled here.
    std::string at_text, at_version, at_err;
    if (this->store_->read(&at_text, &at_version, &at_err) == StoreStatus::OK) {
      PiTimerSettings at_settings;
      if (at_settings.parse_settings_text(at_text, SETTINGS_BASENAME, &at_err)) {
        std::vector<ConfigValue> values;
        if (this->addon_detected.rtc && !at_settings.addons.rtc)
          values.push_back({"config.addons.rtc", yaml_bool(true)});
        if (this->addon_detected.pahub && !at_settings.addons.pahub)
          values.push_back({"config.addons.pahub", yaml_bool(true)});
        if (this->addon_detected.wifi && !at_settings.addons.wifi)
          values.push_back({"config.addons.wifi", yaml_bool(true)});
        if (!values.empty()) {
          std::string out, apply_err;
          if (apply_config_values(at_text, values, &out, &apply_err)) {
            std::string new_version;
            if (this->store_->save(out, &at_version, &new_version, &at_err) == StoreStatus::OK) {
              for (const auto &v : values)
                ESP_LOGI(TAG, "addon auto-enabled: %s", v.path.c_str());
            } else {
              ESP_LOGE(TAG, "addon auto-enable save failed: %s", at_err.c_str());
            }
          } else {
            ESP_LOGE(TAG, "addon auto-enable apply failed: %s", apply_err.c_str());
          }
        }
      }
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
  if (!now.is_valid()) {
    // SNTP has no valid time yet; fall back to the addon RTC if enabled.
    if (this->rtc_enabled_ && this->rtc_time_ != nullptr) {
      now = this->rtc_time_->now();
      if (!now.is_valid())
        return false;
    } else {
      return false;
    }
  }
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
  // Gate the RTC fallback on the runtime addon flag from settings AND
  // hardware detection (auto-detect add-on: effective = user_enabled && detected).
  this->rtc_enabled_ = settings.addons.rtc;  // manual enable works without detection; detection is informational

  bool clock_known = this->local_now(&weekday_mon0, &seconds_of_day, &date_iso);
  *out = render_snapshot(settings, clock_known, weekday_mon0, seconds_of_day, date_iso);
  if (this->display_test_mode_ != "normal") {
    out->header_text = "TEST " + this->display_test_mode_;
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
  // The relay add-on is gated at runtime: if it is off, no bell/relay
  // output ever happens even if the settings file lists relays.
  if (!settings.addons.relays)
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
      const char *ev_key = (ev == "Game Start") ? "start" : (ev == "Countdown Warning") ? "countdown" : "end";
      for (const auto &r : relays) {
        int i = 0;
        if (r == "RELAY1") i = 1;
        else if (r == "RELAY2") i = 2;
        else continue;
        int dur = relay_event_duration(settings, ev_key, i);
        if (dur <= 0) dur = settings.relay_duration;
        uint32_t ms = static_cast<uint32_t>(dur) * 1000U;
        if (i == 1) out->duration_ms_1 = ms;
        if (i == 2) out->duration_ms_2 = ms;
        if (ms > out->duration_ms) out->duration_ms = ms;
      }
      ESP_LOGI(TAG, "test relay: event %s, mask %d, r1=%ums r2=%ums", ev.c_str(), mask,
               static_cast<unsigned>(out->duration_ms_1), static_cast<unsigned>(out->duration_ms_2));
      return true;
    }
    return false;
  }
  // Manual push-button trigger: the GPIO button asked for the next scheduled
  // game with manual_trigger == true to be started. Find the first game today
  // whose start is in the future and which needs manual input, ring its start
  // relays once, and remember it so it won't ring again. Gated on the
  // pushbutton add-on flag — if off, GPIO presses are ignored.
  if (!settings.addons.pushbutton)
    return false;
  if (this->manual_trigger_pending()) {
    this->clear_manual_trigger();
    int weekday_mon0 = 0, seconds_of_day = 0;
    std::string date_iso;
    if (this->local_now(&weekday_mon0, &seconds_of_day, &date_iso)) {
      const std::vector<PiTimerGame> *list = nullptr;
      auto it = settings.schedules.find(DAY_NAMES[weekday_mon0]);
      if (it != settings.schedules.end())
        list = &it->second;
      if (list != nullptr) {
        for (const PiTimerGame &g : *list) {
          if (!g.manual_trigger) continue;
          if (g.start <= seconds_of_day) continue;
          const std::string key = date_iso + "/" + std::to_string(g.start) + "/manual";
          if (this->fired_keys_.count(key)) continue;
          this->fired_keys_.insert(key);
          int mask = 0;
          if (settings.ring_at_start.has_value()) {
            for (const auto &r : *settings.ring_at_start) {
              if (r == "RELAY1") mask |= BELL_RELAY_1;
              else if (r == "RELAY2") mask |= BELL_RELAY_2;
            }
          }
          if (mask != 0) {
            out->mask = mask;
            for (const auto &r : *settings.ring_at_start) {
              int i = 0;
              if (r == "RELAY1") i = 1;
              else if (r == "RELAY2") i = 2;
              else continue;
              int dur = relay_event_duration(settings, "start", i);
              if (dur <= 0) dur = settings.relay_start_duration;
              uint32_t ms = static_cast<uint32_t>(dur) * 1000U;
              if (i == 1) out->duration_ms_1 = ms;
              if (i == 2) out->duration_ms_2 = ms;
              if (ms > out->duration_ms) out->duration_ms = ms;
            }
            ESP_LOGI(TAG, "manual trigger: game %s start %d, mask %d r1=%ums r2=%ums",
                     g.message.value_or("None").c_str(), g.start, out->mask,
                     static_cast<unsigned>(out->duration_ms_1), static_cast<unsigned>(out->duration_ms_2));
            return true;
          }
        }
      }
    }
    return false;
  }
  // Qualified: the member bell_tick() would otherwise hide the free function.
  auto game = settings.find_current_game(weekday_mon0, seconds_of_day, date_iso);
  if (!::esphome::curling_engine::bell_tick(settings, game, date_iso, &this->fired_keys_, out))
    return false;
  ESP_LOGI(TAG, "bell: relays mask %d, %u ms", out->mask, static_cast<unsigned>(out->duration_ms));
  if (settings.addons.uart)
    this->uart_status();
  return true;
}

void CurlingEngineComponent::register_log_callback() {
#ifdef USE_LOG_LISTENERS
  if (logger::global_logger != nullptr)
    logger::global_logger->add_log_callback(this, &CurlingEngineComponent::log_callback);
#endif
}

void CurlingEngineComponent::log_callback(void *instance, uint8_t level, const char *tag, const char *message, size_t message_size) {
  auto *self = static_cast<CurlingEngineComponent *>(instance);
  std::string line;
  if (tag) line += std::string("[") + tag + "] ";
  if (message && message_size) line.append(message, message_size);
  std::lock_guard<std::mutex> lock(self->log_mutex_);
  self->log_buffer_.push_back(line);
  if (self->log_buffer_.size() > LOG_BUFFER_LINES)
    self->log_buffer_.pop_front();
}

std::string CurlingEngineComponent::get_logs() const {
  std::lock_guard<std::mutex> lock(log_mutex_);
  std::string out;
  for (const auto &l : log_buffer_) {
    out += l;
    out += "\n";
  }
  return out;
}

void CurlingEngineComponent::set_log_level(uint8_t level) {
  if (logger::global_logger != nullptr)
    logger::global_logger->set_log_level(level);
}

void CurlingEngineComponent::uart_status() {
  if (this->uart_ == nullptr)
    return;
  int weekday_mon0 = 0, seconds_of_day = 0;
  std::string date_iso;
  if (!this->local_now(&weekday_mon0, &seconds_of_day, &date_iso))
    return;
  PiTimerSettings settings;
  std::string err;
  if (!this->read_parsed(&settings, nullptr, nullptr, &err))
    return;
  auto game = settings.find_current_game(weekday_mon0, seconds_of_day, date_iso);
  std::string line = date_iso + " ";
  if (game.has_value()) {
    line += "start=" + std::to_string(game->start) + " buzzer=" + std::to_string(game->buzzer);
    if (game->seconds_until_buzzer.has_value())
      line += " left=" + std::to_string(*game->seconds_until_buzzer);
  } else {
    line += "idle";
  }
  line += "\n";
  this->uart_->write_array(reinterpret_cast<const uint8_t *>(line.data()), line.size());
}
}  // namespace curling_engine
}  // namespace esphome
