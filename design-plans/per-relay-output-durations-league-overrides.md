# Per-relay output durations and per-league output overrides

Written against: 71775b6

## Evidence chain

- Surface: `renderLeagues()` (web_ui.html lines 1324–1479), `entryEditor()` (lines 632–679), `CONFIG_SECTIONS` (lines 828–910), `renderConfig()` (lines 1095–1126) — all in `external_components/web_editor/web_ui.html`; `LeagueProfile`/`PiTimerSettings`/`PiTimerGame` in `external_components/curling_engine/settings.h`; YAML parsing in `settings.cpp`; league GET/POST serialization in `external_components/web_editor/api.cpp`; output firing in `external_components/curling_engine/bell.cpp` and `curling_engine.cpp`.
- Problem: (1) Leagues have no per-league output settings — only `buzzer_relays` (parsed at settings.cpp line 1711 but never applied to games) and pace fields. Users cannot enable/disable Game Start / Countdown Warning / Game End per league, choose relays per league, or set durations per league. (2) Settings outputs use ONE duration per event type (`relay_start_duration`, `relay_countdown_duration`, `relay_end_duration` at settings.h lines 115–117) rather than a duration per relay, so e.g. RELAY1 and RELAY2 for Game Start must share one duration. (3) Schedule entries cannot override league outputs — the league select onchange (web_ui.html lines 653–666) applies `pace_of_play`/`pace_unit_duration` from the league but nothing for outputs.
- Design evidence: `bell.cpp` fires three events: start (line 69, `ring_at_start` + `relay_start_duration`), countdown warning (line 75, `countdown_relays` + `relay_countdown_duration`), buzzer/end (line 83, `relays` + `relay_end_duration`). Each reads a single global duration. `curling_engine.cpp` test_relay (lines 188–203) mirrors this. `config_edit.cpp` `complete_countdown_values` (lines 236–257) shows the pattern for completing partial config edits. The schedule league select already applies league `pace_enabled`/`pace_minutes_per_end` to entries (web_ui.html line 661–662) — outputs should follow the same cascade: league → entry override.
- Owner: `external_components/web_editor/web_ui.html` (UI), `external_components/curling_engine/settings.h/.cpp` (data model + YAML), `external_components/web_editor/api.cpp` (REST), `external_components/curling_engine/bell.cpp` (output firing consumer).
- Scope and affected surfaces: Config tab output sections (Game Start, Countdown Warning, Game End), Leagues tab (add form + edit rows + import), Schedule tab entry editor (output overrides per entry), `bell.cpp`/`curling_engine.cpp` output firing (must honor per-game/per-league overrides with per-relay durations).
- Uncertainty: None on the direction — per-relay durations, per-league output toggles, and schedule entry overrides are all explicitly requested. The exact YAML key naming follows existing conventions (`ring_at_start`, `countdown`, `relays`).

## Design decision

Introduce per-relay output durations and per-league output configuration with schedule-entry overrides:

1. **PiTimerSettings**: replace single `relay_start_duration`/`relay_countdown_duration`/`relay_end_duration` with per-relay duration fields (`relay1_start_duration`…`relay4_start_duration`, etc.). When a relay's per-relay duration is 0, fall back to the event-type default / `relay_duration`. In the config tab, render one duration field per enabled relay, each labelled with the relay name (e.g., "RELAY1 duration") next to its checkbox.

2. **LeagueProfile**: add per-event output config — enabled flag + relay list + duration for Game Start, Countdown Warning, Game End. Serialize via API GET/POST and YAML.

3. **PiTimerGame**: add optional output override fields (start/countdown/end enabled + relays + duration) so a schedule entry can override its league's outputs.

4. **bell.cpp**: when firing an output for a game, prefer the game's override, then the league's config, then global settings; use per-relay durations when available, falling back to event-type duration.

## Reuse

- Exemplar: `web_ui.html` schedule pace select (lines 641–644) — three-state Default/Show/Hide pattern for override fields.
- Exemplar: `configField()` type `'relays'` (web_ui.html lines 1045–1060) — relay checkbox group with `relayDisplayName()` (lines 912–918) for output names.
- Exemplar: `entryEditor()` league select onchange (lines 653–666) — applies league values to an entry; outputs cascade the same way.
- Existing `relays_mask()` in `bell.cpp` line 8 converts relay name vectors to bitmasks; reuse for per-event relay lists.
- `complete_countdown_values` in `config_edit.cpp` lines 236–257 — pattern for handling partial config edits (countdown needs both minutes_before and relays).

## Changes

### A. Settings per-relay durations (config tab)

1. `external_components/curling_engine/settings.h`, `PiTimerSettings` (lines 114–117)
   - Change: replace `int relay_start_duration{0}`, `int relay_countdown_duration{0}`, `int relay_end_duration{0}` with per-relay duration fields: `int relay1_start_duration{0}`…`int relay4_start_duration{0}`, `int relay1_countdown_duration{0}`…`int relay4_countdown_duration{0}`, `int relay1_end_duration{0}`…`int relay4_end_duration{0}`. Keep `int relay_duration{0}` as the ultimate fallback.
   - Preserve: `relays`, `ring_at_start`, `countdown_relays`, `countdown_minutes_before` unchanged.
   - Verify: settings can store a different duration per relay per event type.

2. `external_components/curling_engine/settings.cpp`, YAML parse (around lines 1539–1549 for buzzer block)
   - Change: parse per-relay duration keys (`relay1_start_duration`, etc.) when present; leave defaults (0) when absent.
   - Preserve: existing `relay_duration` / `relay_start_duration` / `relay_countdown_duration` / `relay_end_duration` parse for backward compatibility (treat as fallback defaults).
   - Verify: YAML with per-relay durations parses; YAML without them keeps zeros.

3. `external_components/web_editor/api.cpp`, config GET/POST (around config_edit handling)
   - Change: serialize/deserialize per-relay duration keys alongside existing ones.
   - Preserve: existing config API contract unchanged for old keys.
   - Verify: GET returns per-relay durations; POST accepts them.

4. `external_components/web_editor/web_ui.html`, `CONFIG_SECTIONS` output sections (lines 894–909) and `configField()` (lines 990–1062)
   - Change: for Game Start / Countdown Warning / Game End sections, replace the single "Output duration (seconds)" field with per-relay duration fields. Render each enabled relay's checkbox (from `relayDisplayName`) followed by a number input labelled "<relay name> duration" (e.g., "RELAY1 duration"). Use `configField` type `'relays'` for relay selection plus a new per-relay duration rendering in `renderConfig()`.
   - Preserve: relay selection checkboxes, `Test` button, `grid2` layout.
   - Verify: config tab shows "RELAY1 duration", "RELAY2 duration" etc. next to each relay checkbox for each output type.

### B. League output settings

5. `external_components/curling_engine/settings.h`, `LeagueProfile` (lines 61–74)
   - Change: add per-event output config:
     - `bool start_ring_enabled{false}; std::vector<std::string> start_relays; int start_duration{0};`
     - `bool countdown_ring_enabled{false}; std::vector<std::string> countdown_relays; int countdown_duration{0};`
     - `bool end_ring_enabled{false}; std::vector<std::string> end_relays; int end_duration{0};`
   - Preserve: existing `buzzer_relays`, `pace_enabled`, etc. unchanged.
   - Verify: league can enable/disable each output type and pick relays + duration.

6. `external_components/curling_engine/settings.cpp`, league YAML parse (lines 1711–1718 area)
   - Change: parse new output fields (`start_ring_enabled`, `start_relays`, `start_duration`, etc.) into `LeagueProfile`.
   - Preserve: existing `buzzer_relays` parse unchanged.
   - Verify: league YAML with output settings loads; without them defaults to disabled.

7. `external_components/web_editor/api.cpp`, leagues GET (line 973 area) and POST/YAML (lines 1006–1012 area)
   - Change: serialize new output fields in GET JSON and POST YAML generation.
   - Preserve: all existing league fields unchanged.
   - Verify: API round-trip preserves league output settings.

8. `external_components/web_editor/web_ui.html`, leagues Add League form (lines 1337–1368) and inline edit row (lines 1378–1423)
   - Change: add per-output-type blocks: enabled checkbox + relay checkboxes + per-relay duration fields (labelled with relay names). Reuse `relayDisplayName` and the `'relays'` field pattern.
   - Change: import default (line 1467) — new leagues default outputs to disabled.
   - Preserve: all existing league fields unchanged.
   - Verify: leagues tab shows Game Start / Countdown Warning / Game End toggles with relays and durations.

### C. Schedule entry output overrides

9. `external_components/curling_engine/settings.h`, `PiTimerGame` (lines 76–90)
   - Change: add optional output override fields mirroring LeagueProfile output config (`std::optional<bool> start_ring_override`, `std::optional<std::vector<std::string>> start_relays_override`, `std::optional<int> start_duration_override`, etc. for countdown and end).
   - Preserve: existing fields unchanged.
   - Verify: schedule entry can override any subset of league outputs.

10. `external_components/curling_engine/settings.cpp`, schedule parsing (around line 1296 pace override loop)
    - Change: after applying league pace override, apply league output settings to games that match the league name (fuzzy match), unless the entry explicitly set output overrides. Entry-level overrides win over league-level.
    - Preserve: existing pace override logic unchanged.
    - Verify: schedule entry inheriting league outputs gets them; entry override replaces league value.

11. `external_components/web_editor/web_ui.html`, `entryEditor()` (lines 632–679)
    - Change: in the league select onchange (lines 653–666), also apply league output settings to the entry (start/countdown/end enabled + relays + duration). Add per-entry output override fields in the `more` section (enabled checkboxes + relays + durations per output type).
    - Preserve: start/buzzer/name/pace fields unchanged.
    - Verify: selecting a league pre-fills output settings; editing an entry's outputs overrides the league.

12. `external_components/curling_engine/bell.cpp`, `bell_tick()` (lines 54–93) and `bell_check()` (lines 32–52)
    - Change: for each event (start, countdown, end), resolve relays and duration: game override → league config → global settings. Use per-relay duration when available, else event-type default.
    - `curling_engine.cpp` `test_relay()` (lines 188–203): same resolution for manual test.
    - Preserve: existing global-settings behavior when no overrides exist.
    - Verify: game with league output override fires the league's relays/duration; entry override replaces league.

## Scope

- Inherit: Config tab outputs, Leagues tab, Schedule tab entries, bell output firing, relay test.
- Verify: light/dark themes, narrow viewports, adding/editing leagues, selecting a league in schedule, editing entry outputs, config save/dirty tracking, API round-trip, YAML parse/render.
- Exclude: Status tab, Quick Start tab, Raw YAML tab (outputs still editable there as before), History/Diagnostics tabs, pace of play (already handled).

## Validation

- Product: Leagues have per-output toggles/relays/durations; settings show per-relay duration fields labelled by relay name; schedule entries inherit league outputs and can override them.
- Interface: Config tab output sections, Leagues add/edit/import, Schedule entry editor (all 7 days + exceptions), narrow viewports (grid2 stacks), relay test per output type.
- System: Reuses existing `relays_mask`, `relayDisplayName`, `configField`, pace-override cascade patterns; introduces no new component or token.
- Repository: `esphome config esphome/curling-timer.yaml` → Configuration is valid!; host tests (`tests/host/run.ps1`) pass.

## Stop conditions

- Stop if per-relay durations cannot be parsed from existing YAML settings files without breaking backward compatibility.
- Stop if `bell.cpp` output firing must keep using only global settings (then league/schedule overrides are UI-only).
- Stop if `PiTimerGame` output override fields conflict with existing `manual_trigger` or pace fields.

## Design documentation

- After acceptance and validation: record the new `LeagueProfile` output fields and per-relay `PiTimerSettings` duration fields in `docs/web-editor.md` (leagues section and config section), plus the YAML key names for league output settings and schedule entry output overrides.
