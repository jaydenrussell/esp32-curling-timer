# Flashing the ESP32-C6 SuperMini co-processor (one-time)

The C6 must run Espressif's ESP-Hosted **network_adapter** slave firmware
before the P4 can drive it over SDIO. Flash it once from your PC over the
SuperMini's USB-C; after that only the P4 talks to it.

## 1. Match the version

The `network_adapter.bin` version must match the `esp_hosted` IDF version
that your ESPHome release builds with. ESPHome publishes the exact file —
get it from the **ESPHosted** component docs page:
<https://esphome.io/components/esp32_hosted/> ("Flashing the network
adapter" section, contains the per-release download link).

## 2. Flash (Windows)

```powershell
pip install esptool
esptool.py --chip esp32c6 --port COMx --baud 921600 \
  write_flash 0x0 network_adapter.bin
```

- `COMx` = the SuperMini's port in Device Manager
- If flashing fails, hold `BOOT` while pressing `EN`/reset, release,
  retry (enters ROM download mode)

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
  10k pull-ups on CMD/D0/D3.
- **Transport up, Wi-Fi dead** — version mismatch between
  `network_adapter.bin` and ESPHome; re-download the exact release.
- **C6 stuck** — wire EN to GPIO27, enable `reset_pin: GPIO27`, and the
  P4 will hard-reset it.
