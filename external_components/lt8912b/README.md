# lt8912b - M5Stack Add-On Display Out for PoE-P4 (U221)

ESPHome external component driving the **LT8912B MIPI-DSI → HDMI**
add-on on M5Stack Unit PoE-P4 (U213): 1280x720@60 HDMI output.

## Status

Landed — display driver enabled in `esphome/curling-timer.yaml`
(test card until the LVGL UI lands).

## How it works

- Own `i2c_master` bus (default SDA=GPIO0, SCL=GPIO1, 100 kHz) drives the
  bridge at 0x48 (main) / 0x49 (cec_dsi) / 0x4A (avi) via
  `esp_lcd_new_panel_io_i2c`.
- MIPI-DSI host, 2 lanes @ 960 Mbps; the DSI PHY is powered by the
  `esp_ldo` component (channel 3, 2.5 V) — both are required in YAML.
- Bridge + DPI panel come from the official IDF component
  `espressif/esp_lcd_lt8912b` (added automatically at codegen), with the
  720p60 ready-made timing.
- Display buffer is 24-bit RGB888 in PSRAM (the bridge only accepts
  RGB888); LVGL's 16-bit output is converted on flush.
- Reset GPIO4 (active-low, driven by the driver), backlight GPIO33.

## Config

```yaml
esp_ldo:
  - channel: 3
    voltage: 2.5V

lt8912b:
  # all optional, defaults shown
  reset_pin: GPIO4
  enable_pin: GPIO33
  sda_pin: GPIO0
  scl_pin: GPIO1
  i2c_frequency: 100kHz
  lane_bit_rate: 960Mbps
```

Do not attach an `i2c:` bus to the same pins — this component owns them.

## References

- Pins: `docs/wiring-U213.md` (LCD_RST=GPIO4, LCD_BL=GPIO33, I2C G0/G1)
- Upstream driver: <https://components.espressif.com/components/espressif/esp_lcd_lt8912b>
- Reference bring-up: esp-bsp `esp32_p4_function_ev_board` (HDMI section)
- Add-on SKU: <https://docs.m5stack.com/en/products/sku/U221>
