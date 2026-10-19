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

M5Stack official U213 pin map — RMII data pads are fixed by chip IOMUX.

| Signal | P4 pin | Note |
|---|---|---|
| PHY type | IP101 (IP101GRI) | driver `type: IP101` |
| PHY address | 1 | |
| SMI MDC | GPIO31 | |
| SMI MDIO | GPIO52 | |
| PHY reset | GPIO51 | ESPHome `power_pin` |
| RMII RXDV | GPIO28 | fixed pad |
| RMII RXD0 | GPIO29 | fixed pad |
| RMII RXD1 | GPIO30 | fixed pad |
| RMII TXD0 | GPIO34 | fixed pad (boot-strapping pin) |
| RMII TXD1 | GPIO35 | fixed pad (boot-strapping pin) |
| RMII TXEN | GPIO49 | fixed pad |
| RMII REF_CLK in | **GPIO50** | only GPIO32/44/50 are valid clock-input pads |

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

## SDIO-Bus header (Wi-Fi co-processor bus) — 2.54-9P

Confirmed against the official M5Stack Unit PoE-P4 PinMap (`docs.m5stack.com/en/unit/Unit_PoE-P4`).
On the P4 host the SDMMC slot pins go through GPIO matrix, so G8–G13 are our
routing choice (same choice M5Stack uses on Tab5).

| Header pin | P4 GPIO | Used for |
|---|---|---|
| 1 | GPIO13 | spare (D3, unused in 1-bit SDIO) |
| 2 | GPIO12 | spare (D2, unused in 1-bit SDIO) |
| 3 | GPIO11 | spare (D1, unused in 1-bit SDIO) |
| 4 | GPIO10 | SDIO D0 |
| 5 | GPIO9 | SDIO CMD |
| 6 | GPIO8 | SDIO CLK |
| 7 | GPIO27 | USB1_DP — C6 EN reset wire, optional |
| 8 | GPIO26 | USB1_DM spare |
| 9 | — | **5V rail — never feed the SuperMini from here** |

## ISP-Bus header — 2.54-6P

| Header pin | Signal | Note |
|---|---|---|
| 1 | GND | common ground |
| 2 | GPIO35 | BOOT label |
| 3 | RST | P4 reset |
| 4 | GPIO38 | UART0 RX |
| 5 | GPIO37 | UART0 TX |
| 6 | **3V3** | **logic rail — the SuperMini's 3V3 feed comes from here** |

## Hat2-Bus — 2.54-16P (top of unit)

Expansion bus for future M5 hats. **Not for the C6** — despite G19–G23
being present, keep the SDIO link on the SDIO-Bus above. Two of its pins
are 5V; easy to miswire against the 1.6× larger 9P SDIO header.

| Pin L | Pin | Pin R | Pin |
|---|---|---|---|
| 1 GND | 2 G44 | | |
| 3 5V | 4 G43 | | |
| 5 G21 | 6 G42 | | |
| 7 G20 | 8 G41 | | |
| 9 G19 | 10 G40 | | |
| 11 NC | 12 G39 | | |
| 13 3V3 | 14 G23 | | |
| 15 5V | 16 G22 | | |

## Onboard indicators

| Signal | P4 pin | Note |
|---|---|---|
| IR transmitter | GPIO14 | IR_TX, MHS153IRCT |
| RGB LED green | GPIO15 | |
| RGB LED blue | GPIO16 | |
| RGB LED red | GPIO17 | ESPHome `status_led` / bench blinky |
| User button | GPIO45 | KEY_USER |

## Relay (Unit 2Relay, Grove Port B — HY2.0-4P)

| Grove pin | P4 pin | Note |
|---|---|---|
| G53 | GPIO53 | Relay 1 control (manual LED on = closed) |
| G54 | GPIO54 | Relay 2 control |
| 5V | — | unit power |
| GND | — | unit ground |

Relay 1 (`relay_bell_ring`) keys the 24VAC bell circuit; relay 2 aux.

## ESP32-C6 SuperMini (Wi-Fi co-processor) — wired on the **Hat2-Bus**

The HDMI add-on physically occupies the SDIO-Bus/ISP-Bus header area, so
the C6 link lands on the Hat2-Bus (2.54-16P, top of unit). P4 host SDMMC
slot 1 is fully GPIO-matrixable (IDF `sd_host_sdmmc.c`), and G39/G40/G41
are claimed by no other U213 peripheral; RMII stays on G28/29/30/34/35/
49/50.

Chip fixed mapping — SDIO slave IOs are always GPIO18–23 on the C6.

| C6 SuperMini pin | Hat2-Bus pin | P4 GPIO | Function |
|---|---|---|---|
| GPIO19 | 10 | GPIO40 | SDIO CLK (shortest wire) |
| GPIO18 | 12 | GPIO39 | SDIO CMD |
| GPIO20 | 8 | GPIO41 | SDIO D0 |
| 3V3 | 13 | — | logic supply (left row) |
| GND | 1 | — | common ground (left row) |

Bus runs **1-bit SDIO** (`bus_width: 1`): the SuperMini lacks headers on
C6 GPIO21–23, and the slave firmware's internal ~45k pull-ups hold
CMD/D0 idle-high — no external resistors. `sdio_frequency: 20MHz`
initially; raise to 40MHz once stable.

**Future 4-bit upgrade:** add SM GP21/G22/G23 → Hat2 pin 6 (G42) /
pin 4 (G43) / pin 2 (G44), set `bus_width: 4` + `d1/d2/d3_pin`. The
whole 4-bit bus then rides one contiguous right-row strip (pins 2–12).

Hat2-Bus pins 3 and 15 are 5V — never route them to the SuperMini.

Rules: wires under 5cm, all grounds common, inspect every joint before
applying 5V. Silkscreen on SuperMini variants may label GPIOs 20–23 as
SPI `SCK/MO/MI` — same pins, check the number, not the alias.

## Bring-up order

1. U213 + U221 bench, flash minimal config (ethernet-only default —
   Wi-Fi is an optional package and not needed for this step)
2. Add SuperMini C6, flash `network_adapter.bin` over its USB-C (docs/flash-c6.md),
   then enable the `wifi_addon` package in `curling-timer.yaml`
3. Confirm both ethernet and Wi-Fi come up, remove the bench flash cable
4. Wire Relay card + 24VAC test load
5. Enable `external_components` WIP blocks (display → engine → UI → editor)
