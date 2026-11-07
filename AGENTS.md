# AGENTS.md

## How to run

- **Build**: `pip install esphome` → `cp esphome/secrets.yaml.example esphome/secrets.yaml` → fill secrets → `esphome compile esphome/curling-timer.yaml`
- **Lint**: `esphome lint esphome/curling-timer.yaml`
- **Host tests** (no hardware): `tests/host/run.sh` (Linux/g++) or `tests/host/run.ps1` (Windows/MSVC). Runs doctest cases over settings, model, quickstart, sha256, store, fs_std, config_edit, render, bell, and web auth.
- **No typecheck step** — ESPHome uses C++/Python; verification is via build + host tests only.
- **Typecheck**: ESPHome uses C++/Python; no formal typecheck beyond ESP-IDF toolchain

## Entrypoints

- Main build config: `esphome/curling-timer.yaml` — the repository/CI build target
- Paste-ready Home Assistant variant: `esphome/curling-timer.ha.yaml` — fetched from GitHub by `type: git` external_components source; do not hand-edit, it is regenerated from `curling-timer.yaml`
- Wi-Fi C6 failover package: `esphome/wifi-c6-failover.yaml` — include only after C6 is wired and flashed (`packages: { wifi_addon: !include wifi-c6-failover.yaml }`)
- Secrets: `esphome/secrets.yaml` (gitignored; copy from `secrets.yaml.example`)
- External components live in `external_components/` — `lt8912b` (HDMI), `curling_engine` (schedule + bells), `web_editor` (on-device UI), `calendar_import`

## Command order

- `lint` before `compile` to catch YAML errors early
- Flash C6 first (`firmware/network_adapter_esp32c6.bin` over SuperMini USB-C), then enable Wi-Fi in `curling-timer.yaml`
- When bumping component code: update `esphome/component_ref.txt`, regenerate the paste variant, commit everything together, then tag. CI verifies the paste variant matches the generator.

## Architecture

- Single ESPHome package: `esphome/curling-timer.yaml` is the build entrypoint
- Hardware: M5Stack Unit PoE-P4 (U213) ESP32-P4NRW32 (16MB flash, 32MB PSRAM) + IP101 Ethernet PHY (PoE) + optional ESP32-C6 SuperMini on SDIO header for Wi-Fi 6 failover
- Ethernet-only is the default build; Wi-Fi requires flashing C6 + enabling `wifi-c6-failover.yaml`
- GPIO pin map: `docs/wiring-U213.md` — verify all pins against this before adding hardware
- C6 first flash: `docs/flash-c6.md`

## Testing

- Host tests: `tests/host/run.sh` or `tests/host/run.ps1` — 87 doctest cases, no hardware needed
- Hardware-in-the-loop requires physical U213 + relays + LT8912B display
- No separate test suite beyond ESPHome's built-in test framework + host doctests

## Common pitfalls

- **Secrets**: Never commit `esphome/secrets.yaml` — use `secrets.yaml.example` and fill securely. Missing secrets fail the build.
- **GPIO pin mapping**: Always verify against `docs/wiring-U213.md` before adding hardware. RMII TXD0/TX1 are boot-strapping pins (G34/G35); the SDIO-Bus header pin 9 is 5V — never feed the C6 from there.
- **C6 version match**: `network_adapter_esp32c6.bin` must match the ESPHome `esp_hosted` ref — mismatch traps the P4 in a boot loop.
- **Task priorities**: ESP-IDF tasks need explicit priorities; avoid blocking `delay()` in lambdas.
- **engineering_sample**: U213 is pre-rev3 silicon — `engineering_sample: true` is required in the YAML.
- **Web editor password**: must be ≥ 6 characters or the editor fails to start.
- **Web UI**: paste-ready `curling-timer.ha.yaml` is for Home Assistant ESPHome Device Builder — repo builds use `curling-timer.yaml`, not the `.ha` variant.


## Subagent Delegation Guidelines
- `@planner`: Default agent for breaking down features and system architecture.
- `@reviewer`: Run before committing any custom C++ components or complex YAML changes.
- `@release`: Call once changes are verified to build, commit, tag, and push.