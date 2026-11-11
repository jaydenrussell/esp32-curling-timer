# Design System: Curling Timer (MASTER)

Generated with ui-ux-pro-max (`--design-system`, `--persist`).
Source of truth for the web UI redesign of `external_components/web_editor/web_ui.html`.

## Product

- **Product:** ESP32-P4 curling timer — live countdown, schedule, relay control for a curling club.
- **Product type:** Tool (hardware controller / dashboard).
- **Target audience:** Club operators using a wall-mounted touch display; commute/leisure/work context; indoor rink lighting (low light).
- **Platform:** Web (single-file embedded UI served by the device).
- **Style keywords:** dark mode, technical, precise, functional, glassmorphism, real-time.
- **Stack:** vanilla HTML/CSS/JS (no framework), served embedded.

## Pattern

- **Name:** Real-Time / Operations Landing
- **Focus:** Live status foremost; telemetry labeled live only when backed by current source, with update time + stale state; pause/hide or update-frequency controls for tickers; keyboard controls; static final snapshot under reduced motion.
- **Sections:** Hero (product + live countdown/status) > Key metrics/indicators > Schedule/leagues > Controls (start/reset/relay) > Config > Diagnostics.
- **CTA:** Primary action in nav + after metrics.

## Style

- **Name:** Glassmorphism (Dark Mode)
- **Mode support:** Light supported | Dark supported (dark is primary).
- **Keywords:** frosted glass, transparent, blurred background, layered, vibrant background, light source, depth, multi-layer.
- **Best for:** dashboards, analytics, admin panels, live-control surfaces.
- **Accessibility risk:** conditional — requires contrast-text-4.5, keyboard, visible-focus, reduced-motion.

## Tokens (CSS variables)

| Role | Hex | CSS variable | Notes |
|------|-----|--------------|-------|
| Background | `#0F172A` | `--color-background` | deep dark |
| Surface / Card | `#1B2336` | `--color-card` | frosted panel |
| Surface-2 | `#1E293B` | `--color-primary` | slightly elevated panel |
| Foreground / text | `#F8FAFC` | `--color-foreground` | body text |
| Muted | `#272F42` | `--color-muted` | secondary surfaces |
| Muted foreground | `#94A3B8` | `--color-muted-foreground` | secondary text |
| Border | `#475569` | `--color-border` | 1px separators |
| Accent / CTA | `#22C55E` | `--color-accent` | status green (live/bell) |
| On accent | `#0F172A` | `--color-on-accent` | text on green |
| Destructive | `#EF4444` | `--color-destructive` | errors/danger |
| On destructive | `#000000` | `--color-on-destructive` | |
| Ring / focus | `#FFFFFF` | `--color-ring` | focus indicator |

*Existing surface keeps cold rink/stone accent hues; this token table is the normalization target.*

## Typography

- **Heading:** Fira Code (mono) — precise, technical, tabular numbers for timers.
- **Body:** Fira Sans (sans) — functional, readable.
- **Google Fonts:**
```
@import url('https://fonts.googleapis.com/css2?family=Fira+Code:wght@400;500;600;700&family=Fira+Sans:wght@300;400;500;600;700&display=swap');
```
- **Mood:** dashboard, data, analytics, code, technical, precise.

## Icons

- **Library:** Phosphor (SVG), outline style preferred; Heroicons as fallback.
- **Rule:** No emojis as structural icons (replaces current 📊 ⚡ 📅 🏆 📈 ⚙ 🩺 nav tab emojis and 🎯 favicon).
- **Semantics:** decorative beside visible text → `aria-hidden="true"`; meaningful without text → text alternative; inside control → accessible name + state.
- **Sizing:** consistent token scale (icon-sm/md/lg); 20px default for nav/toolbar.

## Layout & spacing

- **System:** 4/8dp spacing rhythm; consistent content width; 16/24/32/48 vertical rhythm tiers by hierarchy.
- **Responsive breakpoints:** 375px, 768px, 1024px, 1440px; no horizontal scrolling; max-width content container.
- **Fixed header:** sticky top bar with safe-area clearance; scroll content not hidden behind sticky bars.
- **Glass panels:** backdrop-blur 10–20px, 1px solid rgba(white,0.2) border, z-depth layering.

## UX rules (applied)

- Live status indicators show update time + stale state.
- Submit/save shows loading → success/error (no silent button clicks).
- Required fields marked clearly.
- Focus indicator: ≥2px perimeter, 3:1 state contrast (`outline: 2px solid currentColor; outline-offset: 2px`).
- Touch targets ≥44×44pt; expanded hit area for small icons.
- Hover/pressed states 150–300ms smooth transitions; no layout-shifting transforms.
- `prefers-reduced-motion` respected (animations off, static snapshot).
- Dark-mode contrast ≥4.5:1 for body text in both themes; dividers visible in both.
- `cursor-pointer` on all clickable elements.
- Viewport meta: `width=device-width, initial-scale=1`.

## Anti-patterns to avoid

- Light mode as default.
- Emojis as icons (nav tabs, favicon).
- Slow updates / no automation.
- Horizontal scroll on mobile.
- Low-contrast muted body text.
- Layout-shifting press transforms.
- Hardcoded per-screen hex colors (use tokens).

## Affected surface

- `external_components/web_editor/web_ui.html` (single file: HTML + `<style>` + `<script>`), all tabs (Status, Quick Start, Schedule, Leagues, History, Config/Settings, Diagnostics) + login + header + nav + savebars.

## Open decisions for executor

- Preserve existing cold rink/stone-red/stone-gold accent hues or fully adopt the green-accent token table above? (Recommendation: keep rink/stone as brand accents, map to token roles.)
- Replace Georgia serif display countdown with Fira Code mono? (Recommendation: keep Georgia for the hero countdown per existing identity; use Fira Sans/Code for UI chrome.)
- Nav tabs: emoji → Phosphor SVG icons? (Recommendation: yes, per "No emojis as icons" rule.)

## Status

- Design intelligence gathered; implementation not yet started. Executor should confirm the open decisions before rewriting `web_ui.html`.
