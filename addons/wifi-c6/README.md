# ESP32-C6 Wi-Fi Addon

This addon uses an ESP32-C6 as an ESP-Hosted Wi-Fi coprocessor for the ESP32-P4 timer.

## Important

The C6 does not run ESPHome firmware. It must be flashed with ESP-Hosted slave firmware built from ESP-IDF. ESPHome Device Builder can flash ESPHome firmware, but not the ESP-Hosted slave image.

## Flashing the C6

1. Build the ESP-Hosted slave example for ESP32-C6 in ESP-IDF.
2. Flash it with `esptool.py` or the ESP-IDF flash tool.
3. Connect the C6 to the P4 Hat2 SDIO pins.

## P4 Host Configuration

See `host-snippet.yaml` for the optional P4-side configuration. The P4 auto-detects the module through SDIO and stays offline when it is absent.
