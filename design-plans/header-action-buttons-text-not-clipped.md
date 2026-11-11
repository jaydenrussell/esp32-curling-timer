# Fix header action buttons clipped by icon-only layout

Written against: c1427ae

## Evidence chain

- Surface: sticky header in `external_components/web_editor/web_ui.html` (lines 216–220) — brand h1, version banner, header-actions with Help and Log out buttons.
- Problem: header-actions buttons render as fixed 36×36px icon-only squares (`button.btn.icon` at line 81: `width: 36px; justify-content: center`), so their text labels ("Help", "Log out") overflow and are clipped. The user complaint cites "font spacing broken."
- Design evidence: `.header-actions button` (line 51) declares text-button sizing (`padding: 6px 10px; font-size: 12px; min-height: 30px;`). header-actions buttons (line 219) carry both `btn` and `icon`. `button.btn.icon` (line 81) has higher specificity (0,3,0) than `.header-actions button` (0,1,1) and wins, forcing `width: 36px` and center-only layout. Removing `icon` lets text-button sizing govern.
- Owner: `external_components/web_editor/web_ui.html` — header HTML (line 219) and `.btn.icon` rule (line 81).
- Scope and affected surfaces: header-actions Help and Log out buttons only. The `.btn.icon` rule remains for any other icon buttons; no token or component changes.
- Uncertainty: none — the fix is deterministic (remove one class from two buttons). No backend or API impact.

## Design decision

Remove the `icon` class from the two header-actions buttons so `.header-actions button` text-button sizing governs and labels render fully. Do not alter `button.btn.icon` (kept for other icon-only buttons) and do not introduce new tokens.

## Reuse

- Existing `.header-actions button` rule (line 51) already defines correct text-button sizing; no new CSS needed.
- Existing `button.btn` rule (line 75) applies fallback text-button density if specificity ties resolve that way; either way labels are unclipped.
- `.btn.icon` rule (line 81) untouched — remains available for icon-only buttons elsewhere.

## Changes

1. `external_components/web_editor/web_ui.html`, header-actions (line 219)
   - Change: `button class="btn icon" id="help"` → `button class="btn" id="help"`
   - Change: `button class="btn icon" id="logout"` → `button class="btn" id="logout"`
   - Preserve: `id`, `title`, button order, `.header-actions` container, gap, all other header markup unchanged.
   - Verify: "Help" and "Log out" text fully visible, not clipped to a 36px square.

## Scope

- Inherit: header-actions Help and Log out buttons across all tabs (header is sticky and always present).
- Verify: header on Status/Quick Start/Schedule/Config/Diagnostics tabs; narrow viewport (line 58–62) still applies `.header-actions button` sizing.
- Exclude: any other `.btn.icon` button (unchanged), nav tabs (unchanged), footer/savebar (unchanged).

## Validation

- Product: Help and Log out labels fully visible in header; buttons size to text, not fixed square.
- Interface: light/dark themes (tokens unchanged), narrow viewport, login-gated header always present after auth.
- System: reuses existing `.header-actions button` and `button.btn` rules; no new token, component, or backend change.
- Repository: `esphome lint esphome/curling-timer.yaml` clean; host tests (`tests/host/run.ps1`) pass (UI is static HTML, no logic change).

## Stop conditions

- Stop if another surface depends on header-actions buttons being icon-only squares.
- Stop if `.header-actions button` is later removed (then text sizing would fall back to `button.btn`, still acceptable).

## Design documentation

- After acceptance and validation: none required — no new token or component; alignment of existing rule with existing markup.
