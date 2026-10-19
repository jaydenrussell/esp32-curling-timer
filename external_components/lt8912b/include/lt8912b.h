#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

namespace esphome {
namespace lt8912b {

// M5Stack Add-On Display Out for PoE-P4 (U221) — LT8912B MIPI-DSI to HDMI.
// Two supported modes on this add-on: 1280x720@60 and 1920x1080@30.
//
// Targets the Espressif esp_lcd LT8912B driver
// (espressif/esp_lcd_lt8912b) + esp-idf mipi_dsi, with the same panel
// timing M5GFX uses in M5UnitPoEP4HDMI.
class LT8912BDisplay : public display::Display, public PollingComponent {
 public:
  void set_dimensions(int width, int height) {
    width_ = width;
    height_ = height;
  }
  void set_refresh_rate(int refresh_rate) { refresh_rate_ = refresh_rate; }
  void set_reset_pin(GPIOPin *reset_pin) { reset_pin_ = reset_pin; }
  void set_i2c_bus(i2c::I2CBus *bus) { i2c_bus_ = bus; }

  void setup() override;
  void update() override;
  void dump_config() override;

  void fill(Color color) override;
  void draw_pixel_at(int x, int y, Color color) override;
  int get_width() override { return width_; }
  int get_height() override { return height_; }

 protected:
  int width_{1280};
  int height_{720};
  int refresh_rate_{60};
  GPIOPin *reset_pin_{nullptr};
  i2c::I2CBus *i2c_bus_{nullptr};
  uint8_t *framebuffer_{nullptr};

  // TODO(v1): allocate PSRAM framebuffer, init MIPI-DSI host, run the
  // LT8912B power-on sequence (reset, I2C regs, MIPI lane config), attach
  // esp_lcd panel draw callbacks to the framebuffer, flip on vsync.
};

}  // namespace lt8912b
}  // namespace esphome
