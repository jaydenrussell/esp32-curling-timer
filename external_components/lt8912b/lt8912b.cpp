#ifdef USE_ESP32_VARIANT_ESP32P4
#include <utility>
#include "lt8912b.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "esphome/core/helpers.h"
#include <cstring>

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
      // Match the proven M5GFX PoE-P4 HDMI config: RGB888 framebuffer in and
      // out. The P4 DSI host only emits RGB888 on the wire; RGB565-in relied on
      // the bridge doing 565->888 conversion, which left the LT8912B unlocked
      // (blue screen) on rev1.3 silicon.
      .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
      .dpi_clock_freq_mhz = 80,
      .in_color_format = LCD_COLOR_FMT_RGB888,
      .out_color_format = LCD_COLOR_FMT_RGB888,
      .num_fbs = 2,
      .video_timing =
          {
              .h_size = 1280,
              .v_size = 720,
              .hsync_pulse_width = 40,
              .hsync_back_porch = 220,
              .hsync_front_porch = 110,
              .vsync_pulse_width = 5,
              .vsync_back_porch = 20,
              .vsync_front_porch = 5,
          },
      .flags =
          {
              .disable_lp = true,
          },
  };

  lt8912b_vendor_config_t vendor_config{};
  // 1280x720@60 (CVT-RB, VIC=4) as used by the validated M5GFX PoE-P4 driver.
  vendor_config.video_timing = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750, 1, 1, 4,
                                 LT8912B_ASPECT_RATION_16_9, 80};
  vendor_config.mipi_config.dsi_bus = this->dsi_bus_;
  vendor_config.mipi_config.dpi_config = &dpi_config;
  vendor_config.mipi_config.lane_num = 2;

  esp_lcd_panel_dev_config_t panel_config{};
  panel_config.reset_gpio_num = this->reset_pin_ != nullptr ? this->reset_pin_->get_pin() : -1;
  panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
  panel_config.bits_per_pixel = 24;  // DSI wire format is RGB888
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
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(6, 0, 0)
  // IDF >= 6.0 requires DMA2D to be explicitly enabled on the DPI panel for
  // the hardware-accelerated framebuffer copy path (the use_dma2d flag was
  // removed from the config struct in 6.0).
  esp_err_t dma2d_err = esp_lcd_dpi_panel_enable_dma2d(this->panel_);
  ESP_LOGI(TAG, "esp_lcd_dpi_panel_enable_dma2d: %s", esp_err_to_name(dma2d_err));
#endif

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
  this->log_hdmi_diagnostics();
}

void Lt8912b::log_hdmi_diagnostics() {
  if (this->io_main_ == nullptr || this->panel_ == nullptr) {
    ESP_LOGW(TAG, "HDMI diag: panel not initialized");
    return;
  }
  // LT8912B status registers (see espressif/esp_lcd_lt8912b):
  //   0xC1 bit7 = HDMI HPD (sink hot-plug)
  //   0x9C..0x9F = detected MIPI hsync/vsync widths (DSI lock indicator)
  uint8_t hpd = 0, hsync_l = 0, hsync_h = 0, vsync_l = 0, vsync_h = 0;
  esp_err_t e_hpd = esp_lcd_panel_io_rx_param(this->io_main_, 0xC1, &hpd, 1);
  esp_err_t e_hl = esp_lcd_panel_io_rx_param(this->io_main_, 0x9C, &hsync_l, 1);
  esp_err_t e_hh = esp_lcd_panel_io_rx_param(this->io_main_, 0x9D, &hsync_h, 1);
  esp_err_t e_vl = esp_lcd_panel_io_rx_param(this->io_main_, 0x9E, &vsync_l, 1);
  esp_err_t e_vh = esp_lcd_panel_io_rx_param(this->io_main_, 0x9F, &vsync_h, 1);
  bool sink = (e_hpd == ESP_OK) && (hpd & 0x80);
  ESP_LOGI(TAG,
           "HDMI diag: HPD[0xC1]=0x%02X sink=%s (%s) | MIPI hsync=0x%02X%02X vsync=0x%02X%02X "
           "(rx %s/%s/%s/%s) | ready=%s buf=%p",
           hpd, sink ? "CONNECTED" : "none", esp_err_to_name(e_hpd), hsync_h, hsync_l, vsync_h, vsync_l,
           esp_err_to_name(e_hl), esp_err_to_name(e_hh), esp_err_to_name(e_vl), esp_err_to_name(e_vh),
           this->hdmi_ready_ ? "yes" : "no", (void *) this->buffer_);
}

void Lt8912b::update() {
  if (this->is_failed() || this->panel_ == nullptr)
    return;
  if (this->auto_clear_enabled_) {
    this->clear();
  }
  if (this->show_test_card_) {
    this->test_card();
  } else if (this->page_ != nullptr) {
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
  if (this->is_failed() || this->panel_ == nullptr || this->buffer_ == nullptr || this->x_low_ > this->x_high_ || this->y_low_ > this->y_high_) {
    return;
  }
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
  if (this->is_failed() || this->panel_ == nullptr || this->buffer_ == nullptr)
    return;
  if (ptr == nullptr)
    return;

  // Handle RGB565 (LVGL) -> RGB888 (LT8912B) conversion directly
  if (bitness == display::COLOR_BITNESS_565 && this->color_depth_ == display::COLOR_BITNESS_888) {
    this->convert_565_to_888_and_write(x_start, y_start, w, h, ptr, x_offset, y_offset, x_pad, order, big_endian);
    return;
  }

  // Same format: write directly
  if (bitness == this->color_depth_) {
    this->write_to_display_(x_start, y_start, w, h, ptr, x_offset, y_offset, x_pad);
    return;
  }

  // Fallback to base class
  display::Display::draw_pixels_at(x_start, y_start, w, h, ptr, order, bitness, big_endian, x_offset, y_offset, x_pad);
  this->flush_pending_();
}

void Lt8912b::convert_565_to_888_and_write(int x_start, int y_start, int w, int h, const uint8_t *src_ptr, int x_offset, int y_offset, int x_pad, display::ColorOrder order, bool big_endian) {
  if (!this->check_buffer_())
    return;

  const int bytes_per_pixel_src = 2;
  const int bytes_per_pixel_dst = 3;
  const int stride_src = (x_offset + w + x_pad) * bytes_per_pixel_src;

  const uint8_t *src_row = src_ptr + y_offset * stride_src + x_offset * bytes_per_pixel_src;

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      uint16_t src_val;
      if (big_endian) {
        src_val = (src_row[0] << 8) | src_row[1];
      } else {
        src_val = src_row[0] | (src_row[1] << 8);
      }
      src_row += bytes_per_pixel_src;

      // LVGL RGB565 is always standard order (R in high bits); decode
      // accordingly regardless of the reported ColorOrder to avoid R/B swap.
      uint8_t r = (uint8_t)(((src_val >> 11) & 0x1F) << 3);
      uint8_t g = (uint8_t)(((src_val >> 5) & 0x3F) << 2);
      uint8_t b = (uint8_t)((src_val & 0x1F) << 3);

      int dst_x = x_start + x;
      int dst_y = y_start + y;
      size_t dst_pos = (dst_y * this->width_ + dst_x) * bytes_per_pixel_dst;
      this->buffer_[dst_pos] = r;
      this->buffer_[dst_pos + 1] = g;
      this->buffer_[dst_pos + 2] = b;
    }
    src_row += x_pad * bytes_per_pixel_src;
  }

  if (x_start < this->x_low_) this->x_low_ = x_start;
  if (y_start < this->y_low_) this->y_low_ = y_start;
  if (x_start + w - 1 > this->x_high_) this->x_high_ = x_start + w - 1;
  if (y_start + h - 1 > this->y_high_) this->y_high_ = y_start + h - 1;

  this->flush_pending_();
}

void Lt8912b::write_to_display_(int x_start, int y_start, int w, int h, const uint8_t *ptr, int x_offset, int y_offset,
                                 int x_pad) {
  if (this->is_failed() || this->panel_ == nullptr || this->buffer_ == nullptr || ptr == nullptr)
    return;
  esp_err_t err = ESP_OK;
  auto bytes_per_pixel = 3;  // RGB888 framebuffer
  auto stride = (x_offset + w + x_pad) * bytes_per_pixel;
  ptr += y_offset * stride + x_offset * bytes_per_pixel;
  // Wait for the previous transfer to finish (the callback gives the lock
  // when it completes) before handing the driver new pixels.
  if (this->io_lock_ != nullptr) {
    if (xSemaphoreTake(this->io_lock_, pdMS_TO_TICKS(1000)) != pdTRUE) {
      ESP_LOGE(TAG, "io_lock_ timeout");
      return;
    }
  }
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
  } else if (!this->first_frame_logged_) {
    this->first_frame_logged_ = true;
    ESP_LOGI(TAG, "first framebuffer transferred to DSI (%dx%d) — HDMI stream active", w, h);
  }
}

bool Lt8912b::check_buffer_() {
  if (this->is_failed())
    return false;
  if (this->buffer_ != nullptr)
    return true;
  auto bytes_per_pixel = 3;  // RGB888
  // A full screen is 2.8 MB at 1280x720 - far too much for the internal
  // heap, so ask for external (PSRAM) memory.
  RAMAllocator<uint8_t> allocator(true);
  this->buffer_ = allocator.allocate(this->height_ * this->width_ * bytes_per_pixel);
  if (this->buffer_ != nullptr)
    memset(this->buffer_, 0, this->height_ * this->width_ * bytes_per_pixel);
  if (this->buffer_ == nullptr) {
    this->mark_failed_(LOG_STR("Could not allocate buffer for display!"), ESP_ERR_NO_MEM);
    return false;
  }
  return true;
}

void Lt8912b::draw_pixel_at(int x, int y, Color color) {
  if (this->is_failed() || this->panel_ == nullptr || this->buffer_ == nullptr)
    return;
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
  ESP_LOGV(TAG, 'draw_pixel_at: (%d,%d)', x, y);
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
  if (this->is_failed() || this->panel_ == nullptr)
    return;
  if (!this->check_buffer_())
    return;

  if (this->get_clipping().is_set()) {
    Display::fill(color);
    return;
  }

  uint32_t total = this->width_ * this->height_;
  for (uint32_t i = 0; i != total; i++) {
    this->buffer_[i * 3] = color.r;
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
      return this->get_height_internal();
    case display::DISPLAY_ROTATION_90_DEGREES:
    case display::DISPLAY_ROTATION_270_DEGREES:
    default:
      return this->get_width_internal();
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
                "\n  I2C Frequency: %luHz"
                "\n  Framebuffer: 24 bit RGB888 -> DSI out: 24 bit RGB888",
                this->width_, this->height_, this->rotation_, this->lane_bit_rate_, this->i2c_frequency_);
  LOG_PIN("  Reset Pin ", this->reset_pin_);
  LOG_PIN("  Enable Pin ", this->enable_pin_);
  LOG_PIN("  SDA Pin ", this->sda_pin_);
  LOG_PIN("  SCL Pin ", this->scl_pin_);
}

}  // namespace lt8912b
}  // namespace esphome
#endif  // USE_ESP32_VARIANT_ESP32P4