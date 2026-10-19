# web-editor — schedule editor + REST API

ESPHome external component adding the pitimer web.py-style REST API and
a minimal HTML schedule editor, served alongside ESPHome's own
web_server UI:

- token-authenticated REST endpoints (schedule GET/PUT, exceptions,
  quick-start, current state)
- GET/PUT round-trips the same settings JSON shape the engine uses
- single-page HTML editor on `/edit`

## Status

**WIP** — design pending the engine component landing. Uses the same
pbkdf2 web auth as the base `web_server`.
