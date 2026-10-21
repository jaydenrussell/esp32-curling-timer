# web-editor — schedule/config editor + REST API

ESPHome external component adding the pitimer `web.py`-style REST API
and the full single-page editor, served from the device alongside
ESPHome's own `web_server` UI (stock UI on :80, this one on
`web_editor.port`, default **8080**):

- cookie-authenticated REST endpoints mirroring pitimer `web.py`:
  status, settings, raw YAML, backups/restore, quick-start,
  enable/disable — plus **GET/POST `/api/config`** for the structured
  Config tab (flat dotted `config.*` keys over the engine's
  `config_edit` splice)
- pbkdf2-SHA256 password + HMAC session cookie + per-IP login limiter,
  hash/secret in NVS
- single-page HTML editor at **`GET /`** (Status, Weekly schedule,
  Date exceptions, Config, Raw YAML, History tabs)

## Web UI source

The page source is `web_ui.html`. At codegen time `__init__.py`
regenerates `web_ui.h` (a raw-string literal, plus the running
ESPHome version in place of `{{version}}`) and only rewrites it when
the HTML changed — a define would blow the Windows command-line limit
at this size. After editing `web_ui.html`, just re-run
`esphome compile` (or `esphome config`) to refresh `web_ui.h`; commit
both files together.

## Status

Landed: REST API + auth + embedded UI behind `CurlingEngineComponent`.
On-device QA pending (Phase D).
