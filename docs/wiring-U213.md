# Hardware Wiring — M5Stack Unit PoE-P4 (U213)

Master pin map for the ESP32 Curling Timer. Alltables below were settled
from board documentation and photos during design (no SD card — the
microSD slot stays unused, freeing the 4-bit SDIO bus for Wi-Fi).

## Board overview

| Item | Detail |
|---|---|
| Main MCU | ESP32-P4NRW32 — 16MB flash + 32MB octal PSRAM, 400MHz max |
| Ethernet | IP101GRI PHY, PoE 802.3at |
| Wi-Fi | via ESP32-C6 SuperMini on SDIO header (below) |
| Display | M5Stack Add-On Display Out for PoE-P4 (U221, LT8912B) |
| Relays | M5Stack Unit 2Relay on Grove Port B |

## Ethernet PHY (primary network)

| Signal | P4 pin |
|---|---|
| PHY type | IP101 (IP101GRI) |
| PHY address | 1 |
| MDC | GPIO31 |
| MDIO | GPIO52 |
| PHY power | GPIO51 |
| RMII ref clock | external 50MHz in (`clk_mode: GPIO0_IN`) |

## Display add-on (U221 — LT8912B MIPI-DSI → HDMI)

| Signal | P4 pin |
|---|---|
| LCD reset | GPIO4 |
| LCD backlight | GPIO33 |
| TP / display I2C SDA | GPIO0 |
| TP / display I2C SCL | GPIO1 |
| TP reset | GPIO3 |
| TP interrupt | GPIO2 |

Supported modes: **1280x720@60** (default, chosen) or 1920x1080@30.
Connection is the 24P FPC to P4 MIPI-DSI 2-lane. No stock ESPHome driver
for the LT8912B — provided by `external_components/lt8912b`.

## SDIO header (Wi-Fi co-processor bus)

2.54mm 9-pin header on the bottom edge. SDIO bus runs on G8–G13;
USB1 pads double as free GPIO if USB1 is unused.

| Header pin | P4 GPIO | Used for |
|---|---|---|
| G8 | GPIO8 | SDIO CLK |
| G9 | GPIO9 | SDIO CMD |
| G10 | GPIO10 | SDIO D0 |
| G11 | GPIO11 | SDIO D1 |
| G12 | GPIO12 | SDIO D2 |
| G13 | GPIO13 | SDIO D3 |
| USB1_DP | GPIO27 | spare (C6 EN reset wire, optional) |
| USB1_DM | GPIO26 | spare |
| 5V | — | 5V rail |

## ISP header (first-flash / recovery)

| Signal | Note |
|---|---|
| GND | common ground |
| BOOT | GPIO35 |
| RST | reset pad |
| U0RX | GPIO28 |
| U0TX | GPIO37 |
| 3V3 | logic rail |

## Relay (Unit 2Relay, Grove Port B — HY2.0-4P)

| Grove pin | P4 pin | Note |
|---|---|---|
| G53 | GPIO53 | Relay 1 control (manual LED on = closed) |
| G54 | GPIO54 | Relay 2 control |
| 5V | — | unit power |
| GND | — | unit ground |

Relay 1 (`relay_bell_ring`) keys the 24VAC bell circuit; relay 2 aux.

## ESP32-C6 SuperMini (Wi-Fi co-processor)

Chip fixed mapping — SDIO slave IOs are always GPIO18–23 on the C6.

| C6 SuperMini pin | U213 pin | Function | Extra |
|---|---|---|---|
| GPIO19 | G8 | SDIO CLK | — |
| GPIO18 | G9 | SDIO CMD | 10k pull-up to 3V3 |
| GPIO20 | G10 | SDIO D0 | 10k pull-up to 3V3 |
| GPIO21 | G11 | SDIO D1 | — |
| GPIO22 | G12 | SDIO D2 | — |
| GPIO23 | G13 | SDIO D3 | 10k pull-up to 3V3 |
| EN | GPIO27 (USB1_DP pad) | P4 can hard-reset C6 | optional but recommended |
| 3V3 | 3V3 | logic rail | — |
| GND | GND | common ground | mandatory |
| USB-C | bench only | first flash of `network_adapter.bin` | see docs/flash-c6.md |

Rules: wires under 5cm, all grounds common, inspect every joint before
applying 5V. Silkscreen on SuperMini variants may label GPIOs 20–23 as
SPI `SCK/MO/MI` — same pins, check the number, not the alias.

## Bring-up order

1. U213 + U221 bench, flash minimal config (this yaml without wifi block)
2. Add SuperMini C6, flash `network_adapter.bin` over its USB-C (docs/flash-c6.md),
   then enable `esp32_hosted:` and `wifi:` blocks
3. Confirm both ethernet and Wi-Fi come up, remove the bench flash cable
4. Wire Relay card + 24VAC test load
5. Enable `external_components` WIP blocks (display → engine → UI → editor)
