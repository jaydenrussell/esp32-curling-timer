# Wire the ESP32-C6 SuperMini to U213 — bench runbook

Reference tables live in `docs/wiring-U213.md` (pin map) and
`docs/flash-c6.md` (slave firmware). This is the physical order of
operations. Do NOT plug the SuperMini's USB into anything after wiring.

## 0. Before any wire

- SuperMini flashed with `network_adapter.bin` at the correct
  ESPHome-matched version, **standalone over its own USB**, verified it
  enumerates again after flash
- U213 unpowered (unplug PoE), ethernet cable can stay connected
- Parts: 5x Dupont/Dupont-cut 0.1" jumper wires (<5cm)

**No external pull-up resistors needed.** The C6 slave firmware
(`sdio_slave_initialize()`) enables the internal (~45k) pull-ups on
CMD/D0; the lines are push-pull during transactions, so internal PUs
only define idle level. If SDIO init shows CRC/timing errors, add
`sdio_frequency: 20MHz` before suspecting wiring.

## 2. Wire matrix (U213 Hat2-Bus → SuperMini)

**1-bit SDIO** on the Hat2-Bus (the HDMI add-on occupies the SDIO-Bus/
ISP-Bus area). Hat2-Bus pin numbering: LEFT column pins 1,3,5,7,9,11,
13,15 = GND,5V,G21,G20,G19,NC,3V3,5V; RIGHT column pins 2,4,6,8,10,12,
14,16 = G44,G43,G42,G41,G40,G39,G23,G22.

| U213 Hat2-Bus | SuperMini pin | Function |
|---|---|---|
| Pin 10 (G40) | GPIO19 | SDIO CLK (keep this wire short + away from others) |
| Pin 12 (G39) | GPIO18 | SDIO CMD |
| Pin 8 (G41)  | GPIO20 | SDIO D0 |
| Pin 13 (3V3) | 3V3    | logic supply — never Hat2 pins 3 or 15 (5V) |
| Pin 1 (GND)  | GND    | common ground (unsplittable) |

Silkscreen note: some SuperMini variants alias GPIO20–23 to SPI
`SCK/MO/MI`. Trust the number, not the alias.
GP21/GP22/GP23 on the SuperMini stay unconnected.

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
  common mis-wire), or add `sdio_frequency: 20MHz`
- **Wi-Fi never scans** — version mismatch between `network_adapter.bin`
  and ESPHome's esp_hosted; re-download the matching release (flash-c6.md)
- **Works, then hangs after hours** — wire EN→G27 with `reset_pin:` so
  the P4 can hard-reset the C6
