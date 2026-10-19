# lt8912b — M5Stack Add-On Display Out for PoE-P4 (U221)

ESPHome external component driving the **LT8912B MIPI-DSI → HDMI**
add-on on M5Stack Unit PoE-P4 (U213).

## Status

**WIP** — not yet enabled in `esphome/curling-timer.yaml`.
Work remaining: PSRAM framebuffer allocation, MIPI-DSI host bring-up,
LT8912B init sequence (power-on, I2C register config, 2-lane MIPI at
720p60 / 1080p30 timing as in M5GFX `M5UnitPoEP4HDMI`).

## References

- Pins: `docs/wiring-U213.md` (LCD_RST=GPIO4, LCD_BL=GPIO33, TP I2C G0/G1)
- Upstream driver: <https://components.espressif.com/components/espressif/esp_lcd_lt8912b>
- Add-on SKU: <https://docs.m5stack.com/en/products/sku/U221>
