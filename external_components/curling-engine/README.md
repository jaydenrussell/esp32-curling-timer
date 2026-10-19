# curling-engine — schedule/bell engine (pitimer port)

ESPHome external component porting the rpi-buzzer / pitimer v2.4.4
logical engine to ESP32-P4:

- schedule validation (durability cap 3h, overlap incl. per-entry
  `header_hold`, no-leading-zero time format, hour 0 as raw seconds)
- one buzz per `(date, start)` with `BUZZER_GRACE_SECONDS = 5`
- splice-only editing (non-schedule config sections preserved verbatim)
- NVS/flash-backed store with rolling backups + version hash
- quick-start presets: PRACTICE (5m, during-buzzer, header_hold 60),
  6-end game 75m, 8-end game 105m

## Status

**WIP** — header contract in `include/curling_engine.h`, matching the
Python sources listed for reference porting.
