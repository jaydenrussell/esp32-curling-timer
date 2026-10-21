#pragma once

#ifdef USE_ESP32_VARIANT_ESP32P4
#include "esphome/core/component.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "esphome/core/gpio.h"

#include "esphome/components/display/display.h"

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_mipi_dsi.h>
#include <esp_lcd_io_i2c.h>
#include <esp_lcd_lt8912b.h>
#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace esphome {
namespace lt8912b {

static const char *const TAG = "lt8912b";

class Lt8912b : public display::Display {
 public:
  Lt8912b(size_t width, size_t height) : width_(width), height_(height) {}

  void setup() override;
  void update() override;
  void dump_config() override;

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }
  display::ColorOrder get_color_mode() { return display::COLOR_ORDER_RGB; }

  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = static_cast<InternalGPIOPin *>(pin); }
  void set_enable_pin(GPIOPin *pin) { this->enable_pin_ = static_cast<InternalGPIOPin *>(pin); }
  void set_sda_pin(GPIOPin *pin) { this->sda_pin_ = static_cast<InternalGPIOPin *>(pin); }
  void set_scl_pin(GPIOPin *pin) { this->scl_pin_ = static_cast<InternalGPIOPin *>(pin); }
  void set_i2c_frequency(uint32_t frequency) { this->i2c_frequency_ = frequency; }
  void set_lane_bit_rate(float lane_bit_rate) { this->lane_bit_rate_ = lane_bit_rate; }

  int get_width_internal() override { return this->width_; }
  int get_height_internal() override { return this->height_; }

  void draw_pixels_at(int x_start, int y_start, int w, int h, const uint8_t *ptr, display::ColorOrder order,
                      display::ColorBitness bitness, bool big_endian, int x_offset, int y_offset, int x_pad) override;
  void draw_pixel_at(int x, int y, Color color) override;
  void fill(Color color) override;
  int get_width() override;
  int get_height() override;

 protected:
  void write_to_display_(int x_start, int y_start, int w, int h, const uint8_t *ptr, int x_offset, int y_offset,
                         int x_pad);
  bool check_buffer_();
  void flush_pending_();
  void mark_failed_(const LogString *message, esp_err_t err);

  size_t width_;
  size_t height_;
  InternalGPIOPin *reset_pin_{nullptr};
  InternalGPIOPin *enable_pin_{nullptr};
  InternalGPIOPin *sda_pin_{nullptr};
  InternalGPIOPin *scl_pin_{nullptr};
  uint32_t i2c_frequency_{100000};
  float lane_bit_rate_{960};

  i2c_master_bus_handle_t i2c_bus_{};
  esp_lcd_panel_io_handle_t io_main_{};
  esp_lcd_panel_io_handle_t io_cec_{};
  esp_lcd_panel_io_handle_t io_avi_{};
  esp_lcd_dsi_bus_handle_t dsi_bus_{};
  esp_lcd_panel_handle_t panel_{};
  SemaphoreHandle_t io_lock_{};
  bool hdmi_ready_{false};

  display::ColorBitness color_depth_{display::COLOR_BITNESS_888};
  uint8_t *buffer_{nullptr};
  uint16_t x_low_{1};
  uint16_t y_low_{1};
  uint16_t x_high_{0};
  uint16_t y_high_{0};
};

}  // namespace lt8912b
}  // namespace esphome
#endif  // USE_ESP32_VARIANT_ESP32P4
