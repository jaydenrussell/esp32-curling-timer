// AUTO-GENERATED from web_ui.html by web_editor __init__.py at codegen time — edit web_ui.html, not this file.
#pragma once

static const char WEB_UI_HTML[] = R"esphome_web_ui(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover, interactive-widget=resizes-content">
<title>Curling Timer</title>
<meta name="theme-color" content="#07182a">
<meta name="color-scheme" content="dark">
<meta name="csrf-token" content="">
<link rel="manifest" href="data:application/json;base64,eyJuYW1lIjoiQ3VybGluZyBUaW1lciIsInNob3J0X25hbWUiOiJDdXJsaW5nIFRpbWVyIiwic3RhcnRfdXJsIjoiLyIsImRpc3BsYXkiOiJzdGFuZGFsb25lIiwiYmFja2dyb3VuZF9jb2xvciI6IiMwQjExMjAifQ==">
<link rel="icon" href="data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 100 100'%3E%3Crect width='100' height='100' rx='22' fill='%236750A4'/%3E%3Cpath d='M25 35 L45 55 L75 25' stroke='%23FFFFFF' stroke-width='8' fill='none' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E">
<style>
:root {
  color-scheme: dark;
  --bg: #07182a;
  --bg-2: #0b2540;
  --surface: rgba(255,255,255,0.07);
  --surface-strong: rgba(255,255,255,0.11);
  --surface-soft: rgba(255,255,255,0.04);
  --border: rgba(255,255,255,0.14);
  --border-strong: rgba(255,255,255,0.22);
  --text: #eaf4ff;
  --text-strong: #ffffff;
  --muted: #9fb7cf;
  --text-dim: #7e98b3;
  --primary: #14b8a6;
  --primary-2: #22d3ee;
  --accent: #3b82f6;
  --primary-soft: rgba(20,184,166,0.16);
  --accent-soft: rgba(59,130,246,0.16);
  --ok: #34d399;
  --ok-dark: #065f46;
  --bad: #f87171;
  --warn: #fbbf24;
  --shadow: 0 18px 45px rgba(2, 10, 20, 0.45);
  --shadow-lg: 0 24px 60px rgba(2, 10, 20, 0.55);
  --radius-lg: 22px;
  --radius-md: 16px;
  --radius-sm: 12px;
}
* { box-sizing: border-box; }
html { -webkit-tap-highlight-color: transparent; -webkit-text-size-adjust: 100%; }
body {
  margin: 0;
  color: var(--text);
  font: 15px/1.55 Inter, system-ui, -apple-system, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  background:
    radial-gradient(900px 500px at 10% -10%, rgba(20,184,166,0.18), transparent 60%),
    radial-gradient(800px 420px at 95% 0%, rgba(59,130,246,0.22), transparent 55%),
    radial-gradient(700px 380px at 50% 120%, rgba(34,211,238,0.10), transparent 60%),
    linear-gradient(180deg, var(--bg-2) 0%, var(--bg) 100%);
  min-height: 100vh;
  overflow-x: hidden;
}
h1,h2,h3 { margin: 0 0 8px; line-height: 1.2; letter-spacing: -0.01em; color: var(--text-strong); }
h1 { font-size: 22px; font-weight: 700; }
h2 { font-size: 13px; font-weight: 700; letter-spacing: 0.08em; text-transform: uppercase; color: var(--muted); }
h3 { font-size: 17px; font-weight: 600; }
p { margin: 0 0 12px; color: var(--text); }
a { color: var(--primary-2); text-decoration: none; }
a:hover { text-decoration: underline; }
button, a, [role="button"] { touch-action: manipulation; cursor: pointer; -webkit-tap-highlight-color: transparent; }
button:active { transform: scale(0.985); }
.hidden { display: none !important; }
.spacer { flex: 1; }
.muted { color: var(--muted); }
.small { font-size: 12px; color: var(--text-dim); }

.glass, .card, .hero, .modal-card, .savebar, .addon-card, .entry, .league-list, details.fold, .today, .backups, .league-row, pre.view, #login form {
  background: var(--surface);
  border: 1px solid var(--border);
  backdrop-filter: blur(16px) saturate(140%);
  -webkit-backdrop-filter: blur(16px) saturate(140%);
  box-shadow: var(--shadow);
}

header {
  display: flex; align-items: center; gap: 10px;
  padding: 10px 12px; min-height: 56px;
  position: sticky; top: 0; z-index: 120;
  background: rgba(7, 24, 42, 0.55);
  border-bottom: 1px solid rgba(255,255,255,0.08);
  backdrop-filter: blur(12px) saturate(140%);
  -webkit-backdrop-filter: blur(12px) saturate(140%);
}
.header-brand { display: flex; flex-direction: column; align-items: flex-start; gap: 0; flex-shrink: 0; }
.header-brand h1 { font-size: 20px; margin: 0; line-height: 1.2; }
.header-version { font-size: 11px; color: var(--text-dim); padding: 2px 8px; border-radius: 999px; background: rgba(255,255,255,0.06); border: 1px solid var(--border); }
#header-msg { font-size: 13px; padding: 8px 12px; border-radius: 12px; white-space: pre-wrap; flex: 1; margin: 0; }
.header-actions { display: flex; gap: 8px; margin-left: auto; }

/* Hamburger menu — visible on mobile, hidden on desktop */
.header-hamburger {
  display: none;
  font-size: 22px;
  cursor: pointer;
  color: white;
  padding: 0 8px;
  position: absolute;
  left: 12px;
  top: 50%;
  transform: translateY(-50%);
  z-index: 121;
}
/* Primary navigation — bottom bar on all viewports */
.bottom-nav {
  position: fixed; left: 10px; right: 10px; bottom: 10px;
  display: flex; flex-direction: row; gap: 4px; z-index: 130;
  border-radius: 18px; padding: 6px;
  background: rgba(7, 24, 42, 0.85);
  border: 1px solid var(--border);
  backdrop-filter: blur(18px) saturate(150%);
  -webkit-backdrop-filter: blur(18px) saturate(150%);
  box-shadow: var(--shadow-lg);
}
.bottom-nav button {
  flex: 1; border: 1px solid transparent; background: transparent; color: var(--muted);
  border-radius: 14px; padding: 8px 4px; min-height: 54px;
  display: flex; flex-direction: column; align-items: center; justify-content: center; gap: 2px;
  font-size: 11px; font-weight: 600;
  transition: all .15s ease;
}
.bottom-nav button:hover { background: rgba(255,255,255,0.06); color: var(--text); }
.bottom-nav button.active { background: var(--primary-soft); color: var(--text-strong); border-color: rgba(34,211,238,0.35); }
.bottom-nav .icon { font-size: 20px; line-height: 1; }

/* Top tabs hidden on all viewports - navigation lives in bottom-nav */
nav#tabs { display: none !important; }
@media (max-width: 768px) {
  .header-hamburger { display: block; }
  nav#tabs { display: none !important; }
  .bottom-nav { 
    flex-direction: row; 
  }
  .header-actions { 
    display: none;
  }
}

/* Consistent padding for bottom nav on all viewports */
main {
  max-width: 1120px; margin: 0 auto; padding: 10px 12px 86px;
  display: flex; flex-direction: column; gap: 10px;
}
@media (max-width: 640px) { main { padding: 8px 8px 86px; } }
@media (max-width: 480px) {
  .bottom-nav button .label { display: none; }
  .bottom-nav button { min-height: 48px; }
}

/* Scheduling sub-tabs — shown when tab === 'schedule' */
.sub-tabs {
  display: none;
  position: sticky;
  top: 56px;
  z-index: 125;
  margin: 0 10px;
  border-radius: 0 0 14px 14px;
  padding: 4px 6px;
  background: rgba(7, 24, 42, 0.85);
  border: 1px solid var(--border);
  border-top: 0;
  backdrop-filter: blur(12px) saturate(140%);
  -webkit-backdrop-filter: blur(12px) saturate(140%);
  box-shadow: var(--shadow);
}
.sub-tabs.visible {
  display: flex;
  flex-direction: row;
  gap: 2px;
}
.sub-tabs button {
  flex: 1; border-radius: 12px; padding: 6px 12px; min-height: 32px;
  border: 1px solid transparent; background: transparent; color: var(--muted);
  font-size: 11px; font-weight: 600; cursor: pointer;
  transition: all .15s ease;
  white-space: nowrap;
}
.sub-tabs button:hover { background: rgba(255,255,255,0.06); color: var(--text); }
.sub-tabs button.active { background: var(--primary-soft); color: var(--text-strong); border-color: rgba(34,211,238,0.35); }
.sub-tabs button:focus-visible { outline: 2px solid var(--primary); outline-offset: 2px; }

@media (max-width: 480px) {
  .sub-tabs button { padding: 4px 8px; font-size: 10px; }
}

.card, .hero, .addon-card {
  border-radius: var(--radius-lg); padding: 16px; margin: 0 0 10px;
  display: flex; flex-direction: column; gap: 10px;
}
.card > h2 { margin: 0 0 4px; }
.card h3 { margin: 0 0 6px; }
.hero { padding: 22px; }

button.btn, .btn-toggle {
  font: inherit; border: 1px solid var(--border); background: rgba(255,255,255,0.05);
  color: var(--text); border-radius: 12px; padding: 10px 16px; min-height: 44px;
  transition: all .15s ease; font-size: 14px; font-weight: 600;
  display: inline-flex; align-items: center; justify-content: center; gap: 8px;
}
button.btn:hover:not(:disabled), .btn-toggle:hover:not(:disabled) { background: rgba(255,255,255,0.1); border-color: var(--border-strong); }
button.btn:disabled, .btn-toggle:disabled { opacity: 0.5; cursor: default; }
button.btn.primary, .btn-toggle.primary {
  background: linear-gradient(135deg, var(--primary), var(--primary-2));
  color: #042a2a; border-color: rgba(34,211,238,0.45);
  box-shadow: 0 10px 24px rgba(20,184,166,0.22);
}
button.btn.primary:hover:not(:disabled), .btn-toggle.primary:hover:not(:disabled) { filter: brightness(1.05); }
button.btn.danger, .btn-toggle.danger { color: #fecaca; border-color: rgba(248,113,113,0.45); background: rgba(248,113,113,0.08); }
button.btn.icon { padding: 0; width: 42px; height: 42px; min-height: 42px; border-radius: 12px; }
button.btn.icon svg { width: 18px; height: 18px; fill: currentColor; }
button.btn.preset { font-size: 13px; padding: 8px 12px; min-height: 38px; }

/* Modern check-toggle pills: green filled ON with label text; red outline OFF.
   Deliberately NOT the same gradient or sizing as action buttons so they
   read as settings toggles, not as "Start game" / "Save" buttons. */
.check-toggle {
  min-width: 72px; height: 30px; min-height: 0; min-width: 0;
  border-radius: 999px; padding: 0 12px;
  border: 1px solid var(--border);
  background: rgba(255,255,255,0.05);
  color: var(--text); font-size: 12px; font-weight: 700;
  display: inline-flex; align-items: center; justify-content: center;
  transition: all .15s ease; cursor: pointer; flex-shrink: 0;
  white-space: nowrap; overflow: hidden; text-overflow: ellipsis;
}
.check-toggle.on {
  background: linear-gradient(135deg, #22c55e, #16a34a);
  color: var(--text); border-color: rgba(34,197,94,0.5);
  box-shadow: 0 4px 12px rgba(34,197,94,0.25);
}
.check-toggle.off {
  background: transparent;
  color: var(--text); border-color: rgba(239,68,68,0.55);
  box-shadow: none;
}
.check-toggle:hover:not(:disabled) { filter: brightness(1.1); }
.check-toggle:disabled { opacity: 0.4; cursor: default; }

input, select, textarea {
  font: inherit; color: var(--text); background: rgba(0,0,0,0.2);
  border: 1px solid var(--border); border-radius: 12px; padding: 10px 13px;
  min-height: 44px; min-width: 0; font-size: 14px;
  transition: border-color .15s ease, box-shadow .15s ease, background .15s ease;
}
@media (pointer: coarse) { input, select, textarea { font-size: 16px; } }
input[type="number"]::-webkit-outer-spin-button, input[type="number"]::-webkit-inner-spin-button { -webkit-appearance: none; margin: 0; }
input[type="number"] { -moz-appearance: textfield; }
input:focus, select:focus, textarea:focus {
  outline: none; border-color: rgba(34,211,238,0.55);
  box-shadow: 0 0 0 3px rgba(20,184,166,0.18); background: rgba(0,0,0,0.3);
}
/* Dark dropdown: select needs explicit dark background + light text so the
   browser menu matches the rest of the dark theme. */
select {
  appearance: none; -webkit-appearance: none;
  background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='10' height='6' viewBox='0 0 10 6'%3E%3Cpath d='M1 1l4 4 4-4' stroke='%239fb7cf' stroke-width='1.5' fill='none' stroke-linecap='round' stroke-linejoin='round'/%3E%3C/svg%3E");
  background-repeat: no-repeat; background-position: right 12px center;
  padding-right: 32px; cursor: pointer;
}
select option { background: #0b1a2e; color: var(--text); }
textarea { width: 100%; min-height: 60vh; font: 13px/1.6 ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; white-space: pre; }
label { display: block; font-size: 12px; color: var(--muted); margin: 0 0 6px; font-weight: 600; letter-spacing: 0.03em; }
label.check { display: inline-flex; align-items: center; gap: 8px; margin: 0; font-size: 14px; color: var(--text); font-weight: 500; }
label.check input { width: 18px; height: 18px; min-height: 0; margin: 0; accent-color: var(--primary); }
.field { margin: 0 0 10px; }
.row-actions { display: flex; flex-wrap: wrap; gap: 10px; align-items: center; margin-top: 6px; }
.duration { display: flex; align-items: center; gap: 8px; }
.duration input { width: 8ch; min-width: 76px; }
/* Every numeric field gets a visible +/- control (native spinners are hidden
   above). Keep the value box wide enough for large minute counts. */
.number-stepper { display: inline-flex; align-items: center; gap: 4px; max-width: 100%; }
.number-stepper input { width: 8ch; min-width: 76px; text-align: right; }
.step-btn {
  width: 32px; min-width: 32px; height: 32px; padding: 0;
  display: inline-flex; align-items: center; justify-content: center;
  border-radius: 8px; border: 1px solid var(--border);
  background: rgba(34,211,238,0.10); color: var(--text-strong);
  font-size: 16px; line-height: 1; cursor: pointer;
}
.step-btn:hover:not(:disabled) { background: rgba(34,211,238,0.22); border-color: rgba(34,211,238,0.55); }
.step-btn:disabled { opacity: 0.35; cursor: default; }

.pill {
  display: inline-flex; align-items: center; gap: 6px; font-size: 11px; font-weight: 700;
  padding: 3px 10px; border-radius: 999px; border: 1px solid var(--border);
  letter-spacing: 0.04em; text-transform: uppercase; color: var(--muted); background: rgba(255,255,255,0.04);
}
.pill.ok { color: #a7f3d0; border-color: rgba(52,211,153,0.4); background: rgba(52,211,153,0.12); }
.pill.bad { color: #fecaca; border-color: rgba(248,113,113,0.4); background: rgba(248,113,113,0.12); }
.pill.warn { color: #fde68a; border-color: rgba(251,191,36,0.4); background: rgba(251,191,36,0.12); }
.pill.quick { color: #99f6e4; border-color: rgba(34,211,238,0.4); background: rgba(20,184,166,0.14); }
.pill.detected { color: #bfdbfe; border-color: rgba(59,130,246,0.4); background: rgba(59,130,246,0.14); }
.pill.enabled { color: #a7f3d0; border-color: rgba(52,211,153,0.4); background: rgba(52,211,153,0.12); }
.pill.notdetected { color: var(--muted); border-color: var(--border); background: rgba(255,255,255,0.03); }
.pill.manual { color: #ddd6fe; border-color: rgba(139,92,246,0.4); background: rgba(139,92,246,0.14); }
.banner { border: 1px solid var(--border); border-radius: 14px; padding: 10px 14px; font-size: 13px; line-height: 1.5; margin: 0 0 10px; background: rgba(255,255,255,0.04); }
.banner.bad { background: rgba(248,113,113,0.12); border-color: rgba(248,113,113,0.35); color: #fecaca; }
.banner.ok { background: rgba(52,211,153,0.12); border-color: rgba(52,211,153,0.35); color: #a7f3d0; }
.banner.warn { background: rgba(251,191,36,0.12); border-color: rgba(251,191,36,0.35); color: #fde68a; }
.state { font-weight: 700; letter-spacing: 0.04em; font-size: 13px; }

.grid2 { display: flex; flex-wrap: wrap; gap: 12px; align-items: flex-start; }
.grid3 { display: flex; flex-wrap: wrap; gap: 12px; align-items: flex-start; }
.page-head { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 2px; }
.page-head h1, .page-head h2 { margin: 0; }
.section-title { font-size: 12px; font-weight: 700; letter-spacing: 0.08em; text-transform: uppercase; color: var(--muted); margin: 2px 0 1px; }
.stack { display: flex; flex-direction: column; gap: 12px; }
.split { display: grid; grid-template-columns: 1.2fr 0.8fr; gap: 12px; }
@media (max-width: 900px) { .split { grid-template-columns: 1fr; } }
@media (max-width: 640px) { .grid2 { grid-template-columns: 1fr; } }

.countdown { font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-size: clamp(48px, 15vw, 72px); font-weight: 800; color: var(--text-strong); margin: 8px 0; letter-spacing: -0.02em; }
/* Pre-game countdown colon - simple default colon */
#mirror-countdown .cd-colon { display: inline-block; }
.game-name { font-size: 20px; font-weight: 700; color: var(--text-strong); }
#status-now { display: grid; gap: 14px; }
.status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); gap: 14px; align-items: start; }
.display-mirror, #display-mirror { border: 1px solid rgba(255,255,255,0.10); box-shadow: 0 20px 50px rgba(0,0,0,0.5); }
.info-row { display: flex; flex-wrap: wrap; gap: 8px; align-items: center; }
.kv { display: grid; gap: 2px; }
.kv .k { font-size: 11px; text-transform: uppercase; letter-spacing: 0.06em; color: var(--text-dim); }
.kv .v { font-weight: 600; color: var(--text-strong); }

.today { list-style: none; margin: 0; padding: 0; border-radius: var(--radius-md); overflow: hidden; }
.today li { display: grid; grid-template-columns: max-content max-content minmax(0, 1fr); column-gap: 12px; align-items: center; padding: 12px 14px; border-bottom: 1px solid rgba(255,255,255,0.08); font-size: 13px; font-variant-numeric: tabular-nums; }
.today li:last-child { border-bottom: 0; }
.today li.head { background: rgba(255,255,255,0.04); color: var(--muted); font-weight: 700; font-size: 11px; letter-spacing: 0.07em; text-transform: uppercase; }
.today li.now { background: var(--primary-soft); border-left: 3px solid var(--primary); }
.today li.past { color: var(--text-dim); opacity: 0.75; }
.today .name { overflow-wrap: anywhere; }
.league-list { border-radius: var(--radius-md); overflow: hidden; }
.league-row { display: grid; grid-template-columns: 28px 1fr auto auto auto; gap: 10px; align-items: center; padding: 10px 14px; border-bottom: 1px solid rgba(255,255,255,0.08); font-size: 13px; background: transparent; box-shadow: none; }
.league-row:last-child { border-bottom: 0; }
.entry {
  display: grid; grid-template-columns: 1fr 1fr 1fr auto; gap: 10px;
  padding: 14px; border-radius: var(--radius-md); margin-bottom: 10px; align-items: start;
}
.entry .league { grid-column: 1; }
.entry .start { grid-column: 2; }
.entry .buzzer { grid-column: 3; }
.entry .actions { grid-column: 4; display: flex; flex-direction: column; gap: 6px; }
.entry .name { grid-column: 1 / -1; display: flex; gap: 8px; align-items: center; margin-top: 8px; padding-top: 10px; border-top: 1px solid rgba(255,255,255,0.08); }
.entry .name input { flex: 1; }
.entry .more { grid-column: 1 / -1; display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; margin-top: 8px; padding-top: 10px; border-top: 1px solid rgba(255,255,255,0.08); }
.entry label { font-size: 11px; color: var(--text-dim); text-transform: uppercase; letter-spacing: 0.05em; margin-bottom: 4px; }

details.fold { padding: 0; margin: 0 0 12px; border-radius: var(--radius-md); overflow: hidden; }
details.fold > summary { display: flex; align-items: center; gap: 10px; padding: 14px 16px; cursor: pointer; list-style: none; background: rgba(255,255,255,0.04); font-weight: 700; font-size: 14px; color: var(--text-strong); }
details.fold > summary::-webkit-details-marker { display: none; }
details.fold .chevron { display: inline-block; width: 1em; transition: transform .15s; color: var(--muted); }
details.fold[open] .chevron { transform: rotate(90deg); }
details.fold .fold-note { flex: 1; min-width: 0; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; font-size: 13px; color: var(--muted); font-weight: 500; }
details.fold .fold-count, details.fold .fold-date { font-size: 12px; color: var(--muted); white-space: nowrap; }
details.fold .fold-body { padding: 12px 16px 16px; background: transparent; }
.savebar {
  position: fixed; left: 12px; right: 12px; bottom: 12px; border-radius: 16px;
  padding: 10px 14px; display: none; gap: 10px; align-items: center; justify-content: center; flex-wrap: wrap; z-index: 140;
  background: rgba(7,24,42,0.75);
}
.savebar.show { display: flex; }
.savebar span { color: var(--muted); font-weight: 600; font-size: 13px; }
.modal { position: fixed; inset: 0; background: rgba(2,10,20,0.55); display: none; align-items: center; justify-content: center; z-index: 300; padding: 16px; }
.modal.show { display: flex; }
.modal-backdrop { position: absolute; inset: 0; }
.modal-card { position: relative; border-radius: 22px; padding: 22px; max-width: 92vw; max-height: 90vh; overflow-y: auto; }
.modal-card h2 { color: var(--text-strong); font-size: 20px; text-transform: none; letter-spacing: -0.01em; }
.modal-card ul { margin: 8px 0 16px; padding-left: 20px; color: var(--muted); }
#login { position: fixed; inset: 0; display: none; align-items: center; justify-content: center; z-index: 400; padding: 24px; background: radial-gradient(800px 400px at 20% 0%, rgba(20,184,166,0.18), transparent 60%), linear-gradient(180deg, var(--bg-2), var(--bg)); }
#login.show { display: flex; }
#login form { width: 100%; max-width: 360px; border-radius: 22px; padding: 24px; }
#login h2 { font-size: 24px; text-transform: none; letter-spacing: -0.01em; color: var(--text-strong); }
#login p { color: var(--muted); }
#login input { width: 100%; margin: 0 0 12px; }
pre.view { overflow: auto; max-height: 55vh; border-radius: 14px; padding: 12px; font-size: 12px; font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; background: rgba(0,0,0,0.25); }
.backups { list-style: none; padding: 0; margin: 0; border-radius: var(--radius-md); overflow: hidden; }
.backups li { display: flex; gap: 10px; align-items: center; padding: 12px 14px; border-bottom: 1px solid rgba(255,255,255,0.08); flex-wrap: wrap; font-size: 13px; }
.backups li:last-child { border-bottom: 0; }
.backups li span { flex: 1; min-width: 200px; color: var(--muted); }

/* History page — accordion per-row expand */
.history-container { margin: 0; }
.backup-row {
  background: var(--surface);
  border: 1px solid var(--border);
  border-radius: var(--radius-md);
  margin: 0 0 10px;
  overflow: hidden;
  transition: border-color .15s ease;
}
.backup-row[open] { border-color: var(--primary-2); }
.backup-row summary {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 14px;
  font-size: 13px;
  cursor: pointer;
  list-style: none;
}
.backup-row summary::-webkit-details-marker { display: none; }
.backup-row summary::marker { display: none; }
.summary-date { color: var(--muted); white-space: nowrap; overflow: hidden; text-overflow: ellipsis; flex: 1; min-width: 150px; }
.summary-actions { display: flex; gap: 8px; margin-left: 12px; flex-shrink: 0; }
.backup-row .btn { min-height: 38px; padding: 6px 12px; font-size: 12px; white-space: nowrap; }
.backup-row .content { padding: 0 14px 12px; background: rgba(0,0,0,0.2); }
.backup-row[open] .content { padding-top: 4px; }
pre.view { overflow: auto; max-height: 55vh; border-radius: 14px; padding: 12px; font-size: 12px; font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; background: rgba(0,0,0,0.25); }
.backup-row pre.view { border-radius: 0 0 var(--radius-md) var(--radius-md); margin: 0; }

.addon-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 14px; margin: 10px 0; }
.addon-card h3 { font-size: 16px; }
.addon-desc { color: var(--muted); font-size: 13px; line-height: 1.5; margin: 0; }

.progress-end-bar { position: relative; height: 105px; border-radius: 24px; overflow: hidden; background: #fff; border: 8px solid #000; width: 100%; box-sizing: border-box; }
.progress-band { position: absolute; top: 0; height: 100%; }
.progress-band.lead { background: #DC3545; } .progress-band.second { background: #FFC107; } .progress-band.vice { background: #0DCaf0; } .progress-band.skip { background: #198754; }
.progress-divider { position: absolute; top: 0; height: 100%; background: #000; }
.version-dot { display:inline-block; width:8px; height:8px; border-radius:50%; background: var(--warn); margin-left:6px; vertical-align:middle; cursor:pointer; }

@media (prefers-reduced-motion: reduce) { * { transition: none !important; animation: none !important; } }
</style>
</head>
<body>
<div id="login"><form class="card" id="login-form">
  <h2>Curling Timer <span class="version">v2026.11.247</span></h2>
  <p>ESP32-P4 curling timer — live countdown, schedule, and relay control for your club.</p>
  <label for="username">Username</label>
  <input id="username" type="text" autocomplete="username" placeholder="Username">
  <label for="password">Password</label>
  <input id="password" type="password" autocomplete="current-password" required>
  <input id="login-username-hidden" type="hidden" value="">
  <div id="login-error" class="banner bad hidden"></div>
  <button class="btn primary" type="submit">Log in</button>
</form></div>

<div id="help-modal" class="modal hidden">
  <div class="modal-backdrop" onclick="const m=document.getElementById('help-modal'); m.classList.add('hidden'); m.classList.remove('show');"></div>
  <div class="modal-card card">
    <h2>Help</h2>
    <p>ESP32-P4 Curling Timer - countdown, schedule, and relay control for curling games.</p>
    <ul>
      <li><strong>Status:</strong> live game countdown and bell state.</li>
      <li><strong>Quick Start:</strong> start a one-off game immediately.</li>
      <li><strong>Schedule:</strong> weekly game schedule with date exceptions.</li>
      <li><strong>Settings:</strong> relay durations, buzzer hold, web UI options.</li>
      <li><strong>Diagnostics:</strong> device info, network, logs.</li>
    </ul>
    <button class="btn" onclick="const m=document.getElementById('help-modal'); m.classList.add('hidden'); m.classList.remove('show');">Close</button>
  </div>
</div>

<header>
  <div class="header-brand">
    <h1>⏱️ Curling Timer</h1>
    <span class="header-version">v2026.11.247</span>
  </div>
  <span id="header-msg" class="banner hidden"></span>
  <div class="header-actions">
    <button class="btn" id="help" title="Help">Help</button>
    <button class="btn" id="logout" title="Log out">Log out</button>
  </div>
</header>
<nav id="tabs" aria-label="Primary"></nav>
<nav class="bottom-nav" id="bottom-nav" aria-label="Mobile"></nav>
<nav class="sub-tabs" id="sub-tabs" aria-label="Scheduling sub-navigation">
  <button data-subtab="schedule" class="active">📅 Schedule</button>
  <button data-subtab="leagues">🏆 Leagues</button>
</nav>
<main>
  <section id="tab-status"></section>
  <section id="tab-quickstart" class="hidden"></section>
  <section id="tab-schedule" class="hidden"></section>
  <section id="tab-exceptions" class="hidden"></section>
  <section id="tab-raw" class="hidden"></section>
  <section id="tab-config" class="hidden"></section>
  <section id="tab-diagnostics" class="hidden"></section>
  <section id="tab-leagues" class="hidden"></section>
</main>
<div class="savebar" id="savebar"><span>Unsaved schedule changes</span>
  <button class="btn" id="discard">Discard</button><button class="btn primary" id="save">Save &amp; apply</button></div>
<div class="savebar" id="config-savebar"><span>Unsaved config changes</span>
  <button class="btn" id="config-discard">Discard</button><button class="btn primary" id="config-save">Save &amp; apply</button></div>

<script>
'use strict';
const DAYS = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'];
const NAV_LABELS = {
  status: 'Live',
  quickstart: 'Quick',
  schedule: 'Scheduling',
  config: 'Settings',
  diagnostics: 'Diagnostics'
};
const NAV_ICONS = {
  status: '⏱️',
  quickstart: '⚡',
  schedule: '📅',
  config: '⚙️',
  diagnostics: '🔧'
};
const TABS = [
  ['status', '⏱️', 'Live'],
  ['quickstart', '⚡', 'Quick'],
  ['schedule', '📅', 'Scheduling'],
  ['config', '⚙️', 'Settings'],
  ['diagnostics', '🔧', 'Diagnostics']
];
const state = { tab: 'status', subTab: 'schedule', status: null, statusAt: 0, model: null, dirty: false, showPast: false, openDays: new Set(), openDates: new Set(), raw: null,
                configOriginal: null, configValues: null, configVersion: null, configDirty: false, editingLeagueIdx: null, leagues: null };

// HDMI progress geometry: 1224px inner track, fixed band x/width pairs and
// 4px divider x positions. Shared by the mirror builder and the 1 Hz fill.
const PROGRESS_TRACK_PX = 1224;
const PROGRESS_BAND_GEOM = [[0, 282], [282, 282], [564, 282], [846, 378]];
const PROGRESS_DIV_X = [278, 560, 842];

// Add-on marketplace data model (spec §2). auto = hardware-detected gating.
// configPaths = settings fields that belong to this addon (gated by enablement).
// info = GPIO / I2C / port / hat details shown in the expansion.
const ADDONS = [
  { key: 'rtc', label: 'RTC time source', desc: 'Battery-backed clock (HYM8563 / PCF8563) — keeps time across power loss; timer knows the time on boot before SNTP syncs.', auto: true,
    configPaths: ['config.addons.rtc'],
    info: [ 'I2C: address 0x51 (HYM8563 / PCF8563)', 'Default build bus: SDA GPIO5, SCL GPIO6 (separate from GPIO relay pins)', 'Addon/example wiring may use Grove Port B (GPIO53/GPIO54); do not share those pins with GPIO relay outputs', 'No address conflict with common I2C relay modules (typically 0x25/0x26), but I2C relay modules are not supported by this firmware' ] },
  { key: 'relays', label: 'Relay outputs', desc: 'GPIO relay outputs driving the bell circuit + aux (Unit 2Relay / direct GPIO).', auto: false,
    configPaths: ['config.addons.relays', 'config.relay_count', 'config.relay1_enabled', 'config.relay1_function', 'config.relay2_enabled', 'config.relay2_function', 'config.relay3_enabled', 'config.relay3_function', 'config.relay4_enabled', 'config.relay4_function', 'config.buzzer.ring_at_start', 'config.buzzer.countdown.relays', 'config.buzzer.relays'],
    info: [ 'Current GPIO pins (main config): RELAY1 GPIO53, RELAY2 GPIO54', 'Grove Port B HY2.0-4P wiring: G53/G54 — do not use same pins for I2C RTC/add-ons at the same time', 'Each selected output now pulses for its own configured/test duration', 'I2C addressable 2-relay modules (e.g. 0x25/0x26) are not supported by this firmware' ] },
  { key: 'pushbutton', label: 'Manual trigger button', desc: 'GPIO press rings the start relays for the next manually-triggered game.', auto: false,
    configPaths: ['config.addons.pushbutton', 'config.manual_trigger_gpio'],
    info: [ 'Pin: GPIO45 (manual trigger input)', 'Setting: Manual trigger GPIO under Global Settings → Timer' ] },
  { key: 'pahub', label: 'PaHub I2C hub', desc: 'PCA9548A 6-channel multiplexer — multiple I2C devices on one Grove port.', auto: true,
    configPaths: ['config.addons.pahub'],
    info: [ 'I2C: address 0x70 (PCA9548A)', 'Port: Grove Port A — SDA GPIO21, SCL GPIO22', 'Channels: 0–5 (one per I2C device)', 'RTC address: 0x51 (PCF8563 / HYM8563)' ] },
  { key: 'wifi', label: 'Wi-Fi failover', desc: 'ESP32-C6 SDIO Wi-Fi failover.', auto: true,
    configPaths: ['config.addons.wifi'],
    info: [ 'Module: ESP32-C6 SuperMini on SDIO header', 'Reference: docs/flash-c6.md' ] },
  { key: 'uart', label: 'UART0 serial port', desc: 'Hat2-Bus UART0 serial (GPIO37 TX / GPIO38 RX) — one-line status stream to a serial device.', auto: false,
    configPaths: ['config.addons.uart'],
    info: [ 'Pins: TX GPIO37, RX GPIO38 (INPUT_PULLUP)', 'Hat: Hat2-Bus UART0' ] },
];
let addonDetection = {};

// Map config path → addon key, for gating config fields by addon enablement.
const PATH_TO_ADDON = {};
for (const a of ADDONS) {
  for (const path of a.configPaths) {
    PATH_TO_ADDON[path] = a.key;
  }
}
// The config.addons.* toggle fields themselves are never gated (they are the
// enable switch), so remove them from gating consideration.
for (const key of ['rtc', 'relays', 'pushbutton', 'pahub', 'wifi', 'uart']) {
  delete PATH_TO_ADDON[`config.addons.${key}`];
}

function isAddonEnabled(key) {
  const a = ADDONS.find((x) => x.key === key);
  if (!a) return false;
  const intent = state.configValues && state.configValues[`config.addons.${key}`] === true;
  const detected = addonDetection && addonDetection[key] === true;
  return intent && (!a.auto || detected);
}

// ---- small helpers --------------------------------------------------------
function el(tag, props, ...children) {
  const node = document.createElement(tag);
  for (const [key, value] of Object.entries(props || {})) {
    if (key === 'class') node.className = value;
    else if (key.startsWith('on')) node.addEventListener(key.slice(2), value);
    else if (key === 'value') node.value = value;
    else if (key in node && typeof value !== 'string') node[key] = value;
    else if (value !== false && value != null) node.setAttribute(key, value === true ? '' : value);
  }
  // Numeric inputs are always wrapped so the +/- buttons are visible on
  // desktop and touch alike. Callers still receive the raw input so their
  // existing event handlers and .value reads keep working.
  if (tag === 'input' && props && props.type === 'number') node._stepWrap = makeStepper(node, props);
  for (const child of children.flat()) {
    if (child != null && child !== false) {
      const insert = child && child._stepWrap ? child._stepWrap : child;
      node.append(insert instanceof Node ? insert : String(insert));
    }
  }
  return node;
}
function makeStepper(input, props) {
  const wrap = document.createElement('div');
  wrap.className = 'number-stepper';
  const label = props['aria-label'] || props.placeholder || 'value';
  const dec = document.createElement('button');
  dec.type = 'button'; dec.className = 'step-btn'; dec.textContent = '−';
  dec.setAttribute('aria-label', 'Decrease ' + label);
  const inc = document.createElement('button');
  inc.type = 'button'; inc.className = 'step-btn'; inc.textContent = '+';
  inc.setAttribute('aria-label', 'Increase ' + label);
  const stepSize = () => {
    const s = parseFloat(input.step || props.step);
    return Number.isFinite(s) && s > 0 ? s : 1;
  };
  const decimals = () => {
    const s = String(input.step || '1');
    const dot = s.indexOf('.');
    return dot < 0 ? 0 : Math.min(6, s.length - dot - 1);
  };
  const limit = (v) => {
    const min = parseFloat(input.min), max = parseFloat(input.max);
    if (Number.isFinite(min)) v = Math.max(min, v);
    if (Number.isFinite(max)) v = Math.min(max, v);
    return v;
  };
  const updateButtons = () => {
    const v = parseFloat(input.value);
    const min = parseFloat(input.min), max = parseFloat(input.max);
    dec.disabled = Number.isFinite(v) && Number.isFinite(min) && v <= min;
    inc.disabled = Number.isFinite(v) && Number.isFinite(max) && v >= max;
  };
  const nudge = (dir) => {
    const current = parseFloat(input.value);
    const base = Number.isFinite(current) ? current : (parseFloat(input.min) || 0);
    const next = limit(Number((base + dir * stepSize()).toFixed(decimals())));
    input.value = String(next);
    input.dispatchEvent(new Event('input', { bubbles: true }));
    input.dispatchEvent(new Event('change', { bubbles: true }));
    updateButtons();
  };
  dec.onclick = () => nudge(-1);
  inc.onclick = () => nudge(1);
  input.addEventListener('input', updateButtons);
  input.addEventListener('change', updateButtons);
  input.addEventListener('blur', updateButtons);
  wrap.append(dec, input, inc);
  updateButtons();
  return wrap;
}
const $ = (id) => document.getElementById(id);
function toggleBtn(on, label) {
  const btn = el('button', { class: 'check-toggle ' + (on ? 'on' : 'off'), type: 'button', 'aria-pressed': String(on), 'aria-label': label }, label);
  btn._on = !!on;
  btn._label = label;
  return btn;
}
const pad = (n) => String(n).padStart(2, '0');
function hms(seconds) {
  const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60;
  return `${pad(h)}:${pad(m)}:${pad(s)}`;
}
function shortTime(seconds) { return hms(seconds).replace(/:00$/, '').replace(/^0/, ''); }
function parseTime(value) {
  const match = /^(\d{1,2}):(\d{2})(?::(\d{2}))?$/.exec(value || '');
  if (!match) return null;
  const secs = (+match[1]) * 3600 + (+match[2]) * 60 + (+(match[3] || 0));
  return secs < 86400 ? secs : null;
}
// Game/countdown/hold durations are minutes-only in the UI; relay output
// durations are entered in seconds. Backend fields always stay in whole
// seconds, so convert at the boundary.
function formatMinutes(seconds) {
  if (seconds == null || seconds === '') return '';
  return String(Number((Number(seconds) / 60).toFixed(3)));
}
function minutesToSeconds(minutes) {
  if (minutes === '' || minutes == null) return null;
  const value = Number(minutes);
  return Number.isFinite(value) ? Math.round(value * 60) : null;
}
function minuteInput(props) {
  return el('input', Object.assign({ type: 'number', min: 0, step: 0.05, inputmode: 'decimal' }, props || {}));
}
function formatSeconds(seconds) {
  if (seconds == null || seconds === '') return '';
  return String(Number(seconds));
}
function secondsValue(value) {
  if (value === '' || value == null) return null;
  const n = Number(value);
  return Number.isFinite(n) ? Math.round(n) : null;
}
function secondInput(props) {
  return el('input', Object.assign({ type: 'number', min: 0, step: 1, inputmode: 'numeric' }, props || {}));
}
const COUNTDOWN_MODE_DURATION = 'Duration before warning';
const COUNTDOWN_MODE_BUZZER = 'Time before buzzer';
function normalizeCountdownMode(mode) {
  return mode === 'Time until countdown expiration' || mode === 'time_until_countdown_expiration' || mode === 'duration_before_countdown_expiration'
    ? COUNTDOWN_MODE_DURATION
    : mode === COUNTDOWN_MODE_BUZZER || mode === 'time_before_buzzer' ? COUNTDOWN_MODE_BUZZER : COUNTDOWN_MODE_DURATION;
}
function normalizeConfigPayload(cfg) {
  if (!cfg) return cfg;
  if (cfg['config.buzzer.countdown.seconds_before'] == null && cfg['config.buzzer.countdown.minutes_before'] != null) {
    cfg['config.buzzer.countdown.seconds_before'] = Math.round(Number(cfg['config.buzzer.countdown.minutes_before'] || 0) * 60);
  }
  if (cfg['config.buzzer.countdown.mode'] != null)
    cfg['config.buzzer.countdown.mode'] = normalizeCountdownMode(cfg['config.buzzer.countdown.mode']);
  return cfg;
}
function normalizeLeaguePayload(league) {
  if (!league) return league;
  if (league.countdown_seconds_before == null && league.countdown_minutes_before != null) {
    league.countdown_seconds_before = Math.round(Number(league.countdown_minutes_before || 0) * 60);
  }
  return league;
}
// The engine uses U+02F8 (raised colon) on HDMI, but browser monospace fonts
// usually lack it and render a placeholder box. Normalize every browser-mirror
// countdown to ASCII ':' — simple colon, no wig-wag.
function browserCountdown(text) {
  return Array.from(text || '').map(ch => (ch === ':' || ch === '\u02F8') ? ':' : ch).join('');
}
function buildCountdownContent(text) {
  const nodes = [];
  let digitBuf = '';
  for (const char of Array.from(browserCountdown(text))) {
    if (/\d/.test(char)) {
      digitBuf += char;
    } else {
      if (digitBuf) { nodes.push(el('span', { class: 'cd-group' }, digitBuf)); digitBuf = ''; }
      nodes.push(el('span', { class: 'cd-colon' }, char));
    }
  }
  if (digitBuf) nodes.push(el('span', { class: 'cd-group' }, digitBuf));
  return nodes;
}
function todayIso() {
  const d = new Date();
  return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())}`;
}
function weekdayOf(iso) { return DAYS[(new Date(iso + 'T12:00:00').getDay() + 6) % 7]; }

let messageTimer = 0;
function showMessage(text, kind = 'bad') {
  const box = $('header-msg');
  if (box) { box.className = `banner ${kind}`; box.textContent = text; box.classList.remove('hidden'); }
  for (const id of ['savebar', 'config-savebar']) {
    const bar = $(id);
    if (bar) {
      let msg = bar.querySelector('.save-msg');
      if (!msg) { msg = document.createElement('span'); msg.className = 'save-msg'; bar.prepend(msg); }
      msg.textContent = text;
      msg.style.color = kind === 'ok' ? 'var(--ok)' : 'var(--bad)';
      msg.style.marginRight = '8px';
    }
  }
  clearTimeout(messageTimer);
  if (kind === 'ok') messageTimer = setTimeout(() => { box.classList.add('hidden'); for (const id of ['savebar', 'config-savebar']) { const m = $(id)?.querySelector('.save-msg'); if (m) m.textContent = ''; } }, 4000);
}
function clearMessage() { const h = $('header-msg'); if (h) h.classList.add('hidden'); }

async function api(path, body) {
  const options = body === undefined ? {} : {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'X-XSRF-TOKEN': csrfToken(),
    },
    body: JSON.stringify(body),
  };
  let response;
  try {
    response = await fetch(path, options);
  } catch (err) {
    return { ok: false, status: 0, data: { error: 'cannot reach the timer, check the network' } };
  }
  const data = await response.json().catch(() => ({ error: `unexpected response (${response.status})` }));
  if (response.status === 401 && path !== '/api/login') showLogin();
  if (response.status === 403 && path !== '/api/login') showLogin();
  return { ok: response.ok, status: response.status, data };
}

let _csrfTokenCache = null;
function csrfToken() {
  if (_csrfTokenCache) return _csrfTokenCache;
  const meta = document.querySelector('meta[name="csrf-token"]');
  if (meta && meta.content) {
    _csrfTokenCache = meta.content;
  }
  return _csrfTokenCache || '';
}

// ---- login ----------------------------------------------------------------
function showLogin() { $('login').classList.add('show'); $('password').focus(); }
$('login-form').addEventListener('submit', async (event) => {
  event.preventDefault();
  const btn = $('login-form').querySelector('button[type=submit]');
  btn.disabled = true; btn.textContent = 'Logging in...';
  await new Promise(r => requestAnimationFrame(() => requestAnimationFrame(r)));
  const result = await api('/api/login', { username: $('username').value, password: $('password').value });
  btn.disabled = false; btn.textContent = 'Log in';
  if (!result.ok) {
    $('login-error').textContent = result.data.error;
    $('login-error').classList.remove('hidden');
    $('login').classList.remove('show');
    return;
  }
  $('password').value = '';
  $('login-error').classList.add('hidden');
  $('login').classList.remove('show');
  start();
});
$('logout').addEventListener('click', async () => { await api('/api/logout', {}); location.reload(); });
$('help').addEventListener('click', () => { const m=document.getElementById('help-modal'); m.classList.remove('hidden'); m.classList.add('show'); });

// ---- tabs -----------------------------------------------------------------
function renderTabs() {
  const wide = window.innerWidth >= 768;
  const tabs = TABS.map(([id, icon, title]) =>
    el('button', { class: id === state.tab ? 'active' : '', title, onclick: () => selectTab(id) },
       wide ? NAV_ICONS[id] + ' ' + NAV_LABELS[id] : NAV_ICONS[id]));
  $('tabs').replaceChildren(...tabs);
  // Mobile bottom tab bar (icon + label always)
  $('bottom-nav').replaceChildren(...TABS.map(([id, icon, title]) =>
    el('button', { class: id === state.tab ? 'active' : '', title, onclick: () => selectTab(id) },
       el('span', { class: 'icon' }, icon),
       el('span', { class: 'label' }, title))));
  // Scheduling sub-tabs (Schedule | Leagues)
  const subNav = $('sub-tabs');
  if (state.tab === 'schedule') {
    subNav.classList.add('visible');
    subNav.querySelectorAll('button').forEach(btn => {
      btn.classList.toggle('active', btn.dataset.subtab === state.subTab);
      btn.onclick = () => selectSubTab(btn.dataset.subtab);
    });
  } else {
    subNav.classList.remove('visible');
  }
}

function selectSubTab(id) {
  state.subTab = id;
  renderTabs();
  clearMessage();
  for (const [tab] of TABS) $(`tab-${tab}`).classList.toggle('hidden', tab !== state.tab);
  // When on Scheduling tab, toggle between Schedule and Leagues sub-views
  if (state.tab === 'schedule') {
    const sched = $('tab-schedule');
    const leagues = $('tab-leagues');
    if (id === 'schedule') {
      sched.classList.remove('hidden');
      leagues.classList.add('hidden');
    } else if (id === 'leagues') {
      sched.classList.add('hidden');
      leagues.classList.remove('hidden');
    }
  }
  if (id === 'schedule') renderSchedule();
  if (id === 'leagues') renderLeagues();
}
window.addEventListener('resize', renderTabs);
async function selectTab(id) {
  state.tab = id;
  renderTabs();
  clearMessage();
  for (const [tab] of TABS) $(`tab-${tab}`).classList.toggle('hidden', tab !== id);
  if (id === 'status') { if (state.status) renderStatus(); loadStatus(); }
  if (id === 'quickstart') { if (!state.configValues) loadConfig(); await loadStatus(); renderQuickstart(); }
  if (id === 'schedule') {
    if (!state.model) await loadSettings();
    selectSubTab(state.subTab);
  }
  if (id === 'leagues') { renderLeagues(); }
  if (id === 'config') {
    if (!state.configOriginal || !state.configDirty) await loadConfig();
    else await renderConfig();
    await loadRaw();
  }
  if (id === 'diagnostics') renderDiagnostics();
}

// ---- status tab -----------------------------------------------------------
async function loadStatus() {
  const result = await api('/api/status');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  state.status = result.data;
  state.statusAt = performance.now();
  renderStatus();
  if (state.tab === 'quickstart') renderQuickstart();
}

// the quick start form is built once so the 5 second status refresh never wipes what is being typed
let quickForm = null;
const QUICK_PRESETS = [
  {
    id: 'practice',
    label: 'Practice (7m)',
    name: 'PRACTICE',
    game_completed: '--- PRACTICE DONE ---',
    minutes: 7,
    pregame_minutes: 5,
    header_hold_minutes: 1,
    stones_per_end: 8,
    end_count: 8,
    untimed_ends: 0,
    countdown_seconds_before: 0,
    options: {
      pregame: true,
      pace_of_play: false,
      show_progress_through_end: false,
      total_time_includes_untimed_ends: false,
      replace_conflicts: false,
    },
    start_relays: [],
    start_duration: null,
    middle_relays: [],
    middle_duration: null,
    end_relays: [],
    end_duration: null,
  },
  {
    id: 'game-6',
    label: 'Game (6 ends)',
    name: 'Game (6 ends)',
    game_completed: null,
    minutes: 75,
    pregame_minutes: 5,
    header_hold_minutes: 10,
    stones_per_end: 8,
    end_count: 6,
    untimed_ends: 0,
    options: {
      pregame: true,
      pace_of_play: true,
      show_progress_through_end: true,
      total_time_includes_untimed_ends: false,
      replace_conflicts: false,
    },
    start_relays: [],
    start_duration: null,
    middle_relays: [],
    middle_duration: null,
    end_relays: [],
    end_duration: null,
  },
  {
    id: 'game-8',
    label: 'Game (8 ends)',
    name: 'Game (8 ends)',
    game_completed: null,
    minutes: 105,
    pregame_minutes: 5,
    header_hold_minutes: 10,
    stones_per_end: 8,
    end_count: 8,
    untimed_ends: 0,
    countdown_seconds_before: 0,
    options: {
      pregame: true,
      pace_of_play: true,
      show_progress_through_end: true,
      total_time_includes_untimed_ends: false,
      replace_conflicts: false,
    },
    start_relays: [],
    start_duration: null,
    middle_relays: [],
    middle_duration: null,
    end_relays: [],
    end_duration: null,
  },
  {
    id: 'doubles-8',
    label: 'Doubles (8 ends)',
    name: 'Doubles (8 ends)',
    game_completed: null,
    minutes: 105,
    pregame_minutes: 5,
    header_hold_minutes: 10,
    stones_per_end: 5,
    end_count: 8,
    untimed_ends: 0,
    options: {
      pregame: true,
      pace_of_play: true,
      show_progress_through_end: true,
      total_time_includes_untimed_ends: false,
      replace_conflicts: false,
    },
    start_relays: [],
    start_duration: null,
    middle_relays: [],
    middle_duration: null,
    end_relays: [],
    end_duration: null,
  },
];

const QUICK_PRESET_STORAGE_KEY = 'curling_timer.quick_presets.v1';
const QUICK_PRESET_DELETED_STORAGE_KEY = 'curling_timer.quick_presets.deleted.v1';

function quickPresetNumber(value, fallback, min, max) {
  if (value === '' || value == null) return fallback;
  const number = Number(value);
  if (!Number.isFinite(number)) return fallback;
  return Math.min(max, Math.max(min, number));
}

function quickPresetInteger(value, fallback, min, max) {
  const number = quickPresetNumber(value, fallback, min, max);
  return number == null ? null : Math.round(number);
}

function quickPresetBoolean(value, fallback) {
  if (typeof value === 'boolean') return value;
  if (value === true || value === 1 || value === '1' || value === 'true') return true;
  if (value === false || value === 0 || value === '0' || value === 'false') return false;
  return fallback;
}

function quickPresetText(value, fallback) {
  if (typeof value !== 'string') return fallback;
  const text = value.trim();
  return text || fallback;
}

function quickPresetRelays(value, fallback) {
  const source = Array.isArray(value) ? value : fallback;
  if (!Array.isArray(source)) return [];
  const seen = new Set();
  const relays = [];
  for (const item of source) {
    if (typeof item !== 'string') continue;
    const relay = item.trim();
    if (!relay || seen.has(relay)) continue;
    seen.add(relay);
    relays.push(relay);
  }
  return relays;
}

function normalizeQuickPreset(raw, fallback = null) {
  if (!raw || typeof raw !== 'object' || Array.isArray(raw)) return null;
  const id = typeof raw.id === 'string' ? raw.id.trim() : '';
  if (!/^[A-Za-z0-9][A-Za-z0-9_-]{0,63}$/.test(id)) return null;

  const base = fallback || {
    id,
    label: id,
    name: id,
    game_completed: null,
    minutes: 0,
    pregame_minutes: 0,
    header_hold_minutes: 0,
    stones_per_end: 8,
    end_count: 8,
    untimed_ends: 0,
    countdown_seconds_before: 0,
    options: {
      pregame: false,
      pace_of_play: false,
      show_progress_through_end: false,
      total_time_includes_untimed_ends: false,
      replace_conflicts: false,
    },
    start_relays: [],
    start_duration: null,
    middle_relays: [],
    middle_duration: null,
    end_relays: [],
    end_duration: null,
  };
  const rawOptions = raw.options && typeof raw.options === 'object' && !Array.isArray(raw.options) ? raw.options : {};
  const baseOptions = base.options || {};
  const hasCompleted = Object.prototype.hasOwnProperty.call(raw, 'game_completed');
  const gameCompleted = hasCompleted
    ? (typeof raw.game_completed === 'string' ? raw.game_completed.trim() || null : null)
    : (typeof base.game_completed === 'string' && base.game_completed.trim() ? base.game_completed.trim() : null);

  return {
    id,
    label: quickPresetText(raw.label, base.label || id),
    name: quickPresetText(raw.name, base.name || id),
    game_completed: gameCompleted,
    minutes: quickPresetNumber(raw.minutes, base.minutes, 0, 179),
    pregame_minutes: quickPresetNumber(raw.pregame_minutes, base.pregame_minutes, 0, 60),
    header_hold_minutes: quickPresetNumber(raw.header_hold_minutes, base.header_hold_minutes, 0, 60),
    stones_per_end: quickPresetInteger(raw.stones_per_end, base.stones_per_end, 1, 10),
    end_count: quickPresetInteger(raw.end_count, base.end_count, 1, 12),
    untimed_ends: quickPresetInteger(raw.untimed_ends, base.untimed_ends, 0, 6),
    options: {
      pregame: quickPresetBoolean(rawOptions.pregame, !!baseOptions.pregame),
      pace_of_play: quickPresetBoolean(rawOptions.pace_of_play, !!baseOptions.pace_of_play),
      show_progress_through_end: quickPresetBoolean(rawOptions.show_progress_through_end, !!baseOptions.show_progress_through_end),
      total_time_includes_untimed_ends: quickPresetBoolean(rawOptions.total_time_includes_untimed_ends, !!baseOptions.total_time_includes_untimed_ends),
      replace_conflicts: quickPresetBoolean(rawOptions.replace_conflicts, !!baseOptions.replace_conflicts),
    },
    start_relays: quickPresetRelays(raw.start_relays, base.start_relays || []),
    start_duration: quickPresetInteger(raw.start_duration, base.start_duration, 0, 100000),
    end_relays: quickPresetRelays(raw.end_relays, base.end_relays || []),
    end_duration: quickPresetInteger(raw.end_duration, base.end_duration, 0, 100000),
  };
}

function readStoredQuickPresetObjects() {
  try {
    const raw = localStorage.getItem(QUICK_PRESET_STORAGE_KEY);
    if (!raw) return [];
    const parsed = JSON.parse(raw);
    if (!Array.isArray(parsed)) return [];
    return parsed.filter((item) => item && typeof item === 'object' && !Array.isArray(item)
      && typeof item.id === 'string' && /^[A-Za-z0-9][A-Za-z0-9_-]{0,63}$/.test(item.id.trim()));
  } catch (err) {
    return [];
  }
}

function readDeletedQuickPresetIds() {
  try {
    const raw = localStorage.getItem(QUICK_PRESET_DELETED_STORAGE_KEY);
    if (!raw) return [];
    const parsed = JSON.parse(raw);
    if (!Array.isArray(parsed)) return [];
    return parsed.filter((id) => typeof id === 'string' && /^[A-Za-z0-9][A-Za-z0-9_-]{0,63}$/.test(id));
  } catch (err) {
    return [];
  }
}

function mergeQuickPresets(builtins, stored, deletedIds) {
  const result = [];
  const positions = new Map();
  const deleted = deletedIds instanceof Set ? deletedIds : new Set(deletedIds || []);
  for (const builtin of builtins) {
    if (deleted.has(builtin.id)) continue;
    const preset = normalizeQuickPreset(builtin, builtin);
    if (!preset) continue;
    result.push(preset);
    positions.set(preset.id, result.length - 1);
  }
  for (const raw of stored) {
    const fallback = builtins.find((preset) => preset.id === raw.id) || null;
    const preset = normalizeQuickPreset(raw, fallback);
    if (!preset || deleted.has(preset.id)) continue;
    if (positions.has(preset.id)) {
      result[positions.get(preset.id)] = preset;
    } else {
      positions.set(preset.id, result.length);
      result.push(preset);
    }
  }
  return result;
}

function writeStoredQuickPresets(presets) {
  try {
    localStorage.setItem(QUICK_PRESET_STORAGE_KEY, JSON.stringify(presets));
    return true;
  } catch (err) {
    return false;
  }
}

function writeDeletedQuickPresetIds(ids) {
  try {
    localStorage.setItem(QUICK_PRESET_DELETED_STORAGE_KEY, JSON.stringify(Array.from(ids)));
    return true;
  } catch (err) {
    return false;
  }
}

function buildQuickForm() {
  const minutes = minuteInput({ max: 179, value: '0', 'aria-label': 'Full game time in minutes' });
  const name = el('input', { type: 'text', maxlength: 60, placeholder: 'QUICK START', 'aria-label': 'Name shown on display' });
  const completed = el('input', { type: 'text', maxlength: 120, placeholder: 'Default completion message', 'aria-label': 'Completed message' });
  const pregameMin = minuteInput({ max: 60, value: '5', 'aria-label': 'Pre-game countdown in minutes' });
  const expiresMin = minuteInput({ max: 60, value: '10', 'aria-label': 'Countdown wording hold in minutes' });
  const stonesPerEnd = el('input', { type: 'number', min: 1, max: 10, step: 1, inputmode: 'numeric', value: '8', 'aria-label': 'Stones per end' });
  const endCount = el('input', { type: 'number', min: 1, max: 12, step: 1, inputmode: 'numeric', value: '8', 'aria-label': 'Total number of ends' });
  const untimedEnds = el('input', { type: 'number', min: 0, max: 6, step: 1, inputmode: 'numeric', value: '0', 'aria-label': 'Ends after expiration' });
  const countdownSecondsBefore = el('input', { type: 'number', min: 0, max: 3600, step: 60, inputmode: 'numeric', value: '0', 'aria-label': 'Countdown Clock Expiration seconds before buzzer' });
  let pregameOn = true;
  let paceOn = true;
  let showProgressOn = true;
  let untimedTotalOn = false;
  let replaceOn = false;
  const pregameBtn = toggleBtn(pregameOn, 'Pre-Game Countdown');
  const paceBtn = toggleBtn(paceOn, 'Pace of play');
  const showProgressBtn = toggleBtn(showProgressOn, 'Show progress');
  const untimedTotalBtn = toggleBtn(untimedTotalOn, 'Full game countdown (incl. extra ends)');
  const replaceBtn = toggleBtn(replaceOn, 'Replace conflicts');
  const gameOptionsRow = el('div', { class: 'row-actions', style: 'margin: 0; gap: 8px; flex-wrap: wrap' },
    pregameBtn, paceBtn, showProgressBtn, untimedTotalBtn, replaceBtn);
  pregameBtn.onclick = (event) => { pregameOn = !pregameOn; event.target.className = 'check-toggle ' + (pregameOn ? 'on' : 'off'); document.getElementById('pregameField').style.display = pregameOn ? '' : 'none'; refreshPreview(); };
  paceBtn.onclick = (event) => { paceOn = !paceOn; event.target.className = 'check-toggle ' + (paceOn ? 'on' : 'off'); refreshPreview(); };
  showProgressBtn.onclick = (event) => { showProgressOn = !showProgressOn; event.target.className = 'check-toggle ' + (showProgressOn ? 'on' : 'off'); refreshPreview(); };
  untimedTotalBtn.onclick = (event) => { untimedTotalOn = !untimedTotalOn; event.target.className = 'check-toggle ' + (untimedTotalOn ? 'on' : 'off'); refreshPreview(); };
  replaceBtn.onclick = (event) => { replaceOn = !replaceOn; event.target.className = 'check-toggle ' + (replaceOn ? 'on' : 'off'); refreshPreview(); };

  const options = () => ({ pregame: pregameOn, pace_of_play: paceOn, replace_conflicts: replaceOn, show_progress_through_end: showProgressOn, total_time_includes_untimed_ends: untimedTotalOn, countdown_seconds_before: secondsValue(countdownSecondsBefore.value) || 0 });

  // Outputs driven from global settings: only enabled relays are offered,
  // each with its own duration field for Countdown Clock Start, Countdown Clock Expiration,
  // and Countdown Clock End. Durations are hidden via the wrapping .duration div
  // (not the input itself) so toggling on/off reliably shows/hides the field.
  const relayNames = RELAY_NAMES();
  const relayDisplay = (i) => relayDisplayName(i) || relayNames[i];
  const makeRelayRow = (label, toggles, durs) => {
    const row = el('div', { class: 'row-actions', style: 'margin-bottom: 10px; gap: 10px; align-items: center' },
      el('span', { class: 'muted small', style: 'margin-right: 8px' }, label));
    for (let i = 0; i < toggles.length; i++) {
      const durWrap = el('div', { class: 'duration', style: 'display:none', gap: 6 }, durs[i], el('span', {}, 's'));
      row.append(toggles[i]);
      row.append(durWrap);
      toggles[i]._durWrap = durWrap;
    }
    return row;
  };
  const startToggles = relayNames.map((name, i) => toggleBtn(false, relayDisplay(i)));
  const startDurs = relayNames.map(() => secondInput({ max: 100000, placeholder: '0', 'aria-label': 'Countdown Clock Start relay duration in seconds' }));
  const middleToggles = relayNames.map((name, i) => toggleBtn(false, relayDisplay(i)));
  const middleDurs = relayNames.map(() => secondInput({ max: 100000, placeholder: '0', 'aria-label': 'Countdown Clock Expiration relay duration in seconds' }));
  const endToggles = relayNames.map((name, i) => toggleBtn(false, relayDisplay(i)));
  const endDurs = relayNames.map(() => secondInput({ max: 100000, placeholder: '0', 'aria-label': 'Countdown Clock End relay duration in seconds' }));
  const startRow = relayNames.length ? makeRelayRow('Countdown Clock Start:', startToggles, startDurs) : el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.');
  const middleRow = relayNames.length ? makeRelayRow('Countdown Clock Expiration:', middleToggles, middleDurs) : el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.');
  const endRow = relayNames.length ? makeRelayRow('Countdown Clock End:', endToggles, endDurs) : el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.');

  // relayValues: collect names of enabled relays by original index (filter()
  // reindexes, so we must carry the original position explicitly).
  const relayValues = (toggles) => toggles.map((b, i) => b.className.includes('on') ? relayNames[i] : null).filter(Boolean);
  const relayPreview = (toggles, durs) => toggles
    .map((b, i) => b.className.includes('on')
      ? { name: relayDisplay(i), seconds: secondsValue(durs[i].value) }
      : null)
    .filter(Boolean);
  const relayMaxDuration = (durs) => { let m = 0; for (const d of durs) { const v = secondsValue(d.value) || 0; if (v > m) m = v; } return m || null; };

  const wireRelayToggle = (toggles) => toggles.forEach((btn) => {
    btn.onclick = (event) => {
      const next = !btn.className.includes('on');
      btn.className = 'check-toggle ' + (next ? 'on' : 'off');
      btn._durWrap.style.display = next ? '' : 'none';
      refreshPreview();
    };
  });
  wireRelayToggle(startToggles);
  wireRelayToggle(middleToggles);
  wireRelayToggle(endToggles);
  const refreshPreview = () => {
    const gameSecs = minutesToSeconds(minutes.value) || 0;
    const expires = minutesToSeconds(expiresMin.value) ?? 600;
    const total = gameSecs + expires;
    const when = new Date(Date.now() + total * 1000);
    const endN = Number(endCount.value) || 8;
    const untimed = Number(untimedEnds.value) || 0;
    const startOutputs = relayPreview(startToggles, startDurs);
    const middleOutputs = relayPreview(middleToggles, middleDurs);
    const endOutputs = relayPreview(endToggles, endDurs);

    preview.replaceChildren(
      el('div', { class: 'section-title', style: 'margin:0 0 10px' }, 'Preview — settings that will be applied'),
      previewLine('Game time', gameSecs === 0 ? '—' : durationText(gameSecs)),
      previewLine('Expires at', gameSecs === 0 ? '—' : pad(when.getHours()) + ':' + pad(when.getMinutes())),
      previewLine('Pre-Game Countdown', pregameOn ? (formatMinutes(minutesToSeconds(pregameMin.value) || 0) + ' min') : 'Off'),
      previewLine('Completed message', completed.value.trim() || 'Default'),
      previewLine('Pace of play', paceOn ? 'On' : 'Off'),
      previewLine('Progress through end', showProgressOn ? 'On' : 'Off'),
      previewLine('Replace same-time games', replaceOn ? 'On' : 'Off'),
      previewLine('Countdown', untimedTotalOn ? 'Full game (extra ends included)' : 'Stops at the expiration'),
      previewLine('Stones per end', stonesPerEnd.value || 8),
      previewLine('Total ends', endCount.value || 8),
      previewLine('Ends after expiration', untimed > 0 ? `${untimed} extra end(s) played after the buzzer so the game can finish naturally` : 'Game ends at the buzzer'),
      el('div', { style: 'margin-top:10px;border-top:1px solid rgba(255,255,255,0.10);padding-top:10px' }),
      previewLine('Outputs — Countdown Clock Start', startOutputs.length ? startOutputs.map((o) => o.name + ' ' + (o.seconds ? formatSeconds(o.seconds) + ' s' : 'default')).join(', ') : 'None'),
      previewLine('Outputs — Countdown Clock Expiration', middleOutputs.length ? middleOutputs.map((o) => o.name + ' ' + (o.seconds ? formatSeconds(o.seconds) + ' s' : 'default')).join(', ') : 'None'),
      previewLine('Outputs — Countdown Clock End', endOutputs.length ? endOutputs.map((o) => o.name + ' ' + (o.seconds ? formatSeconds(o.seconds) + ' s' : 'default')).join(', ') : 'None'),
    );
  };

  startDurs.forEach((d) => { d.addEventListener('input', refreshPreview); d.addEventListener('change', refreshPreview); });
  middleDurs.forEach((d) => { d.addEventListener('input', refreshPreview); d.addEventListener('change', refreshPreview); });
  endDurs.forEach((d) => { d.addEventListener('input', refreshPreview); d.addEventListener('change', refreshPreview); });
  middleDurs.forEach((d) => { d.addEventListener('input', refreshPreview); d.addEventListener('change', refreshPreview); });
  endDurs.forEach((d) => { d.addEventListener('input', refreshPreview); d.addEventListener('change', refreshPreview); });

  const button = el('button', { class: 'btn primary', type: 'submit' }, 'Start game');
  const hint = el('span', { class: 'muted small' });

  // Preview section: shows what will be applied, styled like the display.
  const previewLine = (label, value) => el('div', { style: 'display:flex;justify-content:space-between;gap:12px;font-size:13px;color:var(--text);margin:4px 0' },
    el('div', { style: 'color:var(--text-dim);font-size:11px;text-transform:uppercase;letter-spacing:0.05em' }, label),
    el('div', { style: 'text-align:right;font-weight:600;color:var(--text-strong)' }, value));
  const preview = el('div', {
    class: 'card',
    style: 'margin: 0 0 14px; background:rgba(2,10,20,0.45); border:1px solid rgba(255,255,255,0.12); padding:16px'
  });



  const wire = (input) => { input.addEventListener('input', refreshPreview); input.addEventListener('change', refreshPreview); };
  wire(minutes); wire(expiresMin); wire(pregameMin); wire(completed); wire(countdownSecondsBefore);
  [pregameBtn, paceBtn, showProgressBtn, replaceBtn].forEach(b => { b.addEventListener('click', refreshPreview); });
  [stonesPerEnd, endCount, untimedEnds].forEach(i => { i.addEventListener('input', refreshPreview); i.addEventListener('change', refreshPreview); });
  [...startToggles, ...startDurs, ...endToggles, ...endDurs].forEach(i => { i.addEventListener('input', refreshPreview); i.addEventListener('change', refreshPreview); });

  let storedQuickPresets = readStoredQuickPresetObjects()
    .map((item) => normalizeQuickPreset(item, QUICK_PRESETS.find((preset) => preset.id === item.id) || null))
    .filter(Boolean);
  let deletedQuickPresetIds = new Set(readDeletedQuickPresetIds());
  let activePresets = mergeQuickPresets(QUICK_PRESETS, storedQuickPresets, deletedQuickPresetIds);
  let presets = [];
  let quickPicksEnabled = false;
  let editingPresetId = null;
  const presetRow = el('div', { class: 'row-actions', style: 'margin: 0; gap: 8px' });
  let form = null;

  const captureQuickPreset = (id, label) => ({
    id,
    label,
    name: name.value.trim() || label,
    game_completed: completed.value.trim() || null,
    minutes: quickPresetNumber(minutes.value, 0, 0, 179),
    pregame_minutes: quickPresetNumber(pregameMin.value, 0, 0, 60),
    header_hold_minutes: quickPresetNumber(expiresMin.value, 0, 0, 60),
    stones_per_end: quickPresetInteger(stonesPerEnd.value, 8, 1, 10),
    end_count: quickPresetInteger(endCount.value, 8, 1, 12),
    untimed_ends: quickPresetInteger(untimedEnds.value, 0, 0, 6),
    options: options(),
    start_relays: relayValues(startToggles),
    start_duration: relayMaxDuration(startDurs),
    middle_relays: relayValues(middleToggles),
    middle_duration: relayMaxDuration(middleDurs),
    end_relays: relayValues(endToggles),
    end_duration: relayMaxDuration(endDurs),
  });

  const setToggleState = (button, on) => {
    button.className = 'check-toggle ' + (on ? 'on' : 'off');
    button.setAttribute('aria-pressed', String(on));
  };

  const applyRelayPreset = (toggles, durations, selectedRelays, duration) => {
    const names = Array.isArray(selectedRelays) ? selectedRelays : [];
    toggles.forEach((button, index) => {
      const on = names.includes(relayNames[index]);
      setToggleState(button, on);
      button._durWrap.style.display = on ? '' : 'none';
      durations[index].value = on && duration != null ? formatSeconds(duration) : '';
    });
  };

  const applyQuickPreset = (preset) => {
    if (!preset) return;
    minutes.value = String(preset.minutes);
    name.value = preset.name || '';
    completed.value = preset.game_completed || '';
    pregameMin.value = String(preset.pregame_minutes);
    expiresMin.value = String(preset.header_hold_minutes);
    stonesPerEnd.value = String(preset.stones_per_end);
    endCount.value = String(preset.end_count);
    untimedEnds.value = String(preset.untimed_ends);
    countdownSecondsBefore.value = String(preset.countdown_seconds_before || 0);

    pregameOn = !!preset.options.pregame;
    paceOn = !!preset.options.pace_of_play;
    showProgressOn = !!preset.options.show_progress_through_end;
    untimedTotalOn = !!preset.options.total_time_includes_untimed_ends;
    replaceOn = !!preset.options.replace_conflicts;
    setToggleState(pregameBtn, pregameOn);
    setToggleState(paceBtn, paceOn);
    setToggleState(showProgressBtn, showProgressOn);
    setToggleState(untimedTotalBtn, untimedTotalOn);
    setToggleState(replaceBtn, replaceOn);

    applyRelayPreset(startToggles, startDurs, preset.start_relays, preset.start_duration);
    applyRelayPreset(middleToggles, middleDurs, preset.middle_relays, preset.middle_duration);
    applyRelayPreset(endToggles, endDurs, preset.end_relays, preset.end_duration);
    refreshPreview();
    if (form) form.scrollIntoView({ behavior: 'smooth', block: 'start' });
    showMessage(`Loaded quick pick "${preset.label}" — review and press Start game.`, 'ok');
  };

  const renderQuickPicks = () => {
    const rows = [];
    presets = [];
    for (const preset of activePresets) {
      const pickButton = el('button', { class: 'btn preset', type: 'button',
        title: `${preset.name}, ${durationText(preset.minutes * 60)}`,
        onclick: () => applyQuickPreset(preset) }, preset.label);
      const editButton = el('button', { class: 'btn icon', type: 'button',
        'aria-label': `Edit quick pick ${preset.label}`,
        title: `Edit "${preset.label}"`,
        onclick: () => {
          editingPresetId = preset.id;
          applyQuickPreset(preset);
          name.focus();
          name.select();
          showMessage(`Editing quick pick "${preset.label}" — change the name and save.`, 'ok');
        } }, '✏️');
      const deleteButton = el('button', { class: 'btn icon danger', type: 'button',
        'aria-label': `Delete quick pick ${preset.label}`,
        title: `Delete "${preset.label}"`,
        onclick: () => {
          if (!confirm(`Delete quick pick "${preset.label}"?`)) return;
          storedQuickPresets = storedQuickPresets.filter((item) => item.id !== preset.id);
          if (QUICK_PRESETS.some((item) => item.id === preset.id)) deletedQuickPresetIds.add(preset.id);
          activePresets = mergeQuickPresets(QUICK_PRESETS, storedQuickPresets, deletedQuickPresetIds);
          const storedSaved = writeStoredQuickPresets(storedQuickPresets);
          const deletedSaved = writeDeletedQuickPresetIds(deletedQuickPresetIds);
          if (editingPresetId === preset.id) editingPresetId = null;
          renderQuickPicks();
          if (storedSaved && deletedSaved) {
            showMessage(`Deleted quick pick "${preset.label}".`, 'ok');
          } else {
            showMessage(`Deleted "${preset.label}" for this session, but browser storage could not be updated.`, 'bad');
          }
        } }, '✕');
      presets.push(pickButton);
      rows.push(el('div', { class: 'row-actions', style: 'margin: 0; gap: 4px' }, pickButton, editButton, deleteButton));
    }
    presetRow.replaceChildren(...rows);
    presets.forEach((b) => { b.disabled = !quickPicksEnabled; });
  };

  const saveAsQuickPick = () => {
    const editingPreset = editingPresetId ? activePresets.find((preset) => preset.id === editingPresetId) : null;
    const defaultLabel = (editingPreset && editingPreset.label) || name.value.trim() || 'Quick pick';
    const enteredLabel = prompt('Quick pick label', defaultLabel);
    if (enteredLabel === null) return;
    const label = enteredLabel.trim() || name.value.trim() || 'Quick pick';
    const matchingPreset = activePresets.find((preset) =>
      preset.label.toLowerCase() === label.toLowerCase() || preset.id.toLowerCase() === label.toLowerCase()) || editingPreset;

    let id = matchingPreset ? matchingPreset.id : '';
    if (!id) {
      const usedIds = new Set(QUICK_PRESETS.map((preset) => preset.id)
        .concat(activePresets.map((preset) => preset.id), storedQuickPresets.map((preset) => preset.id), Array.from(deletedQuickPresetIds)));
      const base = label.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-+|-+$/g, '') || 'quick-pick';
      id = base;
      let suffix = 2;
      while (usedIds.has(id)) id = `${base}-${suffix++}`;
    }

    const preset = captureQuickPreset(id, label);
    preset.label = label;
    const storedIndex = storedQuickPresets.findIndex((item) => item.id === id);
    if (storedIndex >= 0) storedQuickPresets[storedIndex] = preset;
    else storedQuickPresets.push(preset);
    deletedQuickPresetIds.delete(id);
    activePresets = mergeQuickPresets(QUICK_PRESETS, storedQuickPresets, deletedQuickPresetIds);
    const storedSaved = writeStoredQuickPresets(storedQuickPresets);
    const deletedSaved = writeDeletedQuickPresetIds(deletedQuickPresetIds);
    editingPresetId = null;
    renderQuickPicks();
    if (storedSaved && deletedSaved) {
      showMessage(`Saved quick pick "${label}".`, 'ok');
    } else {
      showMessage(`Saved "${label}" for this session, but browser storage could not be updated.`, 'bad');
    }
  };

  const resetQuickPicks = () => {
    storedQuickPresets = [];
    deletedQuickPresetIds.clear();
    activePresets = mergeQuickPresets(QUICK_PRESETS, storedQuickPresets, deletedQuickPresetIds);
    let cleared = true;
    try {
      localStorage.removeItem(QUICK_PRESET_STORAGE_KEY);
      localStorage.removeItem(QUICK_PRESET_DELETED_STORAGE_KEY);
    } catch (err) {
      cleared = false;
    }
    editingPresetId = null;
    renderQuickPicks();
    if (cleared) showMessage('Quick picks reset to the four built-in presets.', 'ok');
    else showMessage('Quick picks reset for this session, but browser storage could not be cleared.', 'bad');
  };

  const savePresetButton = el('button', { class: 'btn', type: 'button', onclick: saveAsQuickPick }, 'Save as quick pick');
  const resetPresetButton = el('button', { class: 'btn', type: 'button', onclick: resetQuickPicks }, 'Reset quick picks');

  form = el('form', { class: 'card', onsubmit: (event) => {
      event.preventDefault();
      const timer = readQuickForm(minutes, name);
      if (!timer) return;
      const completedMessage = completed.value.trim();
      launchQuick({
        seconds: timer.seconds,
        name: timer.name,
        header_hold: minutesToSeconds(expiresMin.value) ?? 600,
        ...options(),
        pregame_countdown: (pregameOn ? (minutesToSeconds(pregameMin.value) || 0) : 0),
        start_relays: relayValues(startToggles),
        start_duration: relayMaxDuration(startDurs),
        middle_relays: relayValues(middleToggles),
        middle_duration: relayMaxDuration(middleDurs),
        end_relays: relayValues(endToggles),
        end_duration: relayMaxDuration(endDurs),
        end_count: Number(endCount.value) || 8,
        stones_per_end: Number(stonesPerEnd.value) || 8,
        untimed_ends: Number(untimedEnds.value) || 0,
        ...(completedMessage ? { game_completed: completedMessage } : {}),
      });
    } },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Quick Game')),
      el('span', { class: 'pill' }, 'Ad hoc timer')),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Click a quick pick to fill the form, then review and press Start game. The preview shows exactly what will be applied.'),
    // Presets section - keep at the very top per requirements
    el('div', { class: 'card', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 8px' }, 'Quick picks'),
      el('p', { class: 'muted small', style: 'margin: 0 0 10px' }, 'Quick picks fill the form below — nothing starts until you press Start game.'),
      presetRow,
      el('div', { class: 'row-actions', style: 'margin: 0; gap: 8px' }, savePresetButton, resetPresetButton)),
    // Game options toggles row (above fields so pregame toggle controls its field)
    el('div', { class: 'row-actions', style: 'margin: 0; gap: 10px; align-items: center' },
      pregameBtn,
      paceBtn,
      showProgressBtn,
      untimedTotalBtn,
      replaceBtn),

    // Main game settings — field order:
    // Pre-Game Countdown, Full Game Time, Countdown Clock Expiration,
    // Total Number of Ends, Stones Per End, Ends after Expiration,
    // Name/Completed Message + Countdown wording hold (last grid2 group)
    el('div', { id: 'pregameField', class: 'field', style: 'margin-bottom: 10px; display:' + (pregameOn ? '' : 'none') },
      el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Pre-Game Countdown (minutes)'),
      pregameMin),
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Full Game Time (minutes)'),
      minutes),
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Countdown Clock Expiration (seconds)'),
      countdownSecondsBefore,
      el('div', { class: 'muted small', style: 'margin-top: 4px' }, 'Seconds before the buzzer at which the countdown reaches zero when full game countdown is off.')),
    el('div', { class: 'grid2', style: 'gap: 12px' },
      el('div', { class: 'field' },
        el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Total Number of Ends'),
        endCount),
      el('div', { class: 'field' },
        el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Stones Per End'),
        stonesPerEnd)),
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Ends after Expiration'),
      untimedEnds,
      el('div', { class: 'muted small', style: 'margin-top: 4px' }, 'Number of ends to complete after countdown expiration. Finish the current end and play the number of ends set here — usually 1 end is common.')),
    el('div', { class: 'grid2', style: 'gap: 12px' },
      el('div', { class: 'field' },
        el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Name shown on display'),
        name),
      el('div', { class: 'field' },
        el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Completed Message (optional)'),
        completed),
      el('div', { class: 'field' },
        el('label', { class: 'muted', style: 'margin-bottom: 4px' }, 'Countdown wording hold after buzzer (minutes)'),
        expiresMin)),
    // Outputs card
    el('div', { class: 'card', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 4px' }, 'Outputs'),
      el('p', { class: 'muted small', style: 'margin: 0 0 10px' }, 'Choose which enabled relay outputs ring at Countdown Clock Start, at Countdown Clock Expiration, and at Countdown Clock End. Leave a duration blank to use the global buzzer duration.'),
      startRow,
      endRow),

    // Preview card (styled like schedule display preview) — populated by refreshPreview()
    preview,

    // Action row (button and hint)
    el('div', { class: 'row-actions' }, button, hint));
  form._defaultsApplied = false;
  form.applyDefaults = () => {
    if (form._defaultsApplied) return;
    const st = state.status || {};
    const cv = state.configValues || ((state.configOriginal && state.configOriginal.config) || {});
    const pg = Number((cv && cv['config.pregame_countdown'] != null) ? cv['config.pregame_countdown'] : (st.pregame_countdown_seconds != null ? st.pregame_countdown_seconds : 0)) || 0;
    pregameOn = pg > 0;
    pregameBtn.className = 'check-toggle ' + (pregameOn ? 'on' : 'off');
    pregameMin.value = formatMinutes(pg);
    const paceEnabled = cv && cv['config.pace_of_play.enabled'] != null ? !!cv['config.pace_of_play.enabled'] : true;
    paceOn = !!paceEnabled;
    paceBtn.className = 'check-toggle ' + (paceOn ? 'on' : 'off');
    showProgressOn = true;
    showProgressBtn.className = 'check-toggle on';
    untimedTotalOn = false;
    untimedTotalBtn.className = 'check-toggle off';
    // Pull default output relays from Settings.
    const startRelays = Array.isArray(cv && cv['config.buzzer.ring_at_start']) ? cv['config.buzzer.ring_at_start'] : [];
    const middleRelays = Array.isArray(cv && cv['config.buzzer.countdown.relays']) ? cv['config.buzzer.countdown.relays'] : [];
    const endRelays = Array.isArray(cv && cv['config.buzzer.relays']) ? cv['config.buzzer.relays'] : [];
    const startDur = cv && cv['config.buzzer.start_duration'] != null ? Number(cv['config.buzzer.start_duration']) || 0 : 0;
    const middleDur = cv && cv['config.buzzer.countdown_duration'] != null ? Number(cv['config.buzzer.countdown_duration']) || 0 : 0;
    const endDur = cv && cv['config.buzzer.end_duration'] != null ? Number(cv['config.buzzer.end_duration']) || 0 : 0;
    applyRelayPreset(startToggles, startDurs, startRelays, startDur);
    applyRelayPreset(middleToggles, middleDurs, middleRelays, middleDur);
    applyRelayPreset(endToggles, endDurs, endRelays, endDur);
    refreshPreview();
    form._defaultsApplied = true;
  };
  form.update = (enabled, clockOk = true) => {
    button.disabled = !enabled;
    quickPicksEnabled = enabled;
    presets.forEach((b) => { b.disabled = !enabled; });
    hint.textContent = enabled ? '' : (clockOk ? 'Enable the timer first' : 'Waiting for the clock to sync');
    if (enabled || clockOk || (state.status && state.status.pregame_countdown_seconds != null)) form.applyDefaults();
  };
  renderQuickPicks();
  refreshPreview();
  return form;
}

function renderStatus() {
  const s = state.status;
  const root = $('tab-status');
  if (!s) return;
  if (!document.getElementById('status-now')) root.replaceChildren(el('div', { id: 'status-now' }), el('div', { id: 'status-today' }));
  if (quickForm) quickForm.classList.toggle('hidden', !s.valid);
  if (!s.valid) {
    $('status-now').replaceChildren(el('div', { class: 'card' },
      el('div', { class: 'section-title' }, 'System status'),
      el('h3', {}, 'Settings file has an error'),
      el('div', { class: 'banner bad' }, s.error),
      el('p', { class: 'muted' }, 'The display shows this error until the file is fixed. Use Settings → Raw settings or History to recover.')));
    $('status-today').replaceChildren();
    return;
  }
  if (quickForm) quickForm.update(s.enabled && s.clock_synced, s.clock_synced);
  const now = el('div', { class: 'card' });
  const cur = s.current;
  if (!s.enabled) {
    now.append(el('div', { class: 'section-title' }, 'Timer status'),
      el('div', { class: 'state', style: 'color: var(--bad); font-size: 22px' }, 'TIMER DISABLED'),
      el('p', { class: 'muted small' }, 'The display shows the disabled message and no buzzers will ring.'),
      el('div', { class: 'row-actions' }, el('button', { class: 'btn primary', onclick: () => toggleEnabled(true) }, 'Enable timer')));
  } else if (!s.clock_synced) {
    now.append(el('div', { class: 'section-title' }, 'Timer status'),
      el('div', { class: 'banner warn' }, 'Waiting for the clock to sync (SNTP). The countdown, today\'s games and quick start appear once the time is known.'));
  } else if (cur) {
    now.style.padding = '12px 14px';
    if (!isAddonEnabled('relays')) now.append(el('div', { class: 'banner warn', style: 'margin-bottom:8px' }, 'Relay add-on is disabled — bells/buzzer will not ring. Enable it in Settings → Add-ons.'));
    const displayName = cur.name || (cur.quick ? 'Quick Start' : 'Current game');
    const header = el('div', { style: 'display:flex; align-items:flex-start; justify-content:space-between; gap:12px; margin-bottom:6px' },
      el('div', {},
        el('div', { class: 'section-title', style: 'margin-bottom:2px' }, cur.quick ? 'Now on ice (Quick Start)' : 'Now on ice'),
        el('div', { style: 'font-size:17px; font-weight:700; line-height:1.2' }, displayName)),
      el('span', { class: 'pill ok' }, 'Timer enabled'));
    const meta = el('div', { style: 'display:flex; flex-wrap:wrap; gap:14px; align-items:center; color:var(--muted); font-size:13px' },
      el('span', {}, el('strong', { style: 'color:var(--text); font-weight:600' }, 'Start: '), shortTime(cur.start)),
      el('span', {}, el('strong', { style: 'color:var(--text); font-weight:600' }, 'Game expiration: '), shortTime(cur.buzzer)));
    const actions = el('div', { class: 'row-actions', style: 'margin-top:8px' });
    if (cur.quick) actions.append(el('button', { class: 'btn sm', onclick: () => cancelQuick(cur) }, 'Cancel quick start'));
    actions.append(el('button', { class: 'btn danger sm', onclick: () => toggleEnabled(false) }, 'Disable timer'));
    now.append(header, meta, actions);
  } else {
    now.style.padding = '12px 14px';
    if (!isAddonEnabled('relays')) now.append(el('div', { class: 'banner warn', style: 'margin-bottom:8px' }, 'Relay add-on is disabled — bells/buzzer will not ring. Enable it in Settings → Add-ons.'));
    const header = el('div', { style: 'display:flex; align-items:flex-start; justify-content:space-between; gap:12px; margin-bottom:6px' },
      el('div', {},
        el('div', { class: 'section-title', style: 'margin-bottom:2px' }, 'Status'),
        el('div', { style: 'font-size:17px; font-weight:700; line-height:1.2' }, 'Idle')),
      el('span', { class: 'pill ok' }, 'Timer enabled'));
    const meta = s.next
      ? el('div', { style: 'display:flex; flex-wrap:wrap; gap:14px; align-items:center; color:var(--muted); font-size:13px' },
          el('span', {}, el('strong', { style: 'color:var(--text); font-weight:600' }, 'Next: '), s.next.name || 'Game'),
          el('span', {}, el('strong', { style: 'color:var(--text); font-weight:600' }, 'Start: '), shortTime(s.next.start)),
          el('span', {}, el('strong', { style: 'color:var(--text); font-weight:600' }, 'Game expiration: '), shortTime(s.next.buzzer)))
      : el('div', { class: 'muted small' }, 'No more games today');
    now.append(header, meta,
      el('div', { class: 'row-actions', style: 'margin-top:8px' }, el('button', { class: 'btn danger sm', onclick: () => toggleEnabled(false) }, 'Disable timer')));
  }
  let mirror = document.getElementById('display-mirror');
  if (!mirror) {
    mirror = el('div', { id: 'display-mirror', class: 'display-mirror', style: 'background:#000;color:#fff;border-radius:18px;position:relative;overflow:hidden;width:100%;max-width:100%;aspect-ratio:16/9;margin:0 auto' });
  }
  const mirrorCard = el('div', { class: 'card', style: 'padding:14px; grid-column: 1 / -1; max-width: 100%' },
    el('div', { class: 'page-head', style: 'margin-bottom:10px' },
      el('div', {}, el('div', { class: 'section-title' }, 'Live display')),
      el('span', { class: 'pill' }, `Scale ${Math.round((s.mirror_scale ?? 100))}%`)),
    mirror);
  $('status-now').replaceChildren(mirrorCard, now);

  if (!s.clock_synced) {
    $('status-today').replaceChildren();
    return;
  }
  const today = el('div', { class: 'card' },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Today')),
      s.today.source === 'exception' ? el('span', { class: 'pill warn', style: 'max-width:220px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap', title: s.today.note ? s.today.note : '' }, `Exception${s.today.note ? ': ' + s.today.note : ''}`)
                                     : el('span', { class: 'pill' }, 'Weekly schedule')));
  const nowSecs = secondsNow();
  if (!s.today.entries.length) today.append(el('p', { class: 'muted' }, 'No buzzers today.'));
  else today.append(el('ul', { class: 'today' },
    el('li', { class: 'head' }, el('span', {}, 'Start'), el('span', {}, 'Bell/Buzzer'), el('span', { class: 'name' }, 'Entry Name')),
    [...s.today.entries].sort((a, b) => a.start - b.start).map((e) => el('li', {
      class: cur && cur.start === e.start ? 'now' : (e.buzzer + (e.header_hold != null ? e.header_hold : s.header_hold) < nowSecs ? 'past' : '') },
    el('span', {}, shortTime(e.start)),
    el('span', {}, shortTime(e.buzzer)),
    el('span', { class: 'name' }, e.name ? e.name : '',
       e.quick ? el('span', { class: 'pill quick' }, 'quick') : null)))));
  $('tab-status').querySelector('#status-today').replaceChildren(today);
   const sm = state.status || {};
   function unswap(c) { if (!c || typeof c !== 'string' || c.length < 7) return c || ''; return '#' + c.slice(5,7) + c.slice(3,5) + c.slice(1,3); }
  const userScale = ((sm.mirror_scale != null ? Number(sm.mirror_scale)
        : (state.configValues && state.configValues['config.mirror_scale'] != null ? Number(state.configValues['config.mirror_scale']) : 100)) / 100);
  const vw = Math.max(1, (mirror.getBoundingClientRect().width || mirror.clientWidth || 0));
  const renderScale = vw ? (vw / 1280) : 1;
  const scale = renderScale * userScale;
  const px = (n) => Math.max(1, Math.round((n || 0) * scale));

  // Font sizes from settings, scaled to 16:9 1280x720 HDMI reference (mirror width / 1280 * mirror_scale)
  const baseHeader = Number(sm.font_header);
  const baseTimer4 = Number(sm.font_timer_4 || sm.font_timer);
  const baseTimer5 = Number(sm.font_timer_5 || Math.round(baseTimer4 * 370 / 450));
  const basePace = Number(sm.font_pace);
  const baseFooter = Number(sm.font_footer);
    const isFiveDigit = !!(sm.show_countdown && sm.seconds_until_buzzer >= 3600);
    const baseTimer = isFiveDigit ? baseTimer5 : baseTimer4;
    const pregame = !!sm.is_pregame;
    const pregameBlink = !!sm.pregame_blink;
    const baseCountdown = pregame ? Math.round(baseTimer * 1.1) : baseTimer;

  const fh = px(baseHeader);
  const fcd = px(baseCountdown);
  const fp = px(basePace);
  const ff = px(baseFooter);
  const padY = px(20); // HDMI header y: 20
  const padX = px(10); // HDMI footer x: 10

  mirror.style.background = '#000';
  mirror.replaceChildren();
  const stage = el('div', { style: 'position:absolute; inset:0; overflow:hidden;' });

  if (sm.header_text) {
    const h = el('div', { id: 'mirror-header', style: `position:absolute; top:${padY}px; left:0; width:100%; box-sizing:border-box; padding:0 ${px(12)}px; color:${unswap(sm.header_color) || '#fff'}; font-size:${fh}px; font-weight:bold; text-align:center; line-height:1; white-space:nowrap; overflow:hidden;` }, sm.header_text);
    h.dataset.maxFont = String(fh);
    stage.append(h);
  }

  const showPace = !!(sm.show_pace && sm.pace_text);
  const showProgress = !!(sm.show_progress && sm.progress_end_count > 0 && sm.progress_game_duration > 0);
  const progressEndCount = Number(sm.progress_end_count || (s.current && s.current.end_count) || 0);
  const progressGameDuration = Number(sm.progress_game_duration || (s.current ? (s.current.buzzer - s.current.start) : 0));
  const centerTop = sm.header_text ? px(20 + baseHeader * 1.4) : px(20);
  // HDMI pill: y=-79, height 95. Reserve that block plus a small gap so the
  // centre content never overlaps the single progress bar.
  const centerBottom = showProgress ? px(94 + 105 + 8) : px(20 + baseFooter * 1.4);
  const centerWrap = el('div', { style: `position:absolute; left:${px(12)}px; right:${px(12)}px; top:${centerTop}px; bottom:${centerBottom}px; display:flex; flex-direction:column; align-items:center; justify-content:center;` });

  if (sm.show_countdown) {
     let cd;
     if (pregame && pregameBlink) {
       cd = el('div', { id: 'mirror-countdown', style: `color:${unswap(sm.countdown_color) || '#fff'}; font-size:${fcd}px; font-weight:bold; text-align:center; line-height:1; width:100%; margin:0; white-space:nowrap; overflow:hidden;` });
       cd.dataset.maxFont = String(fcd);
       const text = sm.countdown || '';
       const cdNodes = buildCountdownContent(text);
       cd.append(...cdNodes);
     } else {
       cd = el('div', { id: 'mirror-countdown', style: `color:${unswap(sm.countdown_color) || '#fff'}; font-size:${fcd}px; font-weight:bold; text-align:center; line-height:1; width:100%; margin:0; white-space:nowrap; overflow:hidden;` }, browserCountdown(sm.countdown));
       cd.dataset.maxFont = String(fcd);
     }
     if (window._mirrorPregameBlinkIv) { clearInterval(window._mirrorPregameBlinkIv); window._mirrorPregameBlinkIv = null; }
     centerWrap.append(cd);
   } else if (window._mirrorPregameBlinkIv) {
     clearInterval(window._mirrorPregameBlinkIv); window._mirrorPregameBlinkIv = null;
   }
  if (sm.show_status && sm.status_text) {
    const st = el('div', { id: 'mirror-status-text', style: `color:${unswap(sm.status_color) || '#fff'}; font-size:${px(Math.round(basePace*0.8))}px; text-align:center; margin:${px(8)}px 0 0; width:100%; white-space:nowrap; overflow:hidden;` }, sm.status_text);
    st.dataset.maxFont = String(px(Math.round(basePace*0.8)));
    centerWrap.append(st);
  }
  if (showPace) {
    centerWrap.append(el('div', { style: `color:${unswap(sm.pace_color) || '#fff'}; font-size:${fp}px; text-align:center; margin-top:${px(18)}px;` }, sm.pace_text));
  }
  stage.append(centerWrap);

  if (showProgress && progressEndCount > 0 && progressGameDuration > 0) {
    // One pill, exactly like HDMI: fixed slot geometry inside the 1224px
    // inner track, a white background, and bands that grow from their left
    // edges. No dark overlay and no per-end bar stack.
    const pct = (v) => v / PROGRESS_TRACK_PX * 100;
    const progWrap = el('div', { id: 'mirror-progress', style: `position:absolute; left:${px(20)}px; right:${px(20)}px; bottom:${px(124)}px;` });
    const bar = el('div', { id: 'mirror-progress-bar', class: 'progress-end-bar', style: `width:100%; height:${px(95)}px; border-width:${px(8)}px; border-radius:${px(24)}px;` });
    const bandNames = ['lead', 'second', 'vice', 'skip'];
    for (let i = 0; i < 4; i++) {
      bar.append(el('div', { class: `progress-band ${bandNames[i]}`, 'data-band': String(i), style: `left:${pct(PROGRESS_BAND_GEOM[i][0])}%; width:0%;` }));
    }
    for (let i = 0; i < 3; i++) {
      bar.append(el('div', { class: 'progress-divider', 'data-div': String(i), style: `left:${pct(PROGRESS_DIV_X[i])}%; width:${pct(4)}%; display:none;` }));
    }
    progWrap.append(bar);
    stage.append(progWrap);
  }

  const footerRow = el('div', { style: `position:absolute; left:${padX}px; right:${padX}px; bottom:${px(4)}px; display:flex; justify-content:space-between; align-items:center;` });
  if (sm.show_date_time && sm.date_text) footerRow.append(el('div', { style: `color:${unswap(sm.footer_color) || '#fff'}; font-size:${ff}px; line-height:1;` }, sm.date_text)); else footerRow.append(el('div'));
  if (sm.show_game_name && sm.game_name) footerRow.append(el('div', { style: `color:${unswap(sm.footer_color) || '#fff'}; font-size:${ff}px; line-height:1; text-align:center; flex:1; padding:0 ${px(8)}px; overflow:hidden; text-overflow:ellipsis; white-space:nowrap;` }, sm.game_name)); else footerRow.append(el('div', { style: 'flex:1' }));
  if (sm.show_date_time && sm.time_text) footerRow.append(el('div', { style: `color:${unswap(sm.footer_color) || '#fff'}; font-size:${ff}px; line-height:1; text-align:right;` }, sm.time_text)); else footerRow.append(el('div'));
  if (footerRow.childElementCount) stage.append(footerRow);

  if (!stage.childElementCount) stage.append(el('div', { style: 'position:absolute;inset:0;display:flex;align-items:center;justify-content:center;color:#666;' }, '(display idle)'));
  mirror.append(stage);
  fitMirrorText();
  tick();
}

function renderQuickstart() {
  const root = $('tab-quickstart');
  if (!quickForm) quickForm = buildQuickForm();
  if (!root.contains(quickForm)) root.replaceChildren(quickForm);
  // Always render the form; disable/enable Start via update() rather than
  // blanking the page when status is missing or not yet ready.
  quickForm.classList.remove('hidden');
  const s = state.status;
  // Always call update() so applyDefaults() runs even without a status payload.
  if (s) quickForm.update(s.enabled && s.clock_synced, s.clock_synced);
  else quickForm.update(false, false);
}
function secondsNow() {
  const s = state.status;
  if (!s) return 0;
  const [, time] = s.now.split(' ');
  return parseTime(time) + Math.floor((performance.now() - state.statusAt) / 1000);
}

function tick() {
  const s = state.status;
  const box = $('countdown');
  if (box && s && s.current) {
    const left = s.current.seconds_until_buzzer - Math.floor((performance.now() - state.statusAt) / 1000);
    box.textContent = countdownText(left);
    box.style.color = left <= 0 ? 'var(--bad)' : 'var(--ok)';
  }
  const bar = $('mirror-progress-bar');
  if (bar && s && s.show_progress && s.progress_end_count && s.progress_game_duration) {
    // Mirror the HDMI lambda exactly: integer end duration, reset per end,
    // fixed slot widths, skip multiplier applied to the fill rate.
    const dur = Number(s.progress_game_duration);
    const endCount = Number(s.progress_end_count);
    const endDur = Math.max(1, Math.floor(dur / endCount));
    const skipRaw = Number(s.progress_skip_multiplier);
    const skipMult = skipRaw > 0 ? skipRaw : 1.4;
    const totalW = 3 + skipMult;
    const weights = [1, 1, 1, skipMult];
    const slotPct = PROGRESS_BAND_GEOM.map((g) => g[1] / PROGRESS_TRACK_PX * 100);
    const baseSince = (s.progress_seconds_since_start != null) ? Number(s.progress_seconds_since_start) : (s.current && s.current.seconds_since_start != null ? Number(s.current.seconds_since_start) : 0);
    const sinceStart = baseSince + Math.floor((performance.now() - state.statusAt) / 1000);
    const since = Math.max(0, Math.min(dur, sinceStart));
    const inEnd = since % endDur;
    let progress = inEnd * skipMult / endDur;
    progress = Math.max(0, Math.min(1, progress));
    const scaled = progress * totalW;
    let before = 0;
    for (let i = 0; i < 4; i++) {
      let frac = (scaled - before) / weights[i];
      frac = Math.max(0, Math.min(1, frac));
      before += weights[i];
      const band = bar.querySelector(`.progress-band[data-band="${i}"]`);
      if (band) band.style.width = `${frac * slotPct[i]}%`;
      if (i < 3) {
        const div = bar.querySelector(`.progress-divider[data-div="${i}"]`);
        if (div) div.style.display = scaled >= before ? '' : 'none';
      }
    }
  }
  const mc = $('mirror-countdown');
  if (mc && s) {
    if (s.is_pregame) {
      if (s.countdown) {
        const digits = (s.countdown || '').replace(/[^0-9]/g, '');
        const oldDigits = Array.from(mc.querySelectorAll('.cd-group')).map(g => g.textContent).join('');
        if (digits && digits !== oldDigits) {
          // Only rebuild when the numeric part changes (once per second max)
          const nodes = buildCountdownContent(s.countdown);
          mc.replaceChildren(...nodes);
        }
        if (s.pregame_blink) {
          // Flashing colon mode: spans exist, toggle colon opacity each second
          const colons = mc.querySelectorAll('.cd-colon');
          if (colons.length) {
            // Simple flashing: toggle colon opacity each second
            const toggle = Math.floor(performance.now() / 1000) % 2 === 0;
            for (const colon of colons) {
              colon.style.opacity = toggle ? '0' : '1';
            }
          }
        } else {
          // Normal mode: plain text, colon always visible
          mc.textContent = browserCountdown(s.countdown);
        }
      }
      return;
    }
    if (s.current && s.current.seconds_until_buzzer != null) {
      const left = s.current.seconds_until_buzzer - Math.floor((performance.now() - state.statusAt) / 1000);
      mc.textContent = countdownText(left);
    } else if (s.countdown) {
      mc.textContent = browserCountdown(s.countdown);
    }
  }
}

async function toggleEnabled(enabled) {
  if (!enabled && !confirm('Disable the timer? The display will show the disabled message and no buzzers will ring.')) return;
  const result = await api('/api/enabled', { enabled });
  if (!result.ok) return showMessage(result.data.error);
  showMessage(enabled ? 'Timer enabled' : 'Timer disabled', 'ok');
  state.model = null;
  loadStatus();
}

function durationText(total) {
  const m = Math.floor(total / 60), s = total % 60;
  return [m ? `${m} min` : '', s || !m ? `${s} s` : ''].filter(Boolean).join(' ');
}

// countdownText: mirrors the engine's H:MM:SS shape for the browser mirror.
// The engine uses U+02F8 on HDMI; the mirror uses ASCII ':' because browser
// monospace fonts do not include the raised-colon glyph.
function countdownText(total) {
  const t = Math.max(0, Math.floor(total));
  const h = Math.floor(t / 3600);
  const m = Math.floor((t % 3600) / 60);
  const s = t % 60;
  const colon = ':';
  if (h > 0)
    return h + colon + String(m).padStart(2, '0') + colon + String(s).padStart(2, '0');
  return String(m).padStart(2, '0') + colon + String(s).padStart(2, '0');
}

// fitMirrorText: adjusts mirror countdown font size to fit container.
// Respects the configured HDMI font size (set inline as font-size:${fcd}px
// and stored in dataset.maxFont) and scales the size so the countdown text
// fills the available width (grow or shrink proportionally). This keeps the
// full H:MM:SS countdown spanning the mirror width like the real HDMI output.
function fitMirrorText() {
  const mc = $('mirror-countdown');
  if (!mc) return;
  const maxFont = Number(mc.dataset.maxFont) || 0;
  if (!maxFont) return;
  const avail = Math.max(1, mc.clientWidth || 0);
  const text = (mc.textContent || '').trim();
  if (!text) return;
  // Measure at the configured size and scale to fill the available width.
  mc.style.fontSize = maxFont + 'px';
  const measured = mc.scrollWidth || 1;
  const size = maxFont * (avail / measured);
  const scaled = Math.max(24, Math.min(size, maxFont * 3));
  mc.style.fontSize = scaled + 'px';
}

function readQuickForm(minutesInput, nameInput) {
  const minutes = minutesInput.value === '' ? 0 : Number(minutesInput.value);
  if (!Number.isFinite(minutes) || minutes < 0) { showMessage('Game time must be a number of minutes (0 or more)'); return null; }
  const seconds = Math.round(minutes * 60);
  if (seconds < 1) { showMessage('Enter a game duration greater than 0 minutes'); return null; }
  return { seconds, name: nameInput.value.trim() || null };
}

async function launchQuick(payload, options = {}) {
  const { seconds, name, title } = payload;
  const ends = new Date(Date.now() + seconds * 1000);
  const called = title || name;
  const what = called ? `"${called}" (${durationText(seconds)})` : `a ${durationText(seconds)} timer`;
  if (!confirm(`Start ${what} now? Bell/buzzer at about ${pad(ends.getHours())}:${pad(ends.getMinutes())}:${pad(ends.getSeconds())}.`)) return;
  if (!state.configVersion) await loadConfig();
  const request = { ...payload, ...options, version: state.configVersion };
  delete request.title;  // display-only label for the confirmation dialog
  const result = await api('/api/quickstart', request);
  if (!result.ok) return showMessage(result.data.error);
  showMessage(`Quick start timer running, bell/buzzer at ${result.data.buzzer_time}`, 'ok');
  quickForm.reset();
  state.model = null;
  loadStatus();
  selectTab('status');
}

async function cancelQuick(current) {
  if (!confirm(`Cancel the quick start timer "${current.name || ''}"?`)) return;
  const result = await api('/api/quickstart/cancel', { start: current.start });
  if (!result.ok) return showMessage(result.data.error);
  showMessage('Quick start timer cancelled', 'ok');
  state.model = null;
  loadStatus();
}

// ---- schedule / exceptions editors ----------------------------------------
async function loadSettings() {
  const result = await api('/api/settings');
  if (!result.ok) { if (result.status !== 401) showMessage(result.data.error); return; }
  state.model = result.data;
  setDirty(false);
  const lr = await api('/api/leagues');
  state.leagues = (lr.ok && Array.isArray(lr.data)) ? lr.data : [];
  if (state.tab === 'schedule') renderSchedule();
}

function updateSavebars() {
  $('config-savebar').classList.toggle('show', state.configDirty);
  $('savebar').classList.toggle('show', state.dirty);
  $('savebar').style.bottom = state.configDirty ? `${$('config-savebar').offsetHeight}px` : '0px';
}

function setDirty(dirty) {
  state.dirty = dirty;
  updateSavebars();
}

function setConfigDirty(dirty) {
  state.configDirty = dirty;
  updateSavebars();
}

function newEntry(previous) {
  const start = previous ? Math.ceil((previous.buzzer + 25 * 60) / 60) * 60 : 9 * 3600;
  return { start: Math.min(start, 86340), buzzer: Math.min(start + 105 * 60, 86340), name: null,
           game_completed: null, header_hold: null, pace_of_play: null, pace_unit_duration: null, quick: false, league: null,
           pregame_countdown: null, end_count: null, stones_per_end: null, untimed_ends: null, show_progress_through_end: null, countdown_seconds_before: null, middle_relays: null };
}

// Helper: map toggle buttons to relay names by original index (filter reindexes).
function relayNamesFromToggles(toggles) { return toggles.map((b,i) => b.className.includes('on') ? RELAY_NAMES()[i] : null).filter(Boolean); }

// ---- Open Quick-Game-style modal for adding a scheduled game ----
function openScheduleGameModal(dayKey, entries, rerender) {
  // Build a local helper: collect enabled relay names from toggle buttons
  function collectRelays(toggles) { return toggles.map((b,i) => b.className.includes('on') ? RELAY_NAMES()[i] : null).filter(Boolean); }

  // --- timing ---
  const startIn = el('input', { type: 'time', step: 60, value: '19:00', placeholder: 'HH:MM' });
  // Buzzer time is computed from start time + duration, no clock input needed
  const nameIn = el('input', { type: 'text', maxlength: 80, placeholder: 'League / event name' });
  // League dropdown from existing leagues
  let selectedLeague = null;
  let customLeagueMode = false;
  const leagueSelect = el('select', { onchange: (event) => {
    const value = event.target.value;
    if (value === '__custom__') {
      customLeagueMode = true;
      selectedLeague = null;
      customLeagueNameInput.style.display = '';
      nameIn.value = '';
      presetRow.style.display = '';
    } else {
      customLeagueMode = false;
      customLeagueNameInput.style.display = 'none';
      const league = (state.leagues || []).find((l) => l.name === value);
      selectedLeague = league || null;
      if (league) {
        nameIn.value = league.name || '';
        if (league.end_count != null) endsIn.value = String(league.end_count);
        if (league.stones_per_end != null) stonesIn.value = String(league.stones_per_end);
        if (league.untimed_ends != null) untimedIn.value = String(league.untimed_ends);
        if (league.show_progress_through_end != null) {
          progressOn = !!league.show_progress_through_end;
          progressBtn.className = 'check-toggle ' + (progressOn ? 'on' : 'off');
        }
        if (league.pregame_countdown_seconds != null) {
          pregameOn = league.pregame_countdown_seconds > 0;
          pregameBtn.className = 'check-toggle ' + (pregameOn ? 'on' : 'off');
          pregameMinutesInput.value = formatMinutes(league.pregame_countdown_seconds);
          pregameMinutesInput._stepWrap.style.display = pregameOn ? '' : 'none';
        }
        if (league.game_duration_minutes != null) {
          durationMinutesInput.value = String(league.game_duration_minutes);
        }
        if (league.pace_enabled != null) { paceOn = !!league.pace_enabled; paceBtn.className = 'check-toggle ' + (paceOn ? 'on' : 'off'); }
        if (league.total_time_includes_untimed_ends != null) { untimedTotalOn = !!league.total_time_includes_untimed_ends; untimedTotalBtn.className = 'check-toggle ' + (untimedTotalOn ? 'on' : 'off'); }
        if (league.replace_conflicts != null) { replaceOn = !!league.replace_conflicts; replaceBtn.className = 'check-toggle ' + (replaceOn ? 'on' : 'off'); }
        if (league.manual_trigger != null) { manualOn = !!league.manual_trigger; manualBtn.className = 'check-toggle ' + (manualOn ? 'on' : 'off'); }
        // populate output rows
        populateOutputRows(league);
        presetRow.style.display = 'none';
      } else {
        nameIn.value = '';
        presetRow.style.display = '';
      }
    }
  } });
  leagueSelect.append(el('option', { value: '' }, '—'));
  ;(state.leagues || []).forEach((l) => { leagueSelect.append(el('option', { value: l.name }, l.name)); });
  leagueSelect.append(el('option', { value: '__custom__' }, 'Custom league…'));
  const customLeagueNameInput = el('input', { type: 'text', maxlength: 80, placeholder: 'Enter league name', style: 'display:none' });
  const endsIn = el('input', { type: 'number', min: 1, max: 12, value: '8', placeholder: 'ends' });
  const stonesIn = el('input', { type: 'number', min: 1, max: 10, value: '8', placeholder: 'stones' });
  const untimedIn = el('input', { type: 'number', min: 0, max: 6, value: '0', placeholder: 'extra ends' });
  const countdownSecondsBeforeInput = el('input', { type: 'number', min: 0, max: 3600, step: 60, inputmode: 'numeric', value: '0', placeholder: 'expiration seconds' });
  const durationMinutesInput = minuteInput({ max: 999, value: '0', 'aria-label': 'Game duration in minutes' });

  // --- option toggles ---
  let pregameOn = true, paceOn = false, progressOn = false, untimedTotalOn = false, replaceOn = false, manualOn = false;
  const pregameBtn = toggleBtn(pregameOn, 'Pre-Game Countdown');
  const paceBtn = toggleBtn(paceOn, 'Pace of play');
  const progressBtn = toggleBtn(progressOn, 'Show progress through end');
  const showProgressBtn = progressBtn; // alias for body row
  const untimedTotalBtn = toggleBtn(untimedTotalOn, 'Full game countdown (incl. extra ends)');
  const replaceBtn = toggleBtn(replaceOn, 'Replace conflicts');
  const manualBtn = toggleBtn(manualOn, 'Manual push-button trigger');
  const pregameMinutesInput = minuteInput({ max: 60, value: '0', placeholder: 'minutes', 'aria-label': 'Pre-game countdown in minutes' });
  pregameMinutesInput._stepWrap.style.display = 'none';
  // Blank means "use the configured/global hold"; only an explicit value is
  // written into the new entry.
  const warningIn = minuteInput({ max: 999, value: '', placeholder: 'default', 'aria-label': 'Countdown wording hold after buzzer in minutes' });
  pregameBtn.onclick = (e) => { pregameOn = !pregameOn; e.target.className = 'check-toggle ' + (pregameOn ? 'on' : 'off'); pregameMinutesInput._stepWrap.style.display = pregameOn ? '' : 'none'; };
  paceBtn.onclick = (e) => { paceOn = !paceOn; e.target.className = 'check-toggle ' + (paceOn ? 'on' : 'off'); };
  progressBtn.onclick = (e) => { progressOn = !progressOn; e.target.className = 'check-toggle ' + (progressOn ? 'on' : 'off'); };
  untimedTotalBtn.onclick = (e) => { untimedTotalOn = !untimedTotalOn; e.target.className = 'check-toggle ' + (untimedTotalOn ? 'on' : 'off'); };
  replaceBtn.onclick = (e) => { replaceOn = !replaceOn; e.target.className = 'check-toggle ' + (replaceOn ? 'on' : 'off'); };
  manualBtn.onclick = (e) => { manualOn = !manualOn; e.target.className = 'check-toggle ' + (manualOn ? 'on' : 'off'); };

  // --- output rows ---
  function makeOutRow(label) {
    const toggles = [];
    const durs = [];
    const wrapper = el('div', { class: 'row-actions', style: 'margin-bottom: 10px; gap: 10px; align-items: center' },
      el('span', { class: 'muted small', style: 'margin-right: 8px' }, label));
    if (RELAY_NAMES().length) {
      RELAY_NAMES().forEach((rn) => {
        const i = Number(rn.replace('RELAY', ''));
        const display = relayDisplayName(i) || rn;
        const btn = toggleBtn(false, display);
        const dur = secondInput({ max: 100000, placeholder: '0', value: '', 'aria-label': label + ' relay duration in seconds' });
        const wrap = el('div', { class: 'duration', style: 'display:none', gap: 6 }, dur, el('span', {}, 's'));
        btn.onclick = (e) => { const next = !btn.className.includes('on'); btn.className = 'check-toggle ' + (next ? 'on' : 'off'); wrap.style.display = next ? '' : 'none'; };
        wrapper.append(btn, wrap);
        toggles.push(btn); durs.push(dur);
      });
    } else wrapper.append(el('span', { class: 'muted small' }, '(no relay outputs enabled)'));
    return { row: wrapper, toggles, durs };
  }
  const startOut = makeOutRow('Countdown Clock Start:');
  const middleOut = makeOutRow('Countdown Clock Expiration:');
  const endOut = makeOutRow('Countdown Clock End:');
  function populateOutputRows(league) {
    if (!league) return;
    const setRow = (row, relays, durations) => {
      row.toggles.forEach((btn, idx) => {
        const i = Number(RELAY_NAMES()[idx].replace('RELAY', ''));
        const on = (relays || []).some((r) => String(r) === String(i) || String(r) === RELAY_NAMES()[idx]);
        const d = (durations || [])[idx];
        btn.className = 'check-toggle ' + (on ? 'on' : 'off');
        row.durs[idx].value = d != null ? formatSeconds(d) : '';
        // The input's direct parent is the +/- stepper wrapper; the outer
        // .duration row is what hides the whole field including its unit.
        row.durs[idx].closest('.duration').style.display = on ? '' : 'none';
      });
    };
    setRow(startOut, league.start_relays, [league.start_duration]);
    setRow(middleOut, league.middle_relays, [league.middle_duration]);
    setRow(endOut, league.end_relays, [league.end_duration]);
  }

  // --- presets row ---
  const presetRow = el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 6px' },
    el('button', { class: 'btn preset', type: 'button', onclick: () => { durationMinutesInput.value = '105'; } }, '8 ends'),
    el('button', { class: 'btn preset', type: 'button', onclick: () => { durationMinutesInput.value = '75'; } }, '6 ends'),
    el('button', { class: 'btn preset', type: 'button', onclick: () => { durationMinutesInput.value = '7'; } }, 'Practice'));

  // --- body ---
  const body = el('div', { class: 'modal-card', style: 'max-width:640px;width:100%;padding:20px' }, el('h3', {}, 'Add new game'),
    // --- league (first) ---
    el('div', { class: 'row-actions', style: 'margin: 12px 0; gap: 12px' },
      el('div', {}, el('label', {}, 'League'), leagueSelect),
      el('div', {}, customLeagueNameInput)),
    // --- presets (hidden when a league is chosen) ---
    presetRow,
    // --- Game option toggles ---
    el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px; flex-wrap: wrap' },
      pregameBtn, paceBtn, showProgressBtn, untimedTotalBtn, replaceBtn, manualBtn),
    // --- pregame countdown duration (shown when pregame is on) ---
    el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 6px', id: 'schedulePregameDuration' },
      el('span', { class: 'muted small' }, 'Pre-game countdown (minutes)'),
      pregameMinutesInput),

    // --- timing ---
    el('div', { class: 'row-actions', style: 'margin: 12px 0; gap: 12px' },
      el('div', { class: 'row-actions', style: 'margin: 4px 0; gap: 6px' },
        el('span', { class: 'muted small' }, 'Start time'),
        startIn
      ),
      el('div', { class: 'row-actions', style: 'margin: 4px 0; gap: 6px' },
        el('span', { class: 'muted small' }, 'Game duration (minutes)'),
        durationMinutesInput
      ),
      el('div', { class: 'row-actions', style: 'margin: 4px 0; gap: 6px' },
        el('span', { class: 'muted small' }, 'Countdown wording hold after buzzer (minutes)'),
        warningIn
      ),
    ),
    // --- stones and untimed ends ---
    el('div', { class: 'row-actions', style: 'margin: 12px 0; gap: 6px' },
      el('div', {}, el('label', {}, 'Stones per end'), stonesIn),
      el('div', {}, el('label', {}, 'Ends'), endsIn),
      el('div', {}, el('label', {}, 'Ends after expiration'), untimedIn),
      el('div', {}, el('label', {}, 'Countdown expiration (seconds)'), countdownSecondsBeforeInput)),
    // --- output rows ---
    el('div', {}, startOut.row),
    el('div', {}, middleOut.row),
    el('div', {}, endOut.row),
    el('div', { class: 'row-actions', style: 'margin-top: 20px' },
      el('button', { class: 'btn danger', onclick: () => {
        const stSec = parseTime(startIn.value) || 9 * 3600;
        const totalDurationSec = minutesToSeconds(durationMinutesInput.value) || 0;
        const buzzerSec = stSec + totalDurationSec;

        const league = selectedLeague || null;
        const leagueName = customLeagueMode ? (customLeagueNameInput.value.trim() || null) : (league ? league.name : null);
        const pregameSecs = pregameOn ? minutesToSeconds(pregameMinutesInput.value) : null;
        const startRelays = startOut.toggles.map((b, i) => b.className.includes('on') ? Number(RELAY_NAMES()[i].replace('RELAY', '')) : null).filter(n => n != null);
        const startDur = startOut.durs.length ? [secondsValue(startOut.durs[0].value) || 0] : [0];
        const endRelays = endOut.toggles.map((b, i) => b.className.includes('on') ? Number(RELAY_NAMES()[i].replace('RELAY', '')) : null).filter(n => n != null);
        const endDur = endOut.durs.length ? [secondsValue(endOut.durs[0].value) || 0] : [0];
        const middleRelays = middleOut.toggles.map((b, i) => b.className.includes('on') ? Number(RELAY_NAMES()[i].replace('RELAY', '')) : null).filter(n => n != null);
        const middleDur = middleOut.durs.length ? [secondsValue(middleOut.durs[0].value) || 0] : [0];
        entries.push({
          start: stSec,
          buzzer: buzzerSec,
          name: leagueName,
          game_completed: league ? league.header_text || null : null,
          header_hold: minutesToSeconds(warningIn.value),
          pace_of_play: paceOn ? true : null,
          pace_unit_duration: null,
          quick: false,
          league: leagueName,
          pregame_countdown: pregameSecs,
          end_count: Number(endsIn.value) || (league ? league.end_count: 8),
          stones_per_end: Number(stonesIn.value) || (league ? league.stones_per_end: 8),
          untimed_ends: Number(untimedIn.value) || (league ? league.untimed_ends: 0),
          countdown_seconds_before: secondsValue(countdownSecondsBeforeInput.value) || (league ? league.countdown_seconds_before : null),
          show_progress_through_end: progressOn,
          total_time_includes_untimed_ends: untimedTotalOn,
          replace_conflicts: replaceOn,
          manual_trigger: manualOn,
          start_relays: startRelays.length ? startRelays : (league ? league.start_relays : null),
          start_duration: startDur[0] || (league ? league.start_duration : null),
          middle_relays: middleRelays.length ? middleRelays : (league ? league.middle_relays : null),
          middle_duration: middleDur[0] || (league ? league.middle_duration : null),
          end_relays: endRelays.length ? endRelays : (league ? league.end_relays : null),
          end_duration: endDur[0] || (league ? league.end_duration : null),
        });
        setDirty(true);
        rerender();
        document.body.removeChild(modal);
        showMessage('Game added to schedule', 'ok');
      } }, 'Add game'),
      el('button', { class: 'btn primary', onclick: () => document.body.removeChild(modal) }, 'Cancel')
    )
  );

  const modal = el('div', { class: 'modal show' }, el('div', { class: 'modal-backdrop' }), body);
  document.body.append(modal);
}

function entryEditor(entries, index, leagues, rerender) {
  const entry = entries[index];
  const changed = () => setDirty(true);
  const timeInput = (key) => el('input', { type: 'time', step: 60, value: hms(entry[key]).slice(0, 5), required: true,
    onchange: (event) => { const v = parseTime(event.target.value); if (v !== null) { entry[key] = v; changed(); syncPace(); } } });
  const text = (key, placeholder) => el('input', { type: 'text', value: entry[key] || '', placeholder, maxlength: 80,
    oninput: (event) => { entry[key] = event.target.value.trim() || null; changed(); } });
  const hold = minuteInput({ max: 60, placeholder: 'default', 'aria-label': 'Countdown wording hold in minutes',
    value: entry.header_hold != null ? formatMinutes(entry.header_hold) : '',
    onchange: (event) => { entry.header_hold = minutesToSeconds(event.target.value); changed(); } });
  const leagueSelect = el('select', { onchange: (event) => {
    const league = (leagues || []).find((l) => l.name === event.target.value);
    if (!league) { entry.league = null; entry.name = null; changed(); rerender(); return; }
    entry.league = league.name;
    entry.name = league.name;
    entry.game_completed = league.header_text || null;
    if (league.game_duration_minutes != null && entry.start != null) entry.buzzer = Math.min(entry.start + (league.game_duration_minutes || 0) * 60, 86340);
    if (league.pregame_countdown_seconds != null) entry.pregame_countdown = league.pregame_countdown_seconds;
    if (league.stones_per_end != null) entry.stones_per_end = league.stones_per_end;
    if (league.end_count != null) entry.end_count = league.end_count;
    if (league.untimed_ends != null) entry.untimed_ends = league.untimed_ends;
    if (league.show_progress_through_end != null) entry.show_progress_through_end = league.show_progress_through_end;
    if (league.manual_trigger != null) entry.manual_trigger = !!league.manual_trigger;
    if (league.start_relays != null) entry.start_relays = league.start_relays;
    if (league.start_duration != null) entry.start_duration = league.start_duration;
    if (league.end_relays != null) entry.end_relays = league.end_relays;
    if (league.end_duration != null) entry.end_duration = league.end_duration;
    if (league.middle_relays != null) entry.middle_relays = league.middle_relays;
    if (league.middle_duration != null) entry.middle_duration = league.middle_duration;
    syncPace();
    changed(); rerender();
  } },
    el('option', { value: '', selected: !entry.league }, '(none)'),
    ...(leagues || []).map((l) => el('option', { value: l.name, selected: entry.league === l.name }, l.name)));

  // ---- Game option toggles (modern pill switches) ----
  let pgEnabled = entry.pregame_countdown != null && entry.pregame_countdown > 0;
  const pgBtn = toggleBtn(pgEnabled, 'Pre-Game Countdown');
  const pgTotal = (entry.pregame_countdown != null) ? Number(entry.pregame_countdown) : null;
  const pgMin = minuteInput({ max: 60, placeholder: 'default', 'aria-label': 'Pre-game countdown in minutes',
    value: pgTotal != null ? formatMinutes(pgTotal) : '' });
  pgMin.onchange = () => {
    entry.pregame_countdown = minutesToSeconds(pgMin.value);
    pgEnabled = entry.pregame_countdown != null;
    pgBtn.className = 'check-toggle ' + (pgEnabled ? 'on' : 'off');
    changed();
  };
  pgBtn.onclick = (event) => { const next = !pgEnabled; pgEnabled = next; event.target.className = 'check-toggle ' + (next ? 'on' : 'off'); pgFields.style.display = next ? 'flex' : 'none'; if (next && entry.pregame_countdown == null) entry.pregame_countdown = 0; else if (!next) entry.pregame_countdown = null; changed(); };
  const pgFields = el('div', { class: 'duration', role: 'group', 'aria-label': 'Pre-game countdown duration', style: pgEnabled ? '' : 'display:none' }, pgMin, el('span', {}, 'min'));

  const endCountInput = el('input', { type: 'number', min: 1, max: 12, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.end_count != null ? entry.end_count : '' });
  const syncPace = () => {
    const ends = endCountInput.value === '' ? null : Math.max(1, parseInt(endCountInput.value, 10) || 1);
    entry.end_count = ends;
    if (ends && entry.buzzer > entry.start) {
      entry.pace_unit_duration = Math.round((entry.buzzer - entry.start) / ends);
    } else {
      entry.pace_unit_duration = entry.pace_unit_duration ?? null;
    }
    changed();
  };
  endCountInput.onchange = (event) => { endCountInput.value = event.target.value; syncPace(); };

  let manualOn = !!entry.manual_trigger && isAddonEnabled('pushbutton');
  const manualBtn = toggleBtn(manualOn, 'Manual push-button trigger');
  manualBtn.onclick = (event) => { const next = !manualOn; manualOn = next; event.target.className = 'check-toggle ' + (next ? 'on' : 'off'); entry.manual_trigger = next; changed(); };

  let progressOn = entry.show_progress_through_end === true;
  const progressBtn = toggleBtn(progressOn, 'Show progress through end');
  const skipInput = el('input', { type: 'number', min: 0.1, max: 5, step: 0.1, inputmode: 'decimal', placeholder: '1.4',
    value: entry.skip_multiplier != null ? String(entry.skip_multiplier) : '1.4',
    oninput: (event) => { entry.skip_multiplier = Number(event.target.value) || null; changed(); } });
  const skipRow = el('div', { class: 'duration', style: progressOn ? '' : 'display:none', gap: 6 },
    el('span', { class: 'muted small' }, 'Skip multiplier'), skipInput, el('span', {}, 'x'));
  progressBtn.onclick = (event) => { const next = !progressOn; progressOn = next; event.target.className = 'check-toggle ' + (next ? 'on' : 'off'); skipRow.style.display = next ? '' : 'none'; entry.show_progress_through_end = next; if (next && entry.skip_multiplier == null) entry.skip_multiplier = 1.4; changed(); };
  skipInput.onchange = () => changed();

  let untimedTotalOn = entry.total_time_includes_untimed_ends === true;
  const untimedTotalBtn = toggleBtn(untimedTotalOn, 'Full game countdown (incl. extra ends)');
  untimedTotalBtn.onclick = (event) => { const next = !untimedTotalOn; untimedTotalOn = next; event.target.className = 'check-toggle ' + (next ? 'on' : 'off'); entry.total_time_includes_untimed_ends = next; changed(); };

  // ---- Notifications (relays) as toggle buttons instead of checkboxes ----
  const outputInfos = enabledOutputInfos();
  const buildNotificationRow = (which) => {
    const on = (r) => Array.isArray(entry[which + '_relays']) && entry[which + '_relays'].includes(r.id);
    const durField = (info, active) => secondInput({ max: 100000, placeholder: '0',
      'aria-label': `${labelOf(which)} relay duration in seconds`,
      value: active && entry[which + '_duration'] != null ? formatSeconds(entry[which + '_duration']) : '' });
    const row = el('div', { class: 'row-actions', style: 'margin-bottom: 8px; gap: 8px; align-items: center; flex-wrap: wrap' },
      el('span', { class: 'muted small', style: 'margin-right: 8px' }, labelOf(which)));
    outputInfos.forEach((info) => {
      const active = on(info);
      const btn = toggleBtn(active, info.label);
      const dur = durField(info, active);
      const durWrap = el('div', { class: 'duration', style: active ? '' : 'display:none', gap: 6 }, dur, el('span', {}, 's'));
      // Read current state from the DOM, not the captured `active` closure.
      btn.onclick = (event) => { const next = !btn.className.includes('on'); btn.className = 'check-toggle ' + (next ? 'on' : 'off'); durWrap.style.display = next ? '' : 'none'; setRelays(which, next ? [...((entry[which + '_relays'] || [])).concat([info.id])] : (entry[which + '_relays'] || []).filter((x) => x !== info.id)); changed(); };
      dur.oninput = (event) => { setDuration(which, secondsValue(event.target.value)); changed(); };
      row.append(btn);
      row.append(durWrap);
    });
    return row;
  };
  const setRelays = (which, list) => {
    const key = which + '_relays';
    entry[key] = list.length ? list : null;
    if (!entry[key]) entry[which + '_duration'] = null;
  };
  const setDuration = (which, value) => {
    entry[which + '_duration'] = value;
    if (!entry[which + '_relays'] || !entry[which + '_relays'].length) entry[which + '_duration'] = null;
  };
  const labelOf = (which) => which === 'start' ? 'Notifications — Countdown Clock Start' : 'Notifications — Countdown Clock End';

  const outputsCard = outputInfos.length ? el('div', { class: 'card', style: 'margin: 10px 0 0' },
      el('h3', {}, 'Notifications 🔔'),
      el('p', { class: 'muted small', style: 'margin: 0 0 8px' }, 'Choose which relay outputs ring at each event and for how many seconds. Tap a button to enable/disable; leave the duration blank to use the global buzzer duration.'),
      buildNotificationRow('start'),
      buildNotificationRow('buzzer'))
    : el('div', { class: 'card', style: 'margin: 10px 0 0' }, el('h3', {}, 'Notifications 🔔'), el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.'));

  return el('div', { class: 'entry' },
    el('div', { class: 'league' }, el('label', {}, 'League'), leagueSelect),
    el('div', { class: 'start' }, el('label', {}, 'Start'), timeInput('start')),
    el('div', { class: 'buzzer' }, el('label', {}, 'Countdown Clock End'), timeInput('buzzer')),
    el('div', { class: 'actions' },
      el('button', { class: 'btn icon danger', type: 'button', title: 'Delete game', onclick: () => { entries.splice(index, 1); changed(); rerender(); } }, '✕')),
    el('div', { class: 'name' }, text('name', 'League name'),
      entry.quick ? el('span', { class: 'pill quick', style: 'align-self: center' }, 'quick') : null),
    el('div', { class: 'more' },
      pgBtn, pgFields,
      el('label', {}, 'Header when game completed'), text('game_completed', 'default'),
      el('label', {}, 'Countdown wording hold after buzzer (minutes)'), hold,
      el('label', {}, 'Total ends'), endCountInput,
      el('label', {}, 'Stones per end'), el('input', { type: 'number', min: 1, max: 10, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.stones_per_end != null ? entry.stones_per_end : '', onchange: (event) => { entry.stones_per_end = event.target.value === '' ? null : Math.max(1, parseInt(event.target.value, 10) || 1); changed(); } }),
      el('label', {}, 'Ends after expiration'), el('input', { type: 'number', min: 0, max: 6, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.untimed_ends != null ? entry.untimed_ends : '', onchange: (event) => { entry.untimed_ends = event.target.value === '' ? null : Math.max(0, parseInt(event.target.value, 10) || 0); changed(); } }),
      manualBtn,
      progressBtn, skipRow,
      untimedTotalBtn,
      outputsCard));
}

function entryList(entries, leagues, rerender) {
  const box = el('div', {});
  if (!entries.length) box.append(el('p', { class: 'muted small' }, 'No buzzers this day.'));
  entries.forEach((_, index) => box.append(entryEditor(entries, index, leagues, rerender)));
  return box;
}

function foldCard({ openSet, key, title, badge = null, note = null, count, body }) {
  return el('details', { class: 'card fold', open: openSet.has(key),
      ontoggle: (event) => { if (event.target.open) openSet.add(key); else openSet.delete(key); } },
    el('summary', {},
      el('span', { class: 'chevron', 'aria-hidden': 'true' }, '▸'),
      el('span', { class: 'fold-date' }, title),
      badge,
      note || el('span', { class: 'fold-note' }),
      el('span', { class: 'fold-count muted' }, count ? `${count} game${count === 1 ? '' : 's'}` : 'No buzzers')),
    el('div', { class: 'fold-body' }, body));
}

function foldButtons(openSet, getKeys, rerender) {
  return [el('button', { class: 'btn', onclick: () => { getKeys().forEach((k) => openSet.add(k)); rerender(); } }, 'Expand all'),
          el('button', { class: 'btn', onclick: () => { openSet.clear(); rerender(); } }, 'Collapse all')];
}

function renderSchedule() {
  const m = state.model;
  if (!m) return;
  const todayWeekday = m.exceptions.some((d) => d.date === todayIso()) ? null : weekdayOf(todayIso());
  const cards = [
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Schedule')),
      el('span', { class: 'pill' }, `${DAYS.length} days · ${m.exceptions.length} exceptions`))
  ];
  if (m.inner_comments) cards.push(commentWarning(m.inner_comments));
  const calCard = el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Import'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Import games from an ICS calendar URL.'),
    el('div', { class: 'grid2' },
      el('label', {}, 'ICS URL', el('input', { type: 'url', placeholder: 'https://example.com/calendar.ics', id: 'ics-url' })),
      el('div', { class: 'field' }, el('label', {}, 'Action'),
        el('button', { class: 'btn primary', onclick: () => {
          const url = document.getElementById('ics-url').value.trim();
          if (!url) return showMessage('Enter an ICS URL');
          showMessage('Importing calendar…', 'ok');
        } }, 'Import calendar'))),
    el('div', { id: 'imported-events', class: 'muted small' }, 'No imported events yet.'));
  cards.push(calCard);
  cards.push(el('div', { class: 'card' },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Weekly schedule')),
      el('div', { class: 'row-actions', style: 'margin:0' }, foldButtons(state.openDays, () => DAYS, renderSchedule))),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Each day defines the default games. Date exceptions below replace a specific day.')));
  for (const day of DAYS) {
    const entries = m.schedule[day];
    const copyTo = el('select', { onchange: (event) => {
      const target = event.target.value;
      event.target.value = '';
      if (!target || !confirm(`Replace ${target}'s games with a copy of ${day}'s?`)) return;
      m.schedule[target] = structuredClone(entries);
      setDirty(true);
      renderSchedule();
    } }, el('option', { value: '' }, 'Copy to…'), DAYS.filter((d) => d !== day).map((d) => el('option', { value: d }, d)));
    // Add new game entry inline (instead of opening a modal)
    const addNewEntry = () => {
      entries.push({
        name: '', league: null, start: null, buzzer: null, game_completed: null,
        header_hold: null, pace_of_play: null, pace_unit_duration: null, quick: false,
        league_name: null, pregame_countdown: null,
        end_count: 8, stones_per_end: 8, untimed_ends: 0,
        show_progress_through_end: true, total_time_includes_untimed_ends: false,
        replace_conflicts: false, manual_trigger: false,
        start_relays: [], start_duration: 0, middle_relays: [], middle_duration: 0, end_relays: [], end_duration: 0
      });
      setDirty(true);
      renderSchedule();
    };
    cards.push(foldCard({ openSet: state.openDays, key: day, title: day, count: entries.length,
      badge: day === todayWeekday ? el('span', { class: 'pill ok' }, 'today') : null,
      body: [entryList(entries, state.leagues, renderSchedule),
        el('div', { class: 'row-actions' },
          el('button', { class: 'btn', onclick: addNewEntry }, '+ Add game'),
          el('span', { class: 'spacer' }), copyTo)] }));
  }
  const excContainer = el('div', { class: 'stack', style: 'margin-top: 4px' });
  state.exceptionsRoot = excContainer;
  renderExceptions();
  state.exceptionsRoot = null;
  cards.push(el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Exceptions'),
    el('h3', {}, 'Date-specific overrides'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'A date exception replaces that day\'s weekly schedule (use an empty day for no buzzers).')),
    excContainer);
  cards.push(el('div', { class: 'card' }, el('div', { class: 'section-title' }, 'Summary'),
    el('p', { class: 'muted small', style: 'margin: 0' }, `${DAYS.length} weekday(s), ${m.exceptions.length} exception(s) configured.`)));
  $('tab-schedule').replaceChildren(...cards);
}

function commentWarning(count) {
  return el('div', { class: 'banner warn' },
    `The settings file has ${count} comment line(s) inside the schedule/exceptions sections. ` +
    'Saving here removes them; use the Raw YAML tab if you need to keep them.');
}

function renderExceptions() {
  const m = state.model;
  if (!m) return;
  const today = todayIso();
  const date = el('input', { type: 'date', value: today, required: true });
  const note = el('input', { type: 'text', placeholder: 'Note, e.g. bonspiel name', maxlength: 80 });
  let prefillOn = true;
  const prefill = toggleBtn(prefillOn, 'Start from weekly schedule');
  prefill.onclick = (e) => { prefillOn = !prefillOn; e.target.className = 'check-toggle ' + (prefillOn ? 'on' : 'off'); };
  const add = el('form', { class: 'card', onsubmit: (event) => {
      event.preventDefault();
      if (m.exceptions.some((d) => d.date === date.value)) return showMessage(`${date.value} already has an exception`);
      const entries = prefillOn ? structuredClone(m.schedule[weekdayOf(date.value)]) : [];
      const day = { date: date.value, note: note.value.trim(), entries };
      const later = m.exceptions.findIndex((d) => d.date > day.date);
      m.exceptions.splice(later < 0 ? m.exceptions.length : later, 0, day);
      state.openDates.add(day.date);
      setDirty(true);
      renderExceptions();
    } },
    el('div', { class: 'section-title' }, 'Add exception'),
    el('h3', {}, 'Create a date override'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Replace one day\'s weekly schedule. Leave it with no games for a day without buzzers.'),
    el('div', { class: 'grid2' }, el('label', {}, 'Date', date), el('label', {}, 'Note', note)),
    el('div', { class: 'row-actions' }, prefill,
      el('button', { class: 'btn primary', type: 'submit' }, 'Add date')));

  const past = m.exceptions.filter((d) => d.date < today);
  let shown = [];
  const tools = el('div', { class: 'card', style: 'padding: 14px' },
    el('div', { class: 'row-actions', style: 'margin: 0' },
      (() => { const b = toggleBtn(state.showPast, 'Show past dates'); b.onclick = (e) => { state.showPast = !state.showPast; b.className = 'check-toggle ' + (state.showPast ? 'on' : 'off'); renderExceptions(); }; return b; })(),
      el('button', { class: 'btn danger', disabled: !past.length, onclick: () => {
        if (!confirm(`Remove ${past.length} past date(s)?`)) return;
        m.exceptions = m.exceptions.filter((d) => d.date >= today);
        setDirty(true);
        renderExceptions();
      } }, 'Remove past dates'),
      el('span', { class: 'spacer' }),
      foldButtons(state.openDates, () => shown.map((d) => d.date), renderExceptions)));

  const cards = [add, tools];
  if (m.inner_comments) cards.unshift(commentWarning(m.inner_comments));
  shown = [...m.exceptions].filter((d) => state.showPast || d.date >= today).sort((a, b) => a.date.localeCompare(b.date));
  if (!shown.length) cards.push(el('div', { class: 'card' }, el('p', { class: 'muted', style: 'margin:0' }, 'No upcoming date exceptions.')));
  for (const day of shown) {
    const noteText = el('span', { class: 'fold-note muted' }, day.note);
    cards.push(foldCard({ openSet: state.openDates, key: day.date, title: `${weekdayOf(day.date)} ${day.date}`,
      badge: day.date === today ? el('span', { class: 'pill ok' }, 'today') : null, note: noteText, count: day.entries.length,
      body: [
        el('input', { type: 'text', value: day.note, placeholder: 'Note', maxlength: 80, style: 'width: 100%; margin-bottom: 4px',
          oninput: (event) => { day.note = event.target.value.trim(); noteText.textContent = day.note; setDirty(true); } }),
        entryList(day.entries, state.leagues, renderExceptions),
        el('div', { class: 'row-actions' },
          el('button', { class: 'btn', onclick: () => {
            day.entries.push({
              name: '', league: null, start: null, buzzer: null, game_completed: null,
              header_hold: null, pace_of_play: null, pace_unit_duration: null, quick: false,
              league_name: null, pregame_countdown: null,
              end_count: 8, stones_per_end: 8, untimed_ends: 0,
              show_progress_through_end: true, total_time_includes_untimed_ends: false,
              replace_conflicts: false, manual_trigger: false,
              start_relays: [], start_duration: 0, middle_relays: [], middle_duration: 0, end_relays: [], end_duration: 0
            });
            setDirty(true); renderExceptions(); } }, '+ Add game'),
          el('span', { class: 'spacer' }),
          el('button', { class: 'btn danger', title: 'Delete this date', onclick: () => {
            if (!confirm(`Delete the exception for ${day.date}? The weekly schedule will apply again.`)) return;
            m.exceptions.splice(m.exceptions.indexOf(day), 1); state.openDates.delete(day.date); setDirty(true); renderExceptions(); } }, 'Delete date'))] }));
  }
  const target = state.exceptionsRoot || $('tab-exceptions');
  target.replaceChildren(...cards);
}

// ---- config tab (every parameter that is not the schedule) ----------------
const CONFIG_SECTIONS = [
  { group: 'Global Settings', title: 'Timer', hint: 'Master on/off for the entire timer. When off, the display shows the disabled header text and stops counting down. Changes are written to the settings file only when you press Save & apply.',
    fields: [
      ['config.enabled', 'bool', 'Timer enabled (master on/off)'],
      ['config.allow_midnight_wrap', 'bool', 'Allow timers past midnight']] },
  { group: 'Global Settings', title: 'Manual trigger (pushbutton)', hint: 'GPIO press rings the start relays for the next manually-triggered game. Only shown when the pushbutton add-on is enabled.',
    fields: [
      ['config.manual_trigger_gpio', 'int', 'Manual trigger GPIO (pushbutton add-on)', 0, 40]] },
  { group: 'Global Settings', title: 'Club Website URL', hint: 'Base website URL of the club (Curling Club Manager). Used to build league/import paths.',
    fields: [
      ['config.club_website_url', 'text', 'Club website URL (Curling Club Manager)']] },
{ group: 'Global Settings', title: 'League Page URL', hint: 'Optional override URL for the league page.',
    fields: [
      ['config.league_page_url', 'text', 'League page URL (optional override)']] },
  { group: 'Global Settings', title: 'Relays', hint: 'How many relays are active and what each is mapped to.',
    fields: [
      ['config.relay_count', 'int', 'Number of relays', 0, 4],
      ['config.relay1_enabled', 'bool', 'RELAY1 enabled'],
      ['config.relay1_function', 'select', 'RELAY1 function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom'],
      ['config.relay2_enabled', 'bool', 'RELAY2 enabled'],
      ['config.relay2_function', 'select', 'RELAY2 function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom'],
      ['config.relay3_enabled', 'bool', 'RELAY3 enabled'],
      ['config.relay3_function', 'select', 'RELAY3 function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom'],
      ['config.relay4_enabled', 'bool', 'RELAY4 enabled'],
      ['config.relay4_function', 'select', 'RELAY4 function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom']] },
  { group: 'Global Settings', title: 'Live Display Scaling', hint: 'Text scaling for the on-display mirror card.',
    fields: [
      ['config.mirror_scale', 'slider', '', 10, 100, 5]] },
  { group: 'Display', title: 'Background', fields: [ ['config.background_color', 'color', 'Background colour'] ] },
  { group: 'Display', title: 'Pre-game countdown', hint: 'Countdown displayed before the game starts. Set minutes to 0 to disable.',
    fields: [
      ['config.pregame_header_text', 'text', 'Header text (e.g. Get ready)'],
      ['config.pregame_header_color', 'color', 'Header colour'],
      ['config.pregame_countdown', 'minutes', 'Countdown (minutes)', 0, 60],
      ['config.pregame_countdown_color', 'color', 'Countdown colour'],
      ['config.pregame_font_size', 'int', 'Font size', 0, 100000],
      ['config.pregame_blink', 'bool', 'Countdown colon flashes each second']] },
  { group: 'Display', title: 'Header text',
    fields: [
      ['config.header.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.header.text.when_disabled', 'text', 'Text when disabled'],
      ['config.header.text.when_idle', 'text', 'Text when idle'],
      ['config.header.text.counting_down', 'text', 'Text when Countdown'],
      ['config.header.text.game_completed', 'text', 'Text when Countdown Expiration/Output'],
      ['config.header.text.game_ended', 'text', 'Text when Countdown Clock End'],
      ['config.header.color.when_disabled', 'color', 'Colour when disabled'],
      ['config.header.color.when_idle', 'color', 'Colour when idle'],
      ['config.header.color.counting_down', 'color', 'Colour while counting down'],
      ['config.header.color.game_completed', 'color', 'Colour while the buzzer rings'],
      ['config.header.color.game_ended', 'color', 'Colour when game ends']] },
  { group: 'Display', title: 'Countdown clock',
    fields: [
      ['config.countdown.font_size_4_digits', 'int', 'Timer font size under an hour', 0, 100000],
      ['config.countdown.font_size_5_digits', 'int', 'Timer font size an hour or more', 0, 100000],
      ['config.countdown.color.counting_down', 'color', 'Colour while counting down'],
      ['config.countdown.color.game_completed', 'color', 'Colour while the buzzer rings'],
      ['config.countdown.color.game_ended', 'color', 'Colour when game ends']] },
  { group: 'Display', title: 'Pace of play',
    fields: [
      ['config.pace_of_play.enabled', 'bool', 'Show pace of play'],
      ['config.pace_of_play.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.pace_of_play.color', 'color', 'Colour'],
      ['config.pace_of_play.unit_name', 'text', 'Unit name, e.g. END']] },
  { group: 'Display', title: 'Footer',
    fields: [
      ['config.footer.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.footer.datetime', 'bool', 'Show date and time'],
      ['config.footer.entry_name', 'bool', 'Show entry name'],
      ['config.footer.color', 'color', 'Colour']] },
  { group: 'Output Configuration', title: 'Countdown Clock Start',
    hint: 'Relay output durations are in seconds.',
    fields: [
      ['config.buzzer.ring_at_start', 'relays', 'Output'],
      ['config.buzzer.start_duration', 'output_duration', 'Duration', 'start']] },
  { group: 'Output Configuration', title: 'Countdown Clock Expiration',
    hint: 'Duration before countdown expiration fires this many minutes before game end. Time before buzzer counts backwards from game end. 0 disables the countdown expiration. Relay output durations are in seconds.',
    fields: [
      ['config.buzzer.countdown.mode', 'select', 'Countdown mode', COUNTDOWN_MODE_DURATION, COUNTDOWN_MODE_BUZZER],
      ['config.buzzer.countdown.seconds_before', 'minutes', 'Countdown expiration time (minutes)', 0, 1666],
      ['config.buzzer.countdown.relays', 'relays', 'Output'],
      ['config.buzzer.countdown_duration', 'output_duration', 'Duration', 'countdown']] },
  { group: 'Output Configuration', title: 'Countdown Clock End',
    hint: 'Countdown wording hold controls how long the countdown expiration stays after the buzzer rings. The wording itself is set under Display → Header text → Text when Countdown Expiration/Output. Relay output durations are in seconds.',
    fields: [
      ['config.buzzer.header_hold', 'minutes', 'Countdown wording hold after buzzer (minutes)', 0, 1666],
      ['config.buzzer.relays', 'relays', 'Output'],
      ['config.buzzer.end_duration', 'output_duration', 'Duration', 'end']] },
];
const CONFIG_PATHS = CONFIG_SECTIONS.flatMap((section) => section.fields.map((field) => field[0]));
// Per-output durations are rendered dynamically (one per selected relay) under
// numbered paths; register them all so they are diffed and saved.
for (const ev of ['start', 'countdown', 'end']) {
  for (let n = 1; n <= 4; n++) CONFIG_PATHS.push(`config.buzzer.${ev}_duration_${n}`);
}
function relayDisplayName(i) {
  const enabled = state.configValues && state.configValues[`config.relay${i}_enabled`];
  if (!enabled) return null;
  const fn = (state.configValues && state.configValues[`config.relay${i}_function`]) || 'None';
  if (fn === 'None') return null;
  return fn + i;
}
function RELAY_NAMES() {
  const count = (state.configValues && state.configValues['config.relay_count']) || 0;
  const names = [];
  for (let i = 1; i <= count; i++) {
    const enabled = state.configValues && state.configValues[`config.relay${i}_enabled`];
    if (enabled) names.push(`RELAY${i}`);
  }
  return names;
}
function enabledOutputInfos() {
  const count = (state.configValues && state.configValues['config.relay_count']) || 0;
  const infos = [];
  for (let i = 1; i <= count; i++) {
    if (!(state.configValues && state.configValues[`config.relay${i}_enabled`])) continue;
    const fn = (state.configValues && state.configValues[`config.relay${i}_function`]) || 'None';
    const label = fn && fn !== 'None' ? `${fn} (RELAY${i})` : `RELAY${i}`;
    infos.push({ id: `RELAY${i}`, label, index: i });
  }
  return infos;
}
const RELAYS_PATHS = new Set(['config.buzzer.relays', 'config.buzzer.ring_at_start',
  'config.buzzer.countdown.relays']);
const PYGAME_COLORS = ['black', 'white', 'red', 'green', 'blue', 'yellow', 'cyan', 'magenta', 'gray', 'grey', 'orange', 'purple', 'pink', 'brown', 'navy', 'teal', 'lime', 'indigo', 'violet', 'gold', 'silver', 'maroon', 'olive', 'aqua', 'fuchsia', 'silver', 'darkred', 'darkgreen', 'darkblue', 'lightgray', 'lightgrey',
  'navy', 'skyblue', 'wheat', 'orange', 'purple', 'brown', 'pink', 'lime', 'teal', 'olive', 'maroon', 'silver',
  'gold', 'indigo', 'violet', 'coral', 'salmon', 'tan', 'beige', 'ivory', 'khaki', 'plum', 'azure', 'snow',
  'crimson', 'darkgreen', 'darkblue', 'darkred', 'lightblue', 'lightgreen', 'dgray', 'lgray'];

function configNorm(path, value) {
  if (RELAYS_PATHS.has(path)) return Array.isArray(value) ? value : [];
  if (value == null) return '';
  return value;
}
function configDiff() {
  const diff = {};
  for (const path of CONFIG_PATHS) {
    let value = state.configValues[path];
    if (typeof value === 'string') value = value.trim();
    const before = configNorm(path, state.configOriginal[path]);
    const after = configNorm(path, value);
    const equal = RELAYS_PATHS.has(path)
      ? JSON.stringify(before) === JSON.stringify(after)
      : before === after;
    if (!equal) diff[path] = value;
  }
  return diff;
}
function configTouched() { setConfigDirty(Object.keys(configDiff()).length > 0); }

function validateConfigDiff(diff) {
  for (const [path, value] of Object.entries(diff)) {
    if (path === 'config.pace_of_play.unit_duration') {
      if (typeof value !== 'number' || !Number.isFinite(value) || value <= 0)
        return 'Minutes per unit must be a number greater than 0';
    } else if (path === 'config.mirror_scale') {
      if (typeof value !== 'number' || !Number.isInteger(value) || value < 10 || value > 100)
        return 'Display mirror scale must be an integer between 10 and 100 (percentage)';
    } else if (path === 'config.buzzer.countdown.seconds_before') {
      if (typeof value !== 'number' || !Number.isInteger(value) || value < 0 || value > 100000)
        return 'Countdown expiration time must resolve to a whole number of seconds (0 or more)';
    } else if (path.endsWith('.font_size') || path.endsWith('.font_size_4_digits') ||
                path.endsWith('.font_size_5_digits') || path === 'config.buzzer.duration' ||
                path === 'config.buzzer.header_hold' ||
                /config\.buzzer\.(start|countdown|end)_duration_[1-4]$/.test(path)) {
      if (typeof value !== 'number' || !Number.isInteger(value) || value < 0 || value > 100000)
        return `${path} must be a whole number of seconds or pixels (0 or more)`;
    }
  }
  return null;
}

async function loadConfig() {
  const result = await api('/api/config');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  const config = normalizeConfigPayload(result.data.config || {});
  state.configOriginal = config;
  state.configValues = JSON.parse(JSON.stringify(config));
  state.configVersion = result.data.version;
  setConfigDirty(false);
  if (state.tab === 'config') await renderConfig();
}

function configField(field) {
  const [path, type, label, min, max] = field;
  const value = state.configValues[path];
  const set = (next) => { state.configValues[path] = next; configTouched(); };

  // Gate fields by add-on enablement: if the field belongs to a disabled
  // add-on, render it as disabled with a hint so the user knows why.
  const addonKey = PATH_TO_ADDON[path];
  if (addonKey && !isAddonEnabled(addonKey)) {
    return el('div', { class: 'field' }, el('label', {}, label,
      el('span', { class: 'pill notdetected', style: 'margin-left: 8px; font-size: 10px' }, 'add-on disabled')));
  }

  if (type === 'bool') {
    let on = value === true;
    const btn = toggleBtn(on, label);
    btn.onclick = (e) => { on = !on; btn.className = 'check-toggle ' + (on ? 'on' : 'off'); state.configValues[path] = on; configTouched(); };
    return el('div', { class: 'field' }, el('label', {}, btn));
  }
  if (type === 'int') {
    let width = '';
    if (path === 'config.relay_count') width = '40px';
    else if (path.endsWith('.duration') || path === 'config.buzzer.header_hold' || path.endsWith('header_hold')) width = '7ch';
    return el('label', {}, label, el('input', { type: 'number', min, max, step: 1, inputmode: 'numeric',
      style: width ? `width: ${width}` : '',
      value: value == null ? '' : String(value),
      oninput: (event) => set(event.target.value === '' ? null : Number(event.target.value)) }));
  }
  if (type === 'output_duration') {
    const event = field[3]; // 'start' | 'countdown' | 'end'
    const relaysPath = event === 'countdown' ? 'config.buzzer.countdown.relays' :
      event === 'start' ? 'config.buzzer.ring_at_start' : 'config.buzzer.relays';
    const selected = Array.isArray(state.configValues[relaysPath]) ? state.configValues[relaysPath] : [];
    const inputs = selected.map((name) => {
      const i = Number(name.replace('RELAY', ''));
      const display = relayDisplayName(i) || name;
      const path = `config.buzzer.${event}_duration_${i}`;
      const val = state.configValues[path];
      return el('div', { class: 'field' },
        el('label', {}, `${display} duration (seconds)`,
          secondInput({ max: 100000, placeholder: '0', 'aria-label': `${display} duration in seconds`,
            value: val == null ? '' : formatSeconds(val),
            oninput: (event) => { state.configValues[path] = secondsValue(event.target.value); configTouched(); } })));
    });
    if (selected.length === 0) inputs.push(el('p', { class: 'muted small' }, 'No outputs selected.'));
    return el('div', {}, inputs);
  }
  if (type === 'text') {
    const isUrl = path === 'config.club_website_url' || path === 'config.league_page_url';
    return el('label', {}, label, el('input', { type: 'text', maxlength: isUrl ? 200 : 80, value: value == null ? '' : value,
      placeholder: isUrl ? 'https://example.com/club' : 'not set', style: isUrl ? 'width:100%; box-sizing:border-box;' : '',
      oninput: (event) => set(event.target.value) }));
  }
  if (type === 'select') {
    const options = field.slice(3).filter((x) => typeof x === 'string');
    return el('label', {}, label, el('select', { value: value == null ? '' : value,
      onchange: (event) => set(event.target.value) },
      options.map((opt) => el('option', { value: opt, selected: value === opt }, opt))));
  }
  if (type === 'color') {
    const colorOpts = PYGAME_COLORS;
    return el('label', {}, label, el('select', { value: value == null ? '' : value,
      onchange: (event) => set(event.target.value) },
      [el('option', { value: '', selected: value == null || value === '' }, 'not set')]
        .concat(colorOpts.map((opt) => el('option', { value: opt, selected: value === opt }, opt)))));
  }
  if (type === 'number') {
    return el('label', {}, label, el('input', { type: 'number', min, max, step: 0.05, inputmode: 'decimal',
      value: value == null ? '' : String(value),
      oninput: (event) => set(event.target.value === '' ? null : Number(event.target.value)) }));
  }
  if (type === 'slider') {
    const [,, , min, max, step = 0.05] = field;
    const val = value == null ? (min + max) / 2 : Number(value);
    const out = el('output', { class: 'muted small', style: 'margin-left:8px' }, String(val) + '%');
    return el('label', {}, label,
      el('div', { class: 'row-actions', style: 'gap:8px' },
        el('input', { type: 'range', min, max, step, style: 'flex:1',
          value: val,
          oninput: (event) => { const v = Number(event.target.value); out.textContent = String(v) + '%'; set(v); } }),
        out));
  }
  if (type === 'minutes') {
    return el('label', {}, label, minuteInput({ min, max, 'aria-label': label,
      value: value == null ? '' : formatMinutes(value),
      oninput: (event) => set(minutesToSeconds(event.target.value)) }));
  }
   if (type === 'relays') {
     const selected = Array.isArray(value) ? value : [];
     return el('div', { class: 'field' }, label, el('div', { class: 'row-actions', style: 'margin-top: 2px' },
       RELAY_NAMES().map((name) => {
         const i = Number(name.replace('RELAY', ''));
         const display = relayDisplayName(i) || name;
         const checked = selected.includes(name);
          const btn = toggleBtn(checked, display);
          btn.onclick = () => {
            const list = Array.isArray(state.configValues[path]) ? [...state.configValues[path]] : [];
            const at = list.indexOf(name);
            if (at < 0) list.push(name);
            else list.splice(at, 1);
            // Keep per-event duration values when a relay is unchecked:
            // deleting them makes configDiff emit `undefined`, which fails
            // numeric validation and blocks the whole save. Unused durations
            // are ignored by the engine and restored if the relay is selected
            // again.
            set(list);
            btn.className = 'check-toggle ' + (list.includes(name) ? 'on' : 'off');
          };
          return btn;
       })));
   }
  return el('div', {}, label);
}

async function testRelay(eventName, btn) {
  try {
    const result = await api('/api/test_relay', { event: eventName });
    if (btn) {
      const dur = (result.data && result.data.duration) ? result.data.duration : 0;
      if (dur <= 0) {
        btn.textContent = result.ok ? '✓' : '✗';
        btn.style.background = result.ok ? 'var(--ok-dark)' : 'var(--bad)';
        setTimeout(() => { btn.textContent = 'Test'; btn.style.background = ''; }, 1000);
      } else {
        btn.classList.add('primary');
        btn.style.background = result.ok ? 'var(--ok-dark)' : 'var(--bad)';
        let remaining = dur;
        btn.textContent = remaining;
        const iv = setInterval(() => {
          remaining -= 1;
          if (remaining <= 0) {
            clearInterval(iv);
            btn.textContent = 'Test';
            btn.classList.remove('primary');
            btn.style.background = '';
          } else {
            btn.textContent = remaining;
          }
        }, 1000);
      }
    }
    if (!result.ok) showMessage(`Test failed: ${result.data.error}`);
  } catch(e) { console.error(e); if (btn) { btn.textContent = '✗'; btn.style.background = 'var(--bad)'; } }
}

async function renderConfig() {
  if (!state.configValues) return;
  // Load add-on detection before rendering fields so gating is correct
  // on first render (not just after the marketplace re-renders).
  const detectionResult = await api('/api/addons/detection');
  if (detectionResult.ok) addonDetection = detectionResult.data || {};
  if (!document.getElementById('pygame-colors')) {
    document.body.append(el('datalist', { id: 'pygame-colors' },
      PYGAME_COLORS.map((color) => el('option', { value: color }))));
  }
  const nodes = [];
  let currentGroup = null;
  for (const section of CONFIG_SECTIONS) {
    if (section.title === 'Manual trigger (pushbutton)' && !isAddonEnabled('pushbutton'))
      continue;
    if (section.group !== currentGroup) {
      currentGroup = section.group;
       nodes.push(el('div', { class: 'section-title', style: 'margin: 8px 0 0; font-size: 12px' }, currentGroup));
    }
    let fields = section.fields;
    if (section.title === 'Relays') {
      const count = state.configValues['config.relay_count'] || 0;
      const relayFields = [];
      for (let i = 1; i <= count; i++) {
        relayFields.push(['config.relay' + i + '_enabled', 'bool', 'RELAY' + i + ' enabled']);
        relayFields.push(['config.relay' + i + '_function', 'select', 'RELAY' + i + ' function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom']);
      }
      fields = [['config.relay_count', 'int', 'Number of relays', 0, 4]].concat(relayFields);
    }
    const isPregameSection = section.title === 'Pre-game countdown';
    let fieldNodes;
    if (isPregameSection) {
      fieldNodes = [
        // Row 1: header text + header color
        el('div', { style: 'display:flex; flex-wrap:wrap; gap:8px; margin:6px 0' },
          ...[fields[0], fields[1]].map(configField)),
        // Row 2: countdown + countdown color + font size
        el('div', { style: 'display:flex; flex-wrap:wrap; gap:6px; margin:6px 0' },
          ...[fields[2], fields[3], fields[4]].map(configField)),
        // Row 3: pillbox only
        el('div', { style: 'display:flex; flex-wrap:wrap; gap:6px; align-items:center; margin:6px 0' },
          configField(fields[5]))
      ];
    } else if (section.title === 'Pace of play') {
      // Pace of play: enabled (row1), font size + color (row2), unit name (row3)
      fieldNodes = [
        el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px' }, configField(fields[0])),
        el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px' }, configField(fields[1]), configField(fields[2])),
        el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px' }, configField(fields[3]))
      ];
    } else if (section.title === 'Footer') {
      // Footer: font size + color (row1), datetime + entry name (row2)
      fieldNodes = [
        el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px' }, configField(fields[0]), configField(fields[3])),
        el('div', { class: 'row-actions', style: 'margin: 6px 0; gap: 8px' }, configField(fields[1]), configField(fields[2]))
      ];
    } else if (section.group.startsWith('Output Configuration')) {
      // Non-relay settings first (mode, expiration time, header hold, ...).
      // Relay options and their per-output durations always come last.
      fieldNodes = [];
      for (const f of fields) {
        if (f[1] !== 'relays' && f[1] !== 'output_duration') fieldNodes.push(configField(f));
      }
      for (const f of fields) {
        if (f[1] !== 'relays') continue;
        const addonKey = PATH_TO_ADDON[f[0]];
        if (addonKey && !isAddonEnabled(addonKey)) {
          fieldNodes.push(el('div', { class: 'field' }, el('label', {}, f[2],
            el('span', { class: 'pill notdetected', style: 'margin-left: 8px; font-size: 10px' }, 'add-on disabled'))));
          continue;
        }
        const relaysPath = f[0].startsWith('config.buzzer.countdown') ? 'config.buzzer.countdown.relays' :
          f[0].startsWith('config.buzzer.ring_at_start') ? 'config.buzzer.ring_at_start' : 'config.buzzer.relays';
        const selected = Array.isArray(state.configValues[relaysPath]) ? state.configValues[relaysPath] : [];
        // Find the matching output_duration field to derive its per-relay path prefix
        const durField = fields.find((d) => d[1] === 'output_duration');
        const event = durField ? durField[3] : null;
        const rows = [];
        for (const info of enabledOutputInfos()) {
          const checked = selected.includes(info.id);
          const btn = toggleBtn(checked, info.label);
          btn.onclick = () => {
            const list = Array.isArray(state.configValues[f[0]]) ? [...state.configValues[f[0]]] : [];
            const at = list.indexOf(info.id);
            if (at < 0) list.push(info.id);
            else list.splice(at, 1);
            // Keep per-event duration values when a relay is unchecked:
            // deleting them would make configDiff emit `undefined`, which
            // fails numeric validation and blocks the whole save. Unused
            // durations are ignored by the engine, and are restored if the
            // relay is selected again.
            state.configValues[f[0]] = list;
            configTouched();
            btn.className = 'check-toggle ' + (list.includes(info.id) ? 'on' : 'off');
          };
          // Duration input for this specific relay (same row as the pillbox)
          let dur = null;
          if (event) {
            const dp = `config.buzzer.${event}_duration_${info.index}`;
            const dv = state.configValues[dp];
            dur = secondInput({ max: 100000, 'aria-label': `${info.label} duration in seconds`,
              value: dv == null ? '' : formatSeconds(dv),
              oninput: (e) => { state.configValues[dp] = secondsValue(e.target.value); configTouched(); } });
          }
          rows.push(el('div', { style: 'display:inline-grid; grid-template-columns:auto auto; align-items:center; gap:8px; margin:4px 8px 4px 0' },
            btn, dur ? el('span', { class: 'muted small', style: 'display:inline-flex; align-items:center; gap:4px' }, dur, 's') : null));
        }
        if (selected.length === 0) rows.push(el('p', { class: 'muted small' }, 'No outputs selected.'));
        const groupLabelId = 'relay-group-' + f[0].replace(/[^a-zA-Z0-9]/g, '-');
        fieldNodes.push(el('div', { class: 'field', style: 'margin-top: 8px' },
          el('div', { id: groupLabelId, style: 'font-weight:600' }, f[2]),
          el('div', { role: 'group', 'aria-labelledby': groupLabelId,
            style: 'display:flex; flex-wrap:wrap; gap:4px' }, rows)));
      }
    } else {
      fieldNodes = fields.map(configField);
    }
    const noGrid = isPregameSection || section.title === 'Live Display Scaling' ||
      section.group.startsWith('Output Configuration');
    nodes.push(el('div', { class: 'card' },
      el('div', { class: 'section-title' }, section.group),
      el('h3', {}, section.title),
      section.hint ? el('p', { class: 'muted small', style: 'margin: 0' }, section.hint) : null,
      el('div', { class: noGrid ? '' : 'grid2' }, fieldNodes),
      section.group.startsWith('Output Configuration') ? isAddonEnabled('relays') ? el('button', { class: 'btn primary', onclick: (e) => testRelay(section.title, e.target) }, 'Test') : null : null));
  }
  $('tab-config').replaceChildren(
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Settings')),
      el('span', { class: 'pill' }, 'Device config')));
  renderAddonMarketplace();
  $('tab-config').append(...nodes);
}

async function renderAddonMarketplace() {
  if (!state.configValues) return;
  const detectionResult = await api('/api/addons/detection');
  if (detectionResult.ok) addonDetection = detectionResult.data || {};
  let market = document.getElementById('addon-marketplace');
  if (!market) {
    market = el('section', { id: 'addon-marketplace', class: 'card' },
      el('h3', {}, 'Add-ons 🔧'),
      el('p', { class: 'muted small', style: 'margin: 0' }, 'Hardware add-on drivers are compiled into the firmware. Enable detected hardware with a tap — no manual pin or address config needed. Disabled add-ons hide their related config throughout the UI.'),
      el('div', { class: 'addon-grid' }));
    $('tab-config').prepend(market);
  }
  const grid = market.querySelector('.addon-grid');
  const det = addonDetection || {};
  grid.replaceChildren();
  for (const a of ADDONS) {
    const intent = state.configValues[`config.addons.${a.key}`] === true;
    const detected = det[a.key] === true;
    // Manual enable overrides detection gating for auto-addons (rtc, pahub, wifi).
    // User can force-enable even if auto-detection failed; detection is informational.
    const enabled = intent;
    const showDetail = enabled || detected;
    const pillCls = a.auto ? (detected ? (intent ? 'enabled' : 'detected') : 'notdetected') : 'manual';
    const pillText = a.auto ? (detected ? (intent ? 'Enabled' : 'Detected') : 'Not detected') : 'Manual';
    // Enable button is never disabled for manual override; only auto-gating shows disabled state visually.
    const btnDisabled = false;
    const btnText = intent ? 'Disable' : 'Enable';
    const btnCls = intent ? 'btn danger' : 'btn primary';

    const infoLines = (a.info || []).map((line) => el('div', { class: 'muted small', style: 'margin: 2px 0' }, line));
    // PaHub per-channel detection results.
    if (a.key === 'pahub' && det.pahub) {
      const mask = det.pahub_channel_mask || 0;
      const rtcCh = det.pahub_rtc_channel || -1;
      const chAddrs = det.pahub_channel_addrs || [];
      const chInfo = [];
      for (let ch = 0; ch < 6; ch++) {
        const bit = 1 << ch;
        if (mask & bit) {
          const addr = chAddrs[ch] || 0;
          const label = addr === 81 ? 'RTC (PCF8563/HYM8563)' : `0x${addr.toString(16)}`;
          chInfo.push(`Ch${ch}: ${label}`);
        } else {
          chInfo.push(`Ch${ch}: —`);
        }
      }
      infoLines.push(el('div', { class: 'muted small', style: 'margin-top: 4px' }, 'PaHub channels: ' + chInfo.join('  ')));
      if (rtcCh >= 0) {
        infoLines.push(el('div', { class: 'muted small ok', style: 'margin-top: 2px' }, `RTC (0x51) detected on PaHub channel ${rtcCh}`));
      }
    }
    const detail = el('details', { class: 'card fold', open: false },
      el('summary', {},
        el('span', { class: 'chevron', 'aria-hidden': 'true' }, '▸'),
        el('span', { class: 'fold-date' }, 'Configuration'),
        el('span', { class: 'fold-count muted' }, 'details')),
      el('div', { class: 'fold-body' },
        ...infoLines));
    const redetectBtn = el('button', { class: 'btn sm', title: 'Re-detect hardware', onclick: async () => {
      const result = await api('/api/addons/detection', { method: 'POST' });
      if (result.ok) {
        addonDetection = result.data;
        renderAddonMarketplace();
        showMessage('Hardware detection re-run', 'ok');
      } else {
        showMessage('Re-detect failed: ' + (result.data && result.data.error), 'bad');
      }
    } }, 'Re-detect');
    const card = el('div', { class: 'addon-card' },
      el('h3', {}, a.label),
      el('p', { class: 'addon-desc' }, a.desc),
      el('div', { class: 'row-actions', style: 'margin-top: 8px; gap: 8px' },
        el('span', { class: `pill ${pillCls}` }, pillText),
        a.auto ? redetectBtn : null,
        el('button', { class: btnCls, disabled: btnDisabled, onclick: () => toggleAddon(a.key, !intent) }, btnText)),
      showDetail ? detail : null);
    grid.append(card);
  }
}

async function toggleAddon(key, next) {
  if (!state.configVersion) await loadConfig();
  if (!state.configVersion) { showMessage('Config not loaded, please reload the page'); return; }
  const result = await api('/api/config', { version: state.configVersion, config: { [`config.addons.${key}`]: next } });
  if (!result.ok) { showMessage(`Not saved: ${result.data.error}`); return; }
  showMessage('Saved; reloading configuration', 'ok');
  loadConfig();
  if (state.tab === 'config') await renderConfig();
}

$('config-save').addEventListener('click', async () => {
  if (!state.configVersion) await loadConfig();
  if (!state.configVersion) { showMessage('Config not loaded, please reload the page'); return; }
  const diff = configDiff();
  if (!Object.keys(diff).length) return setConfigDirty(false);
  const problem = validateConfigDiff(diff);
  if (problem) return showMessage(problem);
  $('config-save').disabled = true;
  const result = await api('/api/config', { version: state.configVersion, config: diff });
  $('config-save').disabled = false;
  if (!result.ok) {
    if (result.status === 409) return showMessage(result.data.error + ' (Discard reloads the latest version.)');
    return showMessage(`Not saved: ${result.data.error}`);
  }
  showMessage('Saved; the display updates within a second', 'ok');
  loadConfig();
});
$('config-discard').addEventListener('click', () => { if (confirm('Discard your unsaved config changes?')) loadConfig(); });

function checkModel(m) {
  const lists = [...DAYS.map((d) => [d, m.schedule[d]]), ...m.exceptions.map((d) => [d.date, d.entries])];
  for (const [where, entries] of lists) {
    entries.sort((a, b) => a.start - b.start);
    for (const e of entries) {
      if (e.buzzer <= e.start) return `${where}: buzzer ${shortTime(e.buzzer)} is not after start ${shortTime(e.start)}`;
    }
  }
  return null;
}

$('save').addEventListener('click', async () => {
  if (!state.model) await loadModel();
  const m = state.model;
  const problem = checkModel(m);
  if (problem) { showMessage(problem); renderSchedule(); renderExceptions(); return; }
  $('save').disabled = true;
  const result = await api('/api/settings', { version: m.version, schedule: m.schedule, exceptions: m.exceptions });
  $('save').disabled = false;
  if (!result.ok) {
    if (result.status === 409) return showMessage(result.data.error + ' (Discard reloads the latest version.)');
    return showMessage(`Not saved: ${result.data.error}`);
  }
  showMessage('Saved; the display updates within a second', 'ok');
  loadSettings();
});
$('discard').addEventListener('click', () => { if (confirm('Discard your unsaved changes?')) loadSettings(); });
window.addEventListener('beforeunload', (event) => { if (state.dirty || state.configDirty) event.preventDefault(); });

// ---- raw YAML -------------------------------------------------------------
async function loadRaw() {
  const result = await api('/api/raw');
  if (!result.ok) { if (result.status !== 401) showMessage(result.data.error); return; }
  state.raw = result.data;
  const area = el('textarea', { spellcheck: false, value: result.data.text });
  const validate = async () => {
    const check = await api('/api/validate', { text: area.value });
    if (!check.ok) return showMessage(check.data.error);
    showMessage(check.data.ok ? 'No errors found' : check.data.error, check.data.ok ? 'ok' : 'bad');
  };
  const save = async () => {
    if ((state.dirty || state.configDirty) && !confirm('You have unsaved changes that will be lost. Continue?')) return;
    const saved = await api('/api/raw', { version: state.raw.version, text: area.value });
    if (!saved.ok) return showMessage(`Not saved: ${saved.data.error}`);
    showMessage('Saved; the display updates within a second', 'ok');
    state.model = null;
    setDirty(false);
    loadRaw();
  };
  $('tab-config').append(el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Advanced'),
    el('h3', {}, 'Raw settings file'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Full access to every setting (fonts, colours, relays…). Changes are checked before they are applied.'),
    area,
    el('div', { class: 'row-actions' },
      el('button', { class: 'btn', onclick: validate }, 'Check'),
      el('button', { class: 'btn primary', onclick: save }, 'Save & apply'),
      el('button', { class: 'btn', onclick: loadRaw }, 'Reload'),
      el('button', { class: 'btn', onclick: () => {
        const blob = new Blob([area.value], { type: 'text/yaml' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a'); a.href = url; a.download = 'settings.yaml'; a.click();
        URL.revokeObjectURL(url);
      } }, 'Export YAML'),
      el('label', { class: 'btn' }, 'Import YAML',
        el('input', { type: 'file', accept: '.yaml,.yml,text/*', style: 'display:none', onchange: (e) => {
          const file = e.target.files[0]; if (!file) return;
          const r = new FileReader(); r.onload = () => { area.value = r.result; }; r.readAsText(file);
        } })))));
}

// Format raw backup text for display: pretty-print JSON if possible, else pass through.
function formatBackupText(text) {
  try { return JSON.stringify(JSON.parse(text), null, 2); } catch { return text; }
}

// ---- history --------------------------------------------------------------
// ---- diagnostics tab -------------------------------------------------------
async function renderDiagnostics() {
  const root = $('tab-diagnostics');
  const mode = el('select', { onchange: async () => {
    const r = await api('/api/display_test_mode', { mode: mode.value });
    if (r.ok) {
      showMessage('Display test mode: ' + mode.value, 'ok');
      if (state.status) state.status.display_test_mode = mode.value;
    } else {
      showMessage('Display test mode failed: ' + ((r.data && r.data.error) || r.status), 'bad');
      mode.value = (state.status && state.status.display_test_mode) || 'normal';
    }
  } },
    el('option', { value: 'normal' }, 'normal'),
    el('option', { value: 'smpte' }, 'smpte'),
    el('option', { value: 'pluge' }, 'pluge'),
    el('option', { value: 'geometry' }, 'geometry'),
    el('option', { value: 'colorbars' }, 'colorbars'));
  mode.value = (state.status && state.status.display_test_mode) || 'normal';
  const section1 = el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Display'),
    el('h3', {}, 'Display test mode'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Pick a test pattern to take over the display. Choose "normal" to resume normal operation.'),
    el('label', {}, 'Mode', mode));
  const hwTime = el('span', {}, '--');
  const ntpTime = el('span', {}, '--');
  const ntpServer = el('span', {}, '--');
  const ntpDrift = el('span', {}, '--');
  const syncBtn = el('button', { class: 'btn primary', onclick: async () => {
    const r = await api('/api/ntp_sync', {});
    if (r.ok) {
      await loadNtp();
      showMessage('NTP sync requested', 'ok');
    } else {
      showMessage('NTP sync failed: ' + ((r.data && r.data.error) || r.status), 'bad');
    }
  } }, 'Sync now');
  async function loadNtp() {
    const r = await api('/api/ntp_status');
    if (r.ok) {
      hwTime.textContent = new Date(r.data.hardware_time * 1000).toLocaleString();
      ntpTime.textContent = new Date(r.data.ntp_server_time * 1000).toLocaleString();
      if (r.data.ntp_server) ntpServer.textContent = r.data.ntp_server;
      if (r.data.drift_ms != null) ntpDrift.textContent = r.data.drift_ms + ' ms';
    }
  }
  loadNtp();
  const rtcTime = el('span', {}, '--');
  const rtcDetected = el('span', {}, '--');
  const dateInput = el('input', { type: 'date' });
  const timeInput = el('input', { type: 'time', step: 1 });

  async function loadRtc() {
    const r = await api('/api/rtc/time');
    if (r.ok && r.data.ok) {
      const d = r.data;
      rtcTime.textContent = \`\${String(d.year).padStart(4, '0')}-\${String(d.month).padStart(2, '0')}-\${String(d.day).padStart(2, '0')} \${String(d.hour).padStart(2, '0')}:\${String(d.minute).padStart(2, '0')}:\${String(d.second).padStart(2, '0')}\`;
      dateInput.value = \`\${String(d.year).padStart(4, '0')}-\${String(d.month).padStart(2, '0')}-\${String(d.day).padStart(2, '0')}\`;
      timeInput.value = \`\${String(d.hour).padStart(2, '0')}:\${String(d.minute).padStart(2, '0')}\`;
    } else {
      rtcTime.textContent = 'N/A';
    }
    const det = await api('/api/addons/detection');
    if (det.ok && det.data) {
      rtcDetected.textContent = det.data.rtc ? 'Yes' : 'No';
    }
  }
  loadRtc();

  const setBtn = el('button', { class: 'btn primary', onclick: async () => {
    const d = dateInput.value;
    const t = timeInput.value;
    if (!d || !t) { showMessage('Enter both date and time', 'bad'); return; }
    const r = await api('/api/rtc/set', { method: 'POST', body: JSON.stringify({ date: d, time: t }) });
    if (r.ok) {
      showMessage('RTC time set', 'ok');
      loadRtc();
    } else {
      showMessage('Failed: ' + ((r.data && r.data.error) || r.status), 'bad');
    }
  } }, 'Set RTC Time');

  const probeBtn = el('button', { class: 'btn', onclick: async () => {
    const r = await api('/api/addons/detection', { method: 'POST' });
    if (r.ok) {
      await loadRtc();
      showMessage('I2C re-probed', 'ok');
    } else {
      showMessage('Probe failed: ' + ((r.data && r.data.error) || r.status), 'bad');
    }
  } }, 'Probe I2C');

  const section2 = el('div', { class: 'card' },
    el('h2', {}, 'Clock'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'RTC (PCF8563/HYM8563) keeps time independently of NTP. When NTP is unavailable, the display falls back to RTC time.'),
    el('div', { class: 'grid2' },
      el('div', { class: 'field' }, 'Hardware (ESP32) time', hwTime),
      el('div', { class: 'field' }, 'NTP server time', ntpTime),
      el('div', { class: 'field' }, 'NTP server', ntpServer),
      el('div', { class: 'field' }, 'Drift', ntpDrift),
      el('div', { class: 'field' }, 'RTC Detected', rtcDetected),
      el('div', { class: 'field' }, 'Current RTC Time', rtcTime),
      el('div', { class: 'field' }, 'Set Date', dateInput),
      el('div', { class: 'field' }, 'Set Time', timeInput)),
    el('div', { class: 'row-actions' },
      syncBtn,
      probeBtn,
      setBtn,
      el('button', { class: 'btn', onclick: loadRtc }, 'Refresh')),
    el('div', { class: 'muted small', style: 'margin-top: 10px' },
      'Set the date/time here for standalone operation when NTP is unavailable.'));
  const cfgRes = await api('/api/config');
function relayIds(row) { return row.entries.filter(e => e.btn.className.includes('on')).map(e => e.info.id); }
function relayDuration(row) { const onEntries = row.entries.filter(e => e.btn.className.includes('on')); return onEntries.length ? (onEntries[0].dur ? onEntries[0].dur.value : '') : ''; }

function openLeagueModal(league, leagues, onSaved) {
  const idx = league ? leagues.indexOf(league) : -1;
  const editing = idx >= 0;
  
  // Create a new league object for editing (mutated directly)
  const editLeague = league ? { ...league } : {
    name: '',
    game_duration_minutes: 0,
    pregame_countdown_seconds: 0,
    header_text: '',
    pace_enabled: true,
    countdown_seconds_before: 0,
    format: '',
    team_size: 0,
    stones_per_end: 8,
    end_count: 8,
    untimed_ends: 0,
    countdown_seconds_before: 0,
    show_progress_through_end: true,
    total_time_includes_untimed_ends: false,
    replace_conflicts: false,
    manual_trigger: false,
    start_relays: [],
    start_duration: 0,
    end_relays: [],
    end_duration: 0
  };
  
  // Toggle button helper
  const toggleBtn = (on, label) => el('button', { 
    type: 'button', 
    class: 'check-toggle ' + (on ? 'on' : 'off') 
  }, label);
  
  // Build form fields
  const nameIn = el('input', { 
    type: 'text', 
    value: editLeague.name || '', 
    placeholder: 'League name', 
    maxlength: 80,
    oninput: (event) => { editLeague.name = event.target.value.trim() || null; }
  });
  const durationIn = el('input', { 
    type: 'number', 
    min: 1, 
    value: editLeague.game_duration_minutes || 0, 
    oninput: (event) => { editLeague.game_duration_minutes = parseInt(event.target.value) || 0; }
  });
  const pregameIn = el('input', { 
    type: 'number', 
    min: 0, 
    value: editLeague.pregame_countdown_seconds || 0, 
    oninput: (event) => { editLeague.pregame_countdown_seconds = parseInt(event.target.value) || 0; }
  });
  const paceBtn = toggleBtn(editLeague.pace_enabled, 'Pace of play');
  paceBtn.onclick = (e) => { editLeague.pace_enabled = !editLeague.pace_enabled; e.target.className = 'check-toggle ' + (editLeague.pace_enabled ? 'on' : 'off'); };
  const progressBtn = toggleBtn(editLeague.show_progress_through_end, 'Progress through end');
  progressBtn.onclick = (e) => { editLeague.show_progress_through_end = !editLeague.show_progress_through_end; e.target.className = 'check-toggle ' + (editLeague.show_progress_through_end ? 'on' : 'off'); };
  const untimedTotalBtn = toggleBtn(editLeague.total_time_includes_untimed_ends, 'Full game countdown (incl. extra ends)');
  untimedTotalBtn.onclick = (e) => { editLeague.total_time_includes_untimed_ends = !editLeague.total_time_includes_untimed_ends; e.target.className = 'check-toggle ' + (editLeague.total_time_includes_untimed_ends ? 'on' : 'off'); };
  const warningHoldIn = el('input', { 
    type: 'number', 
    min: 0, 
    value: editLeague.countdown_seconds_before || 0, 
    placeholder: 'Countdown expiration before end (seconds)',
    oninput: (event) => { editLeague.countdown_seconds_before = parseInt(event.target.value) || 0; }
  });
  const manualBtn = toggleBtn(editLeague.manual_trigger, 'Manual trigger');
  manualBtn.onclick = (e) => { editLeague.manual_trigger = !editLeague.manual_trigger; e.target.className = 'check-toggle ' + (editLeague.manual_trigger ? 'on' : 'off'); };
  const stonesIn = el('input', { 
    type: 'number', 
    min: 1, 
    value: editLeague.stones_per_end || 8, 
    oninput: (event) => { editLeague.stones_per_end = parseInt(event.target.value) || 8; }
  });
  const endsIn = el('input', { 
    type: 'number', 
    min: 1, 
    value: editLeague.end_count || 8, 
    oninput: (event) => { editLeague.end_count = parseInt(event.target.value) || 8; }
  });
  const untimedIn = el('input', { 
    type: 'number', 
    min: 0, 
    value: editLeague.untimed_ends || 0, 
    oninput: (event) => { editLeague.untimed_ends = parseInt(event.target.value) || 0; }
  });
  const countdownSecondsBeforeIn = el('input', { 
    type: 'number', 
    min: 0, 
    max: 3600, 
    step: 60,
    value: editLeague.countdown_seconds_before || 0, 
    oninput: (event) => { editLeague.countdown_seconds_before = parseInt(event.target.value) || 0; }
  });
  
  const editCard = el('div', { class: 'card league-edit' },
    el('div', { class: 'section-title' }, editing ? 'Edit league profile' : 'Add league profile'),
    el('p', { class: 'muted small', style: 'margin: 0 0 10px' }, 
      editing ? 'Edit existing league settings' : 'Create a new league profile'),
    
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', {}, 'League name'),
      nameIn),
    
    el('div', { class: 'grid2' },
      el('div', { class: 'field' },
        el('label', {}, 'Game duration (minutes)'),
        durationIn),
      el('div', { class: 'field' },
        el('label', {}, 'Pregame countdown (seconds)'),
        pregameIn)),
    
    el('div', { class: 'grid2' },
      el('div', { class: 'field' }, paceBtn),
      el('div', { class: 'field' }, progressBtn)),
    
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', {}, 'Countdown expiration before end (seconds)'),
      warningHoldIn),
    
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', {}, 'Manual trigger (pushbutton)'),
      manualBtn),
    
    el('div', { class: 'grid2' },
      el('div', { class: 'field' },
        el('label', {}, 'Stones per end'),
        stonesIn),
      el('div', { class: 'field' },
        el('label', {}, 'Ends per game'),
        endsIn)),
    
    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', {}, 'Ends after expiration'),
      untimedIn),

    el('div', { class: 'field', style: 'margin-bottom: 10px' },
      el('label', {}, 'Countdown expiration (seconds)'),
      countdownSecondsBeforeIn),

    el('div', { class: 'field', style: 'margin-bottom: 10px' }, untimedTotalBtn),
    
    el('div', { class: 'row-actions' },
      el('button', { class: 'btn primary', type: 'button', onclick: async () => {
        if (!editLeague.name) { showMessage('League name required', 'bad'); return; }
        const profile = { ...editLeague };
        // Remove pace_of_play if not set
        if (profile.pace_enabled === undefined) delete profile.pace_enabled;
        if (profile.show_progress_through_end === undefined) delete profile.show_progress_through_end;
        if (profile.manual_trigger === undefined) delete profile.manual_trigger;
        const next = editing ? leagues.map((item, i) => i === idx ? profile : item) : leagues.concat([profile]);
        const result = await api('/api/leagues', { leagues: next });
        if (!result.ok) { showMessage('Save failed: ' + (result.data.error || 'unknown'), 'bad'); return; }
        state.leagues = next;
        showMessage(editing ? 'League saved' : 'League added', 'ok');
        onSaved();
      }}, editing ? 'Save league' : 'Add league')
    )
  );
  
  // Insert/replace the edit card in the leagues tab
  const root = $('tab-leagues');
  const existing = root.querySelector('.league-edit');
  if (existing) existing.replaceWith(editCard);
  else root.appendChild(editCard);
  
  // Focus name field
  setTimeout(() => nameIn.focus(), 0);
}

// ---- leagues tab -----------------------------------------------------------
async function renderLeagues() {
  const root = $('tab-leagues');
  const r = await api('/api/leagues');
  const leagues = (r.ok && Array.isArray(r.data)) ? r.data.map(normalizeLeaguePayload) : [];
  state.leagues = leagues;

  const saveLeagues = async (arr) => {
    const result = await api('/api/leagues', { leagues: arr });
    if (!result.ok) { showMessage('Save failed: ' + (result.data.error || 'unknown'), 'bad'); return false; }
    return true;
  };

  const addCard = el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Leagues'),
    el('h3', {}, 'Add league profile'),
    el('p', { class: 'muted small', style: 'margin: 0 0 10px' }, 'Define reusable league settings (timing, outputs, display options) to apply to scheduled games.'),
    el('button', { class: 'btn primary', type: 'button', onclick: () => openLeagueModal(null, leagues, () => renderLeagues()) }, '+ Add league profile'));

  const list = el('div', { class: 'card league-list' },
    el('div', { class: 'section-title' }, 'Library'),
    el('h3', {}, 'Configured leagues'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Edit or remove league profiles used by the schedule.'));
  if (!leagues.length) {
    list.append(el('p', { class: 'muted', style: 'margin:0' }, 'No leagues configured yet.'));
  } else {
    leagues.forEach((lp, idx) => {
      const meta = [
        lp.game_duration_minutes ? lp.game_duration_minutes + ' min' : 'no duration set',
        (lp.stones_per_end != null ? lp.stones_per_end : 8) + ' stones',
        (lp.end_count != null ? lp.end_count : 8) + ' ends'
      ].join('  ·  ');
      list.append(el('div', { class: 'league-row' },
        el('span', { class: 'pill quick', 'aria-hidden': 'true' }, '🏆'),
        el('div', {},
          el('div', { class: 'game-name', style: 'font-size:15px' }, lp.name || '(unnamed)'),
          el('div', { class: 'muted small' }, meta),
          lp.manual_trigger && !isAddonEnabled('pushbutton')
            ? el('div', { class: 'muted small' }, 'manual trigger (pushbutton disabled)') : null),
        el('span', { class: 'muted small' }, lp.manual_trigger && isAddonEnabled('pushbutton') ? 'manual' : ''),
        el('button', { class: 'btn', type: 'button', onclick: () => openLeagueModal(lp, leagues, () => renderLeagues()) }, 'Edit'),
        el('button', { class: 'btn icon danger', type: 'button', onclick: async () => {
          if (!confirm(`Delete league "${lp.name || ''}"?`)) return;
          const next = leagues.filter((_, i) => i !== idx);
          if (!(await saveLeagues(next))) return;
          state.editingLeagueIdx = null;
          renderLeagues();
        } }, '✕')));
    });
  }

  const DEFAULT_LEAGUE_URL = 'https://simcoecurlingclub.ca/curling/leagues';
  const cfg = state.model && state.model.config ? state.model.config : {};
  const base = (cfg['config.league_page_url'] || cfg['config.club_website_url'] || '').trim();
  const importBtn = el('button', { class: 'btn', onclick: async () => {
    const url = prompt('Curling Club Manager leagues URL (leave blank to use configured default):', base);
    const endpoint = url && url.trim() ? url.trim() : (base || DEFAULT_LEAGUE_URL);
    if (!endpoint) { showMessage('No league page URL configured.', 'warn'); return; }
    showMessage('Importing leagues...', 'info');
    let names = [];
    try {
      const result = await api('/api/fetch_leagues', { url: endpoint });
      if (!result.ok) { showMessage(result.data.error || 'import failed', 'warn'); return; }
      names = Array.isArray(result.data) ? result.data : [];
    } catch (e) {
      const pasted = prompt('Could not fetch URL. Paste league names, one per line:', '');
      if (pasted) names = pasted.split(/\n/).map(s => s.trim()).filter(Boolean);
    }
    if (!names.length) { showMessage('No leagues found.', 'warn'); return; }
    const existing = new Set(leagues.map(l => (l.name || '').toLowerCase()));
    let added = 0;
    names.forEach(n => {
      if (existing.has(n.toLowerCase())) return;
      leagues.push({ name: n, game_duration_minutes: 0, pregame_countdown_seconds: 0, header_text: '', pace_enabled: false, pace_minutes_per_end: 0, countdown_seconds_before: 0, format: '', team_size: 0, manual_trigger: false });
      existing.add(n.toLowerCase());
      added++;
    });
    if (!(await saveLeagues(leagues))) return;
    renderLeagues();
    showMessage('Imported ' + added + ' new league(s).', 'ok');
  } }, 'Import from Club Manager');

  root.replaceChildren(el('div', { class: 'row-actions', style: 'margin: 0 0 16px' }, importBtn),
    addCard,
    list);
}

// ---- start up -------------------------------------------------------------
let started = false;
async function checkVersion() {
  try {
    const res = await fetch('https://api.github.com/repos/jaydenrussell/esp32-curling-timer/releases/latest');
    if (!res.ok) return;
    const data = await res.json();
    const latest = (data.tag_name || '').replace(/^v/, '');
    const v = document.querySelector('.version'); if (v && latest) v.textContent = 'v' + latest;
    if (latest && latest !== current) {
      const dot = document.createElement('span');
      dot.className = 'version-dot';
      dot.title = `New version ${latest} available - click to view`;
      dot.onclick = () => window.open('https://github.com/jaydenrussell/esp32-curling-timer/releases', '_blank');
      const v = document.querySelector('.version');
      if (v) v.appendChild(dot);
    }
  } catch(e) {}
}
async function start() {
  renderTabs();
  selectTab(state.tab);
  await loadConfig();
  loadStatus();
  if (started) return;
  started = true;
  (function loop() { if (state.tab === 'status' && !document.hidden) tick(); setTimeout(loop, 1000 - (performance.now() % 1000)); })();
  let statusInterval = setInterval(() => { if (state.tab === 'status' && !document.hidden) loadStatus(); }, 1000);
  document.addEventListener('visibilitychange', () => {
    clearInterval(statusInterval);
    statusInterval = setInterval(() => { if (state.tab === 'status' && !document.hidden) loadStatus(); }, document.hidden ? 5000 : 1000);
  });
  document.addEventListener('visibilitychange', () => { if (!document.hidden && state.tab === 'status') loadStatus(); });
}
start();

// ---- PWA: service worker registration ------------------------------------
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.register('/sw.js').catch(() => {});
}
</script>
</body>
</html>)esphome_web_ui";
