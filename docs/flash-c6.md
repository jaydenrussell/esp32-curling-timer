# Flashing the ESP32-C6 SuperMini co-processor (one-time)

The C6 must run Espressif's ESP-Hosted **network_adapter** slave firmware
before the P4 can drive it over SDIO. Flash it once from your PC over the
SuperMini's USB-C; after that only the P4 talks to it.

## 1. Version-matched binary — already in this repo

`firmware/network_adapter_esp32c6.bin` is Espressif's prebuilt slave
firmware from the `esphome/esp-hosted-firmware` release **v2.12.13**
(built with ESP-IDF v5.5.5), which is exactly what ESPHome 2026.9.1
builds the P4 host with. SHA256:

```
09e4ed4ff648444ad045c1d934825af407648f58898586ce9f4275e53fdb23f6
```

If ESPHome is ever upgraded, re-download the matching release tag from
<https://github.com/esphome/esp-hosted-firmware/releases> (tag should
equal the `espressif/esp_hosted` version ESPHome pins).

## 2. Flash (Windows)

SuperMini on USB-C **alone, not wired to the U213**:

```powershell
pip install esptool
esptool.py --chip esp32c6 --port COMx --baud 921600 write_flash 0x0 firmware\network_adapter_esp32c6.bin
```

- `COMx` = the SuperMini's port in Device Manager
- If flashing fails, hold `BOOT` while pressing `EN`/reset, release,
  retry (enters ROM download mode)
- Success check: board re-enumerates and boots without errors

## 3. Wire and verify

1. Wire the SuperMini to the U213 header per `docs/wiring-U213.md`
   (six SDIO wires + 3V3/GND, optional EN→GPIO27).
2. Enable the Wi-Fi package in `esphome/curling-timer.yaml`:
   `packages: { wifi_addon: !include wifi-c6-failover.yaml }`, build
   and flash the P4.
3. On boot, logs should show the C6 joining your Wi-Fi and SDIO transport
   coming up. Ethernet and Wi-Fi both respond; Ethernet stays primary.

## Troubleshooting

- **SDIO no clock / no data** — check wires <5cm, common GND, and the
  `sdio_frequency: 20MHz` step-down (internal pull-ups only on 1-bit)
- **Transport up, Wi-Fi dead** — version mismatch between
  `network_adapter.bin` and ESPHome; re-download the matching release
- **C6 stuck** — wire EN (SDIO-Bus pin 7 / GPIO27) and the P4 will
  hard-reset it (`reset_pin: GPIO27` is already in the package config)
