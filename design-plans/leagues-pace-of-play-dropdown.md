# Leagues pace of play: dropdown instead of checkbox

Written against: 71775b6

## Evidence chain

- Surface: `renderLeagues()` in `external_components/web_editor/web_ui.html` (lines 1324–1479) — the Leagues tab, specifically the inline league edit row (lines 1378–1423) and the Add League form (lines 1337–1368).
- Problem: Leagues pace of play is rendered as a checkbox (lines 1387–1388, 1403–1404) — a two-state control (on/off). The Schedule tab uses a three-state select dropdown for the same concept (lines 641–644: Default/Show/Hide). The user reports the leagues checkbox should match the schedule dropdown. The two surfaces edit conceptually the same field (pace of play on/off/default) but use different component variants.
- Design evidence: Schedule `entryEditor()` pace select (lines 641–644) is the governing exemplar: a `<select>` with options Default (null), Show (true), Hide (false), bound to `entry.pace_of_play` which is `std::optional<bool>` (model.h line 31). `LeagueProfile.pace_enabled` is currently `bool` (settings.h line 66) — only two states, no "Default". To render the same dropdown, leagues need a tri-state field.
- Owner: `external_components/web_editor/web_ui.html` (UI), `external_components/curling_engine/settings.h`/`settings.cpp` (data model), `external_components/web_editor/api.cpp` (REST serialization).
- Scope and affected surfaces: Leagues tab add form + inline edit rows + import default. The `pace_enabled` field flows through: UI → POST /api/leagues → api.cpp → YAML → settings.cpp → LeagueProfile → settings.h. Consumers that read `lp.pace_enabled`: settings.cpp lines 1301, 1729, 1738 (applies pace override to scheduled games); `PiTimerGame.pace_of_play_override` is already `std::optional<bool>` (settings.h line 82).
- Uncertainty: `pace_enabled` is `bool` today. Changing to `std::optional<bool>` alters the YAML/JSON schema (null = Default). Existing league files with `pace_enabled: true/false` remain valid; `pace_enabled:` (absent/null) becomes a new "Default" state.

## Design decision

Change `LeagueProfile.pace_enabled` from `bool` to `std::optional<bool>` (nullopt = Default, true = Show, false = Hide) to match the schedule's tri-state pace model. Replace the leagues checkbox with a `<select>` dropdown identical in shape to the schedule's pace select (Default/Show/Hide). Update all serialization points (API GET/POST/YAML, settings YAML parse) to handle the optional. No CSS or token changes; reuse the existing `el('select', ...)` pattern and option labels from the schedule exemplar.

## Reuse

- Exemplar: `web_ui.html` lines 641–644 — schedule pace `<select>` with Default/Show/Hide options and `onchange` mapping `{ '': null, on: true, off: false }`.
- Existing `el('select', ...)` pattern used throughout config fields (line 1011).
- Existing `PiTimerGame.pace_of_play_override: std::optional<bool>` (settings.h line 82) — same type as the new league field.
- Existing `grid2` layout wrapping pace checkbox (line 1402–1404) — keep layout, swap control.

## Changes

1. `external_components/curling_engine/settings.h`, `LeagueProfile` (line 66)
   - Change: `bool pace_enabled{false}` → `std::optional<bool> pace_enabled{std::nullopt};`
   - Preserve: `pace_minutes_per_end`, all other LeagueProfile fields unchanged.
   - Verify: `pace_enabled` can be nullopt (Default), true (Show), or false (Hide).

2. `external_components/curling_engine/settings.cpp`, league YAML parsing (line 1701)
   - Change: `lp.pace_enabled = pe->b` → handle optional: if present parse bool, if absent leave nullopt.
   - Change: lines 1301, 1729, 1738 — `g.pace_of_play_override = lp.pace_enabled` (optional<bool> → optional<bool> assignment works as-is; nullopt means no override applied, game falls through to next override rule).
   - Preserve: all other league parsing unchanged.
   - Verify: YAML with `pace_enabled: true/false` parses correctly; YAML without `pace_enabled` yields nullopt.

3. `external_components/web_editor/api.cpp`, leagues GET serialization (line 973)
   - Change: `o["pace_enabled"] = lp.pace_enabled` → serialize null when nullopt, bool otherwise.
   - Change: leagues POST/YAML generation (line 1006) → `pace_enabled: true/false` when set, `pace_enabled:` (empty) or omit when nullopt.
   - Preserve: all other league fields unchanged.
   - Verify: GET returns `pace_enabled: null` for Default; POST accepts null/true/false.

4. `external_components/web_editor/web_ui.html`, leagues Add League form (line 1346)
   - Change: `pace_enabled: document.getElementById('lg-add-pace').checked` → `pace_enabled: <select value mapped to null/true/false>`, matching schedule's `{ '': null, on: true, off: false }`.
   - Preserve: `pace_minutes_per_end` input unchanged.
   - Verify: Add form shows Default/Show/Hide dropdown instead of checkbox.

5. `external_components/web_editor/web_ui.html`, leagues inline edit row (lines 1387–1388, 1403–1404)
   - Change: Replace checkbox `paceCheck` + label "Pace of play" with `<select>` Default/Show/Hide bound to `lp.pace_enabled`, same option mapping as schedule exemplar.
   - Preserve: `grid2` layout, all other row fields unchanged.
   - Verify: Edit row shows Default/Show/Hide dropdown.

6. `external_components/web_editor/web_ui.html`, league import default (line 1467)
   - Change: `pace_enabled: false` → `pace_enabled: null` (imported leagues default to "Default" pace).
   - Preserve: all other import fields unchanged.
   - Verify: Imported leagues have `pace_enabled: null`.

## Scope

- Inherit: All league rows (add form, edit rows, import), all consumers of `LeagueProfile.pace_enabled`.
- Verify: Leagues tab add/edit flow, save/dirty tracking, API round-trip (GET/POST), settings YAML parse/render, schedule pace override application (games matching league name still get pace override when set).
- Exclude: Schedule tab entry editor (already correct, exemplar only), Config tab pace of play (separate config, unrelated), Status/Quick Start/Raw/History/Diagnostics tabs.

## Validation

- Product: Leagues pace of play shows Default/Show/Hide dropdown matching the Schedule tab's pace select.
- Interface: Leagues tab add league, edit league, delete league; import from Club Manager; light/dark themes; narrow viewports.
- System: Reuses existing select pattern and option labels; introduces no new component or token; type matches existing `std::optional<bool>` used elsewhere.
- Repository: `esphome config esphome/curling-timer.yaml` → Configuration is valid!; host tests (`tests/host/run.ps1`) pass.

## Stop conditions

- Stop if `pace_enabled` must remain `bool` for backward compatibility with existing settings files (then keep checkbox or add a separate tri-state field).
- Stop if the backend does not accept `null` for `pace_enabled` in POST /api/leagues.

## Design documentation

- After acceptance and validation: record the `LeagueProfile.pace_enabled` type change (`bool` → `std::optional<bool>`) and its YAML/JSON null semantics if the project maintains a data-model reference; none currently exists for leagues schema.
