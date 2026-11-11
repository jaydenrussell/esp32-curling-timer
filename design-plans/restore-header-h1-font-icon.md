# Restore font icon to header h1

Written against: c1427ae

## Evidence chain

- Surface: sticky header brand in `external_components/web_editor/web_ui.html` (lines 216–220) — h1 `Countdown Timer`, version banner, header-actions.
- Problem: header h1 renders `Countdown Timer` with no icon, while every primary nav tab carries an emoji icon. The user's prior complaint noted "the previous version had font icons for header." Absence of the brand icon reduces header/nav visual coherence on the shared top bar.
- Design evidence: `TABS` (line 241) defines each tab with an emoji icon (`📊 ⚡ 📅 🏆 📈 ⚙ 🩺`); nav render (lines 344–345) emits `${icon} ${title}` (wide) or `icon` (narrow). This establishes emoji icons as this surface's branding/nav icon convention. header h1 (line 217) breaks that convention on the same top bar — a direct internal contradiction.
- Owner: `external_components/web_editor/web_ui.html` — header h1 markup (line 217); `TABS`/nav render (lines 241, 344–345) as exemplar.
- Scope and affected surfaces: header h1 only; all tabs (header is sticky and always present after auth).
- Uncertainty: exact emoji glyph. Evidence supports an emoji icon per nav convention; `⏱️` (stopwatch) is the clearest match for a countdown timer brand, consistent with nav tabs using context-appropriate emojis (📊 status, ⚡ quickstart, 📅 schedule).

## Design decision

Prepend `⏱️` stopwatch emoji icon to the header h1 text, matching the nav icon convention (`TABS` line 241, render line 344–345). Do not change `.header-brand` layout, `header-version`, or any nav markup/CSS.

## Reuse

- Existing nav icon convention: `TABS` (line 241) + `${icon} ${title}` render (lines 344–345) — exemplar for brand icon placement.
- Existing `header h1` rule (line 47) and `.header-brand` (line 48) unchanged; icon flows as inline text before `Countdown Timer`.

## Changes

1. `external_components/web_editor/web_ui.html`, header-brand h1 (line 217)
   - Change: `<h1>Countdown Timer</h1>` → `<h1>⏱️ Countdown Timer</h1>`
   - Preserve: `header-brand` div, `header-version` span, gap/padding, all other header markup unchanged.
   - Verify: header h1 shows `⏱️ Countdown Timer`; icon present on all tabs; narrow viewport unaffected.

## Scope

- Inherit: header h1 across all tabs (sticky header always present).
- Verify: Status/Quick Start/Schedule/Config/Diagnostics tabs; light/dark themes (emoji color-scheme neutral); narrow viewport (lines 58–62).
- Exclude: nav tab icons (unchanged), header-actions buttons (Finding 1 handled separately), footer/savebar (unchanged).

## Validation

- Product: header h1 carries an emoji icon matching nav convention; prior "font icons for header" restored in surface-appropriate form.
- Interface: light/dark themes, narrow viewport, login-gated header.
- System: inline text change only; no CSS, token, component, or backend change.
- Repository: `esphome lint esphome/curling-timer.yaml` clean; host tests (`tests/host/run.ps1`) pass (static HTML, no logic change).

## Stop conditions

- Stop if product prefers no brand icon (then nav convention would be the outlier, not the header).
- Stop if a different icon glyph is mandated by brand guidelines (none found — no DESIGN.md exists).

## Design documentation

- After acceptance and validation: none required — no new token or component; alignment of header markup with existing nav icon convention.
