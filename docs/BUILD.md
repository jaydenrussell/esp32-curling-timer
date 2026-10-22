# Building and deploying the curling timer

## Device Builder (Home Assistant add-on)

The paste-ready YAML is generated from `esphome/curling-timer.yaml` and lives
at `esphome/curling-timer.ha.yaml`:

    https://raw.githubusercontent.com/jaydenrussell/esp32-curling-timer/main/esphome/curling-timer.ha.yaml

1. Copy the whole file into `/config/esphome/curling-timer.yaml`.
2. Add the secrets below to `/config/secrets.yaml` in the same folder.
3. Install / update the add-on, then flash.

The external components are fetched from GitHub as a `type: git` source with
`refresh: 0s`, so every install re-fetches `main` - a fix pushed to the repo
lands on the device with the next install.

### If the build fails with a missing symbol or member

The add-on caches the git checkout of `external_components`. When it serves an
older clone, the pasted YAML (newer) calls component APIs that the clone does
not have yet, and the compile fails - for example
`'class esphome::web_editor::WebEditorComponent' has no member named 'status'`.

Fix it by making the add-on fetch the current `main`:

1. Update the ESPHome Device Builder add-on, then install again.
2. If that is not enough, delete the add-on's cached checkout (the
   `external_components` folder in the add-on's data/build directory) and
   install again.

The "Web Editor" diagnostic entity shows which component the device actually
runs: `listening on port 8080 (web_editor 2026-10-22.1)`. If the stamp is
older than the repo's `external_components/web_editor/VERSION`, the clone is
stale.

## secrets.yaml

Every secret below is referenced by `!secret` in the YAML. A missing secret
fails the build with a clear error.

| Secret | Used for | How to make it |
| --- | --- | --- |
| `curling_timer__encryption_key` | Native API encryption (Home Assistant) and OTA encryption + OTA client authentication | `openssl rand -base64 32` |
| `web_password` | Web server (port 80) and web editor (port 8080) login | at least 6 characters |
| `wifi_ssid` / `wifi_password` | Wi-Fi failover via the C6 co-processor | from the router |
| `ap_password` | Setup-mode hotspot | any password |

The two encryption keys are 32 random bytes in base64. The API key has to be
the same value in Home Assistant (the "encryption key" field of the ESPHome
integration) or the connection is refused.

`web_password` is used twice: as the digest login for the ESPHome web server
on port 80 and as the web editor password on port 8080. The editor refuses to
start with a password shorter than 6 characters (same rule as pitimer); the
"Web Editor" diagnostic entity then reports
`failed: web_editor.password is shorter than 6 characters`.

## Timezone

`substitutions.timezone` is an IANA name, e.g. `America/Toronto`. It drives
the SNTP timezone used for the schedule lookup (the games are defined in local
time).

## Repo / CI build

    pip install esphome
    cd esphome
    python -m esphome config curling-timer.yaml
    python -m esphome compile curling-timer.yaml

`tools/make_ha_yaml.py` regenerates `curling-timer.ha.yaml` from
`curling-timer.yaml`; CI runs it with `--check` so the paste variant can never
drift from the source.

The host test suite (settings parser, schedule engine, web editor API, bell
events) runs without hardware:

    powershell -NoProfile -ExecutionPolicy Bypass -File tests\host\run.ps1

## Hardware notes

* ESP32-P4 revision < 3.0 (the U213 shows "Chip Revision: 103") needs
  `engineering_sample: true`, and the CPU is capped at 360 MHz.
* `CONFIG_VFS_SUPPORT_DIR` is enabled through `sdkconfig_options`: the
  settings store keeps backups in a folder and lists them for the web editor,
  and ESP-IDF only provides `opendir`/`readdir`/`mkdir` when that option is on.
* The store lives on the FAT partition labelled `storage`, mounted at
  `/flash` (see `docs/wiring-U213.md`).
