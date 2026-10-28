#ifdef USE_ESP32_VARIANT_ESP32P4
#include <utility>
#include "lt8912b.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace lt8912b {

static bool notify_refresh_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx) {
  SemaphoreHandle_t sem = static_cast<SemaphoreHandle_t>(user_ctx);
  if (xPortInIsrContext()) {
    BaseType_t need_yield = pdFALSE;
    xSemaphoreGiveFromISR(sem, &need_yield);
    return (need_yield == pdTRUE);
  }
  xSemaphoreGive(sem);
  return pdFALSE;
}

static esp_lcd_panel_io_i2c_config_t make_io_config(uint32_t address, uint32_t frequency) {
  return {
      .dev_addr = address,
      .control_phase_bytes = 1,
      .dc_bit_offset = 0,
      .lcd_cmd_bits = 8,
      .lcd_param_bits = 8,
      .flags = {.disable_control_phase = 1},
      .scl_speed_hz = frequency,
  };
}

void Lt8912b::mark_failed_(const LogString *message, esp_err_t err) {
  ESP_LOGE(TAG, "%s: %s", LOG_STR_ARG(message), esp_err_to_name(err));
  this->mark_failed(message);
}

void Lt8912b::setup() {
  ESP_LOGCONFIG(TAG, "Running Setup");

  if (this->enable_pin_ != nullptr) {
    this->enable_pin_->setup();
    this->enable_pin_->digital_write(true);
    delay(10);
  }

  i2c_master_bus_config_t i2c_bus_config{};
  i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
  i2c_bus_config.i2c_port = -1;
  i2c_bus_config.sda_io_num = (gpio_num_t) this->sda_pin_->get_pin();
  i2c_bus_config.scl_io_num = (gpio_num_t) this->scl_pin_->get_pin();
  auto err = i2c_new_master_bus(&i2c_bus_config, &this->i2c_bus_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("i2c_new_master_bus failed"), err);
    return;
  }

  auto io_config = make_io_config(LT8912B_IO_I2C_MAIN_ADDRESS, this->i2c_frequency_);
  err = esp_lcd_new_panel_io_i2c(this->i2c_bus_, &io_config, &this->io_main_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("panel_io_i2c (main) failed"), err);
    return;
  }
  io_config = make_io_config(LT8912B_IO_I2C_CEC_ADDRESS, this->i2c_frequency_);
  err = esp_lcd_new_panel_io_i2c(this->i2c_bus_, &io_config, &this->io_cec_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("panel_io_i2c (cec_dsi) failed"), err);
    return;
  }
  io_config = make_io_config(LT8912B_IO_I2C_AVI_ADDRESS, this->i2c_frequency_);
  err = esp_lcd_new_panel_io_i2c(this->i2c_bus_, &io_config, &this->io_avi_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("panel_io_i2c (avi) failed"), err);
    return;
  }

  esp_lcd_dsi_bus_config_t dsi_bus_config{};
  dsi_bus_config.bus_id = 0;
  dsi_bus_config.num_data_lanes = 2;
  dsi_bus_config.lane_bit_rate_mbps = this->lane_bit_rate_;
  err = esp_lcd_new_dsi_bus(&dsi_bus_config, &this->dsi_bus_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("lcd_new_dsi_bus failed"), err);
    return;
  }

  esp_lcd_dpi_panel_config_t dpi_config = {
      .virtual_channel = 0,
      .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_PLL_F160M,
      .dpi_clock_freq_mhz = 80,
      // RGB565: the display and LVGL both work in 16-bit colour, so the
      // driver copies the pixels straight through (an RGB888 driver would
      // need a 2.8 MB framebuffer and convert every pixel).
      .in_color_format = LCD_COLOR_FMT_RGB888,
      .num_fbs = 2,
      .video_timing =
          {
              .h_size = 1280,
              .v_size = 720,
              .hsync_pulse_width = 32,
              .hsync_back_porch = 80,
              .hsync_front_porch = 408,
              .vsync_pulse_width = 5,
              .vsync_back_porch = 13,
              .vsync_front_porch = 3,
          },
      .flags =
          {
              .use_dma2d = false,
              .disable_lp = true,
          },
  };

  lt8912b_vendor_config_t vendor_config{};
  vendor_config.video_timing = {408, 32, 80, 1280, 1800, 3, 5, 13, 720, 741, 1, 0, 0,
                                LT8912B_ASPECT_RATION_16_9, 80};
  vendor_config.mipi_config.dsi_bus = this->dsi_bus_;
  vendor_config.mipi_config.dpi_config = &dpi_config;
  vendor_config.mipi_config.lane_num = 2;

  esp_lcd_panel_dev_config_t panel_config{};
  panel_config.reset_gpio_num = this->reset_pin_ != nullptr ? this->reset_pin_->get_pin() : -1;
  panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
  panel_config.bits_per_pixel = 24;
  panel_config.flags.reset_active_high = false;
  panel_config.vendor_config = &vendor_config;

  const esp_lcd_panel_lt8912b_io_t io_all = {
      .main = this->io_main_,
      .cec_dsi = this->io_cec_,
      .avi = this->io_avi_,
  };
  err = esp_lcd_new_panel_lt8912b(&io_all, &panel_config, &this->panel_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("esp_lcd_new_panel_lt8912b failed"), err);
    return;
  }

  err = esp_lcd_panel_reset(this->panel_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("esp_lcd_panel_reset failed"), err);
    return;
  }
  err = esp_lcd_panel_init(this->panel_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("esp_lcd_panel_init failed"), err);
    return;
  }

  ESP_LOGI(TAG, "free heap before the DSI lock: %u bytes (largest internal block %u)",
           static_cast<unsigned>(esp_get_free_heap_size()),
           static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)));
  // The lock is released by the transfer-done callback, so it is given
  // once here to let the first transfer start.
  this->io_lock_ = xSemaphoreCreateBinaryStatic(&this->io_lock_buffer_);
  // Static allocation never fails; give once so the first transfer can proceed
  xSemaphoreGive(this->io_lock_);
  esp_lcd_dpi_panel_event_callbacks_t cbs = {
      .on_color_trans_done = notify_refresh_ready,
  };
  err = esp_lcd_dpi_panel_register_event_callbacks(this->panel_, &cbs, this->io_lock_);
  if (err != ESP_OK) {
    this->mark_failed_(LOG_STR("dpi event callbacks failed"), err);
    return;
  }

  this->hdmi_ready_ = esp_lcd_panel_lt8912b_is_ready(this->panel_);
  ESP_LOGCONFIG(TAG, "LT8912B setup complete, HDMI %s", this->hdmi_ready_ ? "ready" : "not detected");
}

void Lt8912b::update() {
  if (this->is_failed())
    return;
  if (this->auto_clear_enabled_) {
    this->clear();
  }
  if (this->page_ != nullptr) {
    this->page_->get_writer()(*this);
  } else if (this->writer_.has_value()) {
    (*this->writer_)(*this);
  }

  if (this->panel_ != nullptr) {
    bool ready = esp_lcd_panel_lt8912b_is_ready(this->panel_);
    if (ready != this->hdmi_ready_) {
      this->hdmi_ready_ = ready;
      ESP_LOGI(TAG, "HDMI %s", ready ? "signal detected" : "signal lost");
    }
  }

  this->flush_pending_();
}

void Lt8912b::flush_pending_() {
  if (this->buffer_ == nullptr || this->x_low_ > this->x_high_ || this->y_low_ > this->y_high_)
    return;
  ESP_LOGV(TAG, "x_low %d, y_low %d, x_high %d, y_high %d", this->x_low_, this->y_low_, this->x_high_,
           this->y_high_);
  int w = this->x_high_ - this->x_low_ + 1;
  int h = this->y_high_ - this->y_low_ + 1;
  this->write_to_display_(this->x_low_, this->y_low_, w, h, this->buffer_, this->x_low_, this->y_low_,
                          this->width_ - w - this->x_low_);
  this->x_low_ = this->width_;
  this->y_low_ = this->height_;
  this->x_high_ = 0;
  this->y_high_ = 0;
}

void Lt8912b::draw_pixels_at(int x_start, int y_start, int w, int h, const uint8_t *ptr, display::ColorOrder order,
                             display::ColorBitness bitness, bool big_endian, int x_offset, int y_offset, int x_pad) {
  if (w <= 0 || h <= 0)
    return;
  if (bitness != this->color_depth_) {
    display::Display::draw_pixels_at(x_start, y_start, w, h, ptr, order, bitness, big_endian, x_offset, y_offset,
                                     x_pad);
    this->flush_pending_();
    return;
  }
  this->write_to_display_(x_start, y_start, w, h, ptr, x_offset, y_offset, x_pad);
}

void Lt8912b::write_to_display_(int x_start, int y_start, int w, int h, const uint8_t *ptr, int x_offset, int y_offset,
                                int x_pad) {
  esp_err_t err = ESP_OK;
  auto bytes_per_pixel = 3 - this->color_depth_;
  auto stride = (x_offset + w + x_pad) * bytes_per_pixel;
  ptr += y_offset * stride + x_offset * bytes_per_pixel;
  // Wait for the previous transfer to finish (the callback gives the lock
  // when it completes) before handing the driver new pixels.
  if (this->io_lock_ != nullptr)
    xSemaphoreTake(this->io_lock_, portMAX_DELAY);
  if (x_offset == 0 && x_pad == 0) {
    err = esp_lcd_panel_draw_bitmap(this->panel_, x_start, y_start, x_start + w, y_start + h, ptr);
  } else {
    for (int y = 0; y != h; y++) {
      err = esp_lcd_panel_draw_bitmap(this->panel_, x_start, y + y_start, x_start + w, y + y_start + 1, ptr);
      if (err != ESP_OK)
        break;
      ptr += stride;
    }
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "lcd_panel_draw_bitmap failed: %s", esp_err_to_name(err));
    if (this->io_lock_ != nullptr)
      xSemaphoreGive(this->io_lock_);
  }
}

bool Lt8912b::check_buffer_() {
  if (this->is_failed())
    return false;
  if (this->buffer_ != nullptr)
    return true;
  auto bytes_per_pixel = 3 - this->color_depth_;
  // A full screen is 1.8 MB at 1280x720 - far too much for the internal
  // heap, so ask for external (PSRAM) memory.
  RAMAllocator<uint8_t> allocator(true);
  this->buffer_ = allocator.allocate(this->height_ * this->width_ * bytes_per_pixel);
  if (this->buffer_ == nullptr) {
    this->mark_failed_(LOG_STR("Could not allocate buffer for display!"), ESP_ERR_NO_MEM);
    return false;
  }
  return true;
}

void Lt8912b::draw_pixel_at(int x, int y, Color color) {
  if (!this->get_clipping().inside(x, y))
    return;

  switch (this->rotation_) {
    case display::DISPLAY_ROTATION_0_DEGREES:
      break;
    case display::DISPLAY_ROTATION_90_DEGREES:
      std::swap(x, y);
      x = this->width_ - x - 1;
      break;
    case display::DISPLAY_ROTATION_180_DEGREES:
      x = this->width_ - x - 1;
      y = this->height_ - y - 1;
      break;
    case display::DISPLAY_ROTATION_270_DEGREES:
      std::swap(x, y);
      y = this->height_ - y - 1;
      break;
  }
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0) {
    return;
  }
  if (!this->check_buffer_())
    return;
  size_t pos = (y * this->width_) + x;
  this->buffer_[pos * 3] = color.r;
  this->buffer_[pos * 3 + 1] = color.g;
  this->buffer_[pos * 3 + 2] = color.b;
  if (x < this->x_low_)
    this->x_low_ = x;
  if (y < this->y_low_)
    this->y_low_ = y;
  if (x > this->x_high_)
    this->x_high_ = x;
  if (y > this->y_high_)
    this->y_high_ = y;
}

void Lt8912b::fill(Color color) {
  if (!this->check_buffer_())
    return;

  if (this->get_clipping().is_set()) {
    Display::fill(color);
    return;
  }

  for (size_t i = 0; i != this->width_ * this->height_; i++) {
    this->buffer_[i * 3 + 0] = color.r;
    this->buffer_[i * 3 + 1] = color.g;
    this->buffer_[i * 3 + 2] = color.b;
  }
  this->x_low_ = 0;
  this->y_low_ = 0;
  this->x_high_ = this->width_ - 1;
  this->y_high_ = this->height_ - 1;
}

int Lt8912b::get_width() {
  switch (this->rotation_) {
    case display::DISPLAY_ROTATION_90_DEGREES:
    case display::DISPLAY_ROTATION_270_DEGREES:
      return this->get_height_internal();
    case display::DISPLAY_ROTATION_0_DEGREES:
    case display::DISPLAY_ROTATION_180_DEGREES:
    default:
      return this->get_width_internal();
  }
}

int Lt8912b::get_height() {
  switch (this->rotation_) {
    case display::DISPLAY_ROTATION_0_DEGREES:
    case display::DISPLAY_ROTATION_180_DEGREES:
      return this->get_width_internal();
    case display::DISPLAY_ROTATION_90_DEGREES:
    case display::DISPLAY_ROTATION_270_DEGREES:
    default:
      return this->get_height_internal();
  }
}

void Lt8912b::dump_config() {
  ESP_LOGCONFIG(TAG,
                "LT8912B MIPI-DSI to HDMI"
                "\n  Width: %u"
                "\n  Height: %u"
                "\n  Rotation: %d degrees"
                "\n  DSI Lanes: 2"
                "\n  Lane Bit Rate: %.0fMbps"
                "\n  I2C Frequency: %uHz"
                "\n  Buffer Color Depth: 24 bit",
                this->width_, this->height_, this->rotation_, this->lane_bit_rate_, this->i2c_frequency_);
  LOG_PIN("  Reset Pin ", this->reset_pin_);
  LOG_PIN("  Enable Pin ", this->enable_pin_);
  LOG_PIN("  SDA Pin ", this->sda_pin_);
  LOG_PIN("  SCL Pin ", this->scl_pin_);
}

}  // namespace lt8912b
}  // namespace esphome
#endif  // USE_ESP32_VARIANT_ESP32P4
