# The on-device timer web interface (`web_editor`)

A pitimer-style editor served **by the ESP32-P4 itself** at
`http://<device-ip>:8080` (`web_editor.port`, default 8080), next to
ESPHome's stock `web_server` UI on port 80, which is left untouched.
It covers every parameter of `pitimer_settings.example.yml` - the
weekly schedule and date exceptions as structured editors, plus a
**Config tab** for everything else (fonts, colours, relays, footer,
pace of play), and the full raw-YAML view of the file.

The page is a port of rpi-buzzer's `pitimer/web.html` (same tabs,
same savebars, same REST calls) with the Config tab added.

## Enabling it

```yaml
external_components:
  - source: ../external_components        # repo-relative
    components: [curling_engine, web_editor]

# the sntp block already exists in curling-timer.yaml; both components
# take the clock from it
curling_engine:
  time_id: sntp_time
  store_path: /flash                      # optional (default)

web_editor:
  password: !secret web_password          # min 6 characters
  port: 8080                              # optional (default 8080)
```

The blocks are enabled in `esphome/curling-timer.yaml` (repo builds
use the repo-relative source above). To build outside the repo — e.g.
the Home Assistant **ESPHome Device Builder** add-on — swap the source
for the GitHub form (this public repo):

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/jaydenrussell/esp32-curling-timer
      ref: main
      path: external_components   # required: ESPHome only auto-scans
                                  # components/ and esphome/components/
    components: [curling_engine, web_editor]
```

## Logging in

- The password comes from `web_editor.password` (keep it in
  `secrets.yaml`). It is hashed with PBKDF2-SHA256 (60 000
  iterations) and stored in NVS, namespace `pitimer_web`
  (`password_hash`, `secret`).
- Changing the YAML password re-hashes it **and rotates the session
  secret**, signing everyone out - same behaviour as pitimer.
- Sessions are HMAC-signed cookies (`pitimer_session`, 30 days,
  `HttpOnly`, `SameSite=Strict`).
- After 5 failed logins from one address within 5 minutes the login
  answers `429` for that address.
- Until the SNTP clock has synced, login/quick-start answer
  `503 clock not synced yet` and the Status tab shows a waiting
  banner - the schedule and Raw YAML tabs still work.

## Tabs

| Tab | What it does |
|---|---|
| Status | Live countdown (5 s refresh), enable/disable, quick-start presets (Practice 5 min / 6 ends 75 min / 8 ends 105 min) plus a custom timer, today's games, cancel a running quick start |
| Weekly schedule | 7 day folds, per-game start/buzzer/name with optional overrides (header text during buzzer, header hold, pace of play, minutes per unit), copy-day, savebar |
| Date exceptions | Add a date (optionally prefilled from that weekday), notes, past-date filter, delete |
| Config | Every non-schedule parameter as grouped form fields; **its own savebar and dirty flag** (Schedule and Config unsaved at once stack two savebars; leaving the page warns about either) |
| Raw YAML | The whole file - Check (validate) / Save & apply / Reload |
| History | 30 rolling backups, view or restore (the current file is backed up first) |

Schedule and Config saves are version-checked: a `409` means someone
(or a quick start) changed the file meanwhile; **Discard** reloads the
latest version. Config saves send only the fields you actually
changed.

## REST API

Paths and status codes mirror `pitimer/web.py` so the original
`test_web.py` scenarios map onto it:

- `GET  /api/status` `settings` `raw` `backups` `config`;
  `GET /api/backups/<name>`
- `POST /api/login` `logout` `settings` `enabled` `raw` `validate`
  `restore` `quickstart` `quickstart/cancel` `config`
- Statuses: `400` `401` `403` `404` `409` `413` `415` `422` `429`
  `500`, plus the non-pitimer `503` clock gate above
- `GET/POST /api/config` is new: a flat object of dotted keys
  (`config.header.font_size`, `config.buzzer.relays`, ...) over the
  engine's line-splice (`config_edit`), version-checked like the
  schedule; keys missing from the file answer `422 no config setting
  ...` (re-add them via Raw YAML first)
- All responses carry `Cache-Control: no-store`,
  `X-Content-Type-Options: nosniff`, `X-Frame-Options: DENY`,
  `Referrer-Policy: no-referrer`; the page itself adds pitimer's CSP.
  Bodies over 512 KB are `413`, non-JSON posts `415`, cross-site
  `Origin` `403`.

## Storage

- Settings: `/flash/pitimer_settings.yml` on the `storage` partition
  (2 MB FAT, wear-levelled), written atomically (tmp + rename) with a
  content-hash `version` for conflict detection.
- Backups: `/flash/backups/`, latest 30, one written before every
  change.
- NVS: `pitimer_web` holds only the password hash and session secret.

## First flash / migration notes

- The custom partition table (`storage` as a FAT data partition)
  **wipes NVS on the first flash**: Home Assistant pairing, API keys
  and relay restore states are lost. Flash once with `erase_flash`,
  then re-adopt the device.
- A settings file from the original Raspberry Pi can be pasted into
  the **Raw YAML** tab and saved; keep the pi's file as a backup
  first (History keeps 30 versions of whatever you save).

## Editing the UI

The page source is `external_components/web_editor/web_ui.html`. At
codegen time the component regenerates `web_ui.h` (raw-string
literal, `{{version}}` replaced by the ESPHome version) and only
rewrites it when the HTML changed. After editing the HTML, re-run
`esphome compile` (or `esphome config`) and commit **both** files.

## Tests

- `tests/host/run.ps1` (MSVC) / `tests/host/run.sh` (g++) - 87
  doctest cases over the engine (schema, splice, quick start, store,
  config_edit) and web auth.
- CI (`.github/workflows/build.yml`): `host-tests` runs `run.sh`,
  `compile` builds `esphome/curling-timer.yaml` via
  `esphome/build-action`.
