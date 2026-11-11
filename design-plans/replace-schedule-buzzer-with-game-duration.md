# Replace Schedule Buzzer field with Game Duration field

Written against: 71775b6

## Evidence chain

- Surface: `entryEditor()` in `external_components/web_editor/web_ui.html` (lines 632–679) — the per-game row editor inside the Weekly schedule tab.
- Problem: Each schedule row shows two time inputs side by side: "Start" (line 669) and "Buzzer" (line 670). The Buzzer time is a second clock entry that users must maintain independently of Start, even though the bell/buzzer time is functionally Start + game duration. A dedicated "Game Duration" field is absent from the Schedule tab even though leagues already carry `game_duration_minutes`. The user reports Buzzer reads as an odd function and should be removed in favor of a Game Duration field.
- Design evidence: `newEntry()` (line 628) already computes `buzzer = start + 105 * 60` (a 105-minute default game duration). The league select onchange (line 658) computes `entry.buzzer = entry.start + league.game_duration_minutes * 60`. So the system already treats buzzer as derived from start + duration; the UI merely exposes buzzer as an independent field. The `checkModel()` validator (line 1154) enforces `buzzer > start`, consistent with buzzer being start + duration.
- Owner: `external_components/web_editor/web_ui.html` — `entryEditor()` and `newEntry()`.
- Scope and affected surfaces: Weekly schedule tab (entry rows) and date exception entry rows (both use `entryEditor`). `newEntry()` default values. The save path (`/api/settings` with `schedule`) still receives `buzzer` — it must be computed client-side as `start + duration * 60`. The load path must derive `duration = (buzzer - start) / 60` for display.
- Uncertainty: How to handle entries where buzzer was hand-edited to a non-duration value (e.g. buzzer ≠ start + round minutes). Decision: compute duration from buzzer − start on load; on save recompute buzzer from start + duration. Existing `timeInput('buzzer')` removed; `timeInput('start')` kept.

## Design decision

Remove the standalone "Buzzer" time input from each schedule row and replace it with a "Game Duration (min)" number input. Buzzer is no longer hand-edited; it is derived as start + duration. On load, duration is derived as (buzzer − start) / 60 for display. On save, buzzer is recomputed and sent to the API as before, so the backend data model is unchanged. Reuse the existing `timeInput` pattern for Start and the existing number-input pattern (like league duration) for Game Duration.

## Reuse

- Existing `timeInput(key)` helper (line 637) for the Start field — unchanged.
- Existing league-duration number input pattern (`web_ui.html` line 1381: `el('input', { type: 'number', ... min: 0 })`) as exemplar for the Game Duration input.
- Existing `grid2`/`times` layout classes already wrapping Start and Buzzer (lines 668–670).
- Exemplar: `newEntry()` line 628 — buzzer already = start + 105 min default.

## Changes

1. `external_components/web_editor/web_ui.html`, `newEntry()` (line 628)
   - Change: Replace `buzzer: Math.min(start + 105 * 60, 86340)` with `game_duration_minutes: 105` and keep `buzzer` as a derived computed value (not stored directly in the entry object sent to the API; recompute on save).
   - Preserve: `start` default, all other entry fields unchanged.
   - Verify: New entries default to a 105-minute game duration.

2. `external_components/web_editor/web_ui.html`, `entryEditor()` (lines 669–670)
   - Change: Remove `el('div', { class: 'times' }, el('label', {}, 'Buzzer', timeInput('buzzer')))`. Insert a Game Duration field in its place using a number input bound to `entry.game_duration_minutes`, with `min: 0`, placeholder default, and `onchange` writing `parseInt(...)` into `entry.game_duration_minutes` and calling `changed()`.
   - Preserve: Start field, `times` wrapper class, row layout, delete button, `more` section unchanged.
   - Verify: Row shows Start + Game Duration, not Start + Buzzer.

3. `external_components/web_editor/web_ui.html`, `entryEditor()` load path (time input value derivation)
   - Change: Where `entry` is read from the model, derive `entry.game_duration_minutes = Math.round((entry.buzzer - entry.start) / 60)` for display (non-mutating, computed before render).
   - Change: On save (the `save` click handler line 1160 or just before `api('/api/settings', ...)`), recompute `entry.buzzer = Math.min(entry.start + entry.game_duration_minutes * 60, 86340)` for each entry so the API payload is unchanged.
   - Preserve: API contract (`/api/settings` receives `schedule` entries with `buzzer`).
   - Verify: Existing entries with hand-set buzzer times display a derived duration; saving recomputes buzzer correctly.

## Scope

- Inherit: All schedule entry rows (weekly schedule and date exceptions) since both use `entryEditor`.
- Verify: Schedule tab, date exceptions tab, save/dirty tracking, `checkModel()` validator still passes (buzzer = start + duration > start).
- Exclude: Leagues tab (separate data model), Quick Start tab, Config tab, Raw YAML tab, Status tab.

## Validation

- Product: Schedule rows show Start + Game Duration instead of Start + Buzzer; duration defaults to 105 minutes for new games.
- Interface: Weekly schedule (7 day folds), date exceptions, narrow viewports (grid2 stacks), add-game and edit-game flows.
- System: Reuses existing input patterns; introduces no new component or token; backend API contract unchanged.
- Repository: `esphome config esphome/curling-timer.yaml` → Configuration is valid!; host tests (`tests/host/run.ps1`) pass.

## Stop conditions

- Stop if backend `buzzer` semantics change (then derived duration may not map).
- Stop if `entryEditor` is shared with a surface that genuinely needs independent buzzer times.

## Design documentation

- After acceptance and validation: none required — no new token or component; the correction aligns UI with existing derived-time behavior.
