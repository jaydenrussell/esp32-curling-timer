# Wire the ESP32-C6 SuperMini to U213 — bench runbook

Reference tables live in `docs/wiring-U213.md` (pin map) and
`docs/flash-c6.md` (slave firmware). This is the physical order of
operations. Do NOT plug the SuperMini's USB into anything after wiring.

## 0. Before any wire

- SuperMini flashed with `network_adapter.bin` at the correct
  ESPHome-matched version, **standalone over its own USB**, verified it
  enumerates again after flash
- U213 unpowered (unplug PoE), ethernet cable can stay connected
- Parts: 7x Dupont/Dupont-cut 0.1" jumper wires (<5cm), 3x 10k resistors

## 1. Pull-ups (3x 10k), all to U213 3V3

| U213 pin | via 10k to 3V3 |
|---|---|
| G9  | SDIO CMD |
| G10 | SDIO D0  |
| G13 | SDIO D3  |

Piggyback the resistor on the Dupont connection at the U213 header pin
(one female-to-male pair joined, resistor leg in the junction).

## 2. Wire matrix (U213 side → SuperMini side)

| U213 header | SuperMini pin | Function |
|---|---|---|
| G8  | GPIO19 | SDIO CLK (keep this wire short + away from others) |
| G9  | GPIO18 | SDIO CMD (+10k to 3V3) |
| G10 | GPIO20 | SDIO D0 (+10k to 3V3) |
| G11 | GPIO21 | SDIO D1 |
| G12 | GPIO22 | SDIO D2 |
| G13 | GPIO23 | SDIO D3 (+10k to 3V3) |
| 3V3 | 3V3    | logic supply |
| GND | GND    | common ground (unsplittable) |

Silkscreen note: some SuperMini variants alias GPIO20–23 to SPI
`SCK/MO/MI`. Trust the number, not the alias.

## 3. Reserved-but-not-yet-wired

| C6 EN | U213 USB1_DP pad (GPIO27) | optional hard-reset; leave off until
the SDIO link is proven, then enable `reset_pin: GPIO27` in the wifi
package and add the wire |

## 4. Electrical checks before power-up (multimeter, ohms)

1. GND ↔ SuperMini GND continuity (few ohm max)
2. 3V3 rail resistance to GND not near 0 (no short)
3. No wire on any RMII/ISP/boot pin (C6 shares GND rail only)
4. Each SDIO line: measured through to the matching SuperMini number,
   no adjacent-pin shorts (AUTO: beep between neighbors)
5. Only then: connect PoE/ethernet power

## 5. Enable Wi-Fi package, reflash, verify

1. In `curling-timer.yaml`:
   ```yaml
   packages:
     wifi_addon: !include wifi-c6-failover.yaml
   ```
2. INSTALL. Expected log progression: SDIO transport init →
   `Connected to <ssid>` → `Active Link = ethernet` (ethernet wins)
3. Pull the ethernet cable → `Active Link` should flip to `wifi` within
   seconds; re-plug and it should flip back
4. `Wi-Fi RSSI` should report — that confirms real traffic over the C6

## 6. Troubleshooting quick hits

- **SDIO errors / transport never mounts** — swap CMD/CLK wire (most
  common mis-wire), re-check pull-ups seated
- **Wi-Fi never scans** — version mismatch between `network_adapter.bin`
  and ESPHome's esp_hosted; re-download the matching release (flash-c6.md)
- **Works, then hangs after hours** — wire EN→G27 with `reset_pin:` so
  the P4 can hard-reset the C6
