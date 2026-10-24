# ESP32 Curling Timer

This project is an ESPHome port of the **rpi-buzzer / pitimer** curling timer
firmware, originally by the CVCC IT Committee (City View Curling Club):
https://sites.google.com/cityviewcurling.ca/cvcc-it/countdown-timer

ESPHome firmware for a curling bell timer: **M5Stack Unit PoE-P4 (U213)**
driving a big-screen HDMI countdown (LT8912B add-on, 1280×720@60), two
relays commanding a 24VAC bell, and Wi-Fi 6 via an ESP32-C6 co-processor
over SDIO — with PoE Ethernet as the primary network.


## Hardware

| Part | Role |
|---|---|
| M5Stack Unit PoE-P4 (U213) | Main controller: ESP32-P4NRW32, 16MB flash + 32MB PSRAM, IP101 Ethernet PHY, PoE |
| M5Stack Add-On Display Out for PoE-P4 (U221) | LT8912B MIPI-DSI → HDMI, 1280×720@60 |
| M5Stack Unit 2Relay (Grove Port B) | Relay 1 keys 24VAC bell; relay 2 aux |
| ESP32-C6 SuperMini *(optional)* | Wi-Fi 6 failover co-processor on the U213 SDIO header (`G8–G13`) |

All pin maps: [`docs/wiring-U213.md`](docs/wiring-U213.md).
C6 first flash: [`docs/flash-c6.md`](docs/flash-c6.md).

## Firmware status

| Phase | Component | State |
|---|---|---|
| 0 · Bring-up | Core (ethernet/wifi/relays/OTA/web) | done — `esphome/curling-timer.yaml` |
| 1 · Display | `external_components/lt8912b` HDMI driver | WIP |
| 2 · Engine | `external_components/curling-engine` schedule + bells | WIP |
| 3 · UI | LVGL countdown on HDMI | pending 1–2 |
| 4 · Editor | `external_components/web-editor` REST + HTML | pending 2 |

## Building

```bash
pip install esphome
cp esphome/secrets.yaml.example esphome/secrets.yaml
$EDITOR esphome/secrets.yaml
esphome compile esphome/curling-timer.yaml
```

Ethernet-only is the default build: no C6 needed to get ethernet,
relays, OTA and the web UI. To add Wi-Fi, wire + flash the SuperMini
C6 (`docs/flash-c6.md`) and enable the package in
`esphome/curling-timer.yaml`.

First hardware flash is over the U213 USB-C; later updates go OTA or
over Ethernet.

## Conventions

- `esphome/` — YAML configs, secrets stay gitignored
- `external_components/` — owned C++/python ESPHome components
- `docs/` — wiring, flashing, build notes
- GitHub CI compiles the main YAML on every push
