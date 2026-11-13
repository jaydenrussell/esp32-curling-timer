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
<link rel="manifest" href="data:application/json;base64,eyJuYW1lIjogIkN1cmxpbmcgVGltZXIiLCAic2hvcnRfbmFtZSI6ICJDdXJsaW5nIFRpbWVyIiwgInN0YXJ0X3VybCI6ICIvIiwgImRpc3BsYXkiOiAic3RhbmRhbG9uZSIsICJiYWNrZ3JvdW5kX2NvbG9yIjogIjBCMTEyMCIgfQ==">
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
  display: flex; align-items: center; gap: 12px;
  padding: 10px 18px; min-height: 64px;
  position: sticky; top: 0; z-index: 120;
  background: rgba(7, 24, 42, 0.55);
  border-bottom: 1px solid rgba(255,255,255,0.08);
  backdrop-filter: blur(18px) saturate(150%);
  -webkit-backdrop-filter: blur(18px) saturate(150%);
}
.header-brand { display: flex; align-items: center; gap: 10px; flex-shrink: 0; }
.header-brand h1 { font-size: 18px; margin: 0; }
.header-version { font-size: 11px; color: var(--text-dim); padding: 3px 8px; border-radius: 999px; background: rgba(255,255,255,0.06); border: 1px solid var(--border); }
#header-msg { font-size: 13px; padding: 8px 12px; border-radius: 12px; white-space: pre-wrap; flex: 1; margin: 0; }
.header-actions { display: flex; gap: 8px; margin-left: auto; }

nav#tabs {
  display: flex; gap: 8px; overflow-x: auto;
  position: sticky; top: 64px; z-index: 110;
  padding: 10px 14px;
  background: rgba(7, 24, 42, 0.45);
  border-bottom: 1px solid rgba(255,255,255,0.08);
  backdrop-filter: blur(14px) saturate(140%);
  -webkit-backdrop-filter: blur(14px) saturate(140%);
}
nav#tabs button {
  flex-shrink: 0; border: 1px solid var(--border); background: rgba(255,255,255,0.04);
  color: var(--muted); border-radius: 999px; padding: 8px 14px; min-height: 38px;
  font-size: 13px; font-weight: 600; display: inline-flex; align-items: center; gap: 8px;
  transition: all .15s ease;
}
nav#tabs button:hover { background: rgba(255,255,255,0.08); color: var(--text); border-color: var(--border-strong); }
nav#tabs button.active {
  background: linear-gradient(135deg, rgba(20,184,166,0.22), rgba(59,130,246,0.22));
  color: var(--text-strong); border-color: rgba(34,211,238,0.45);
  box-shadow: 0 0 0 1px rgba(34,211,238,0.18) inset;
}
.bottom-nav {
  position: fixed; left: 10px; right: 10px; bottom: 10px;
  display: none; flex-direction: row; gap: 4px; z-index: 130;
  border-radius: 18px; padding: 6px;
  background: rgba(7, 24, 42, 0.72);
}
.bottom-nav button {
  flex: 1; border: 1px solid transparent; background: transparent; color: var(--muted);
  border-radius: 14px; padding: 8px 4px; min-height: 54px;
  display: flex; flex-direction: column; align-items: center; justify-content: center; gap: 2px;
  font-size: 11px; font-weight: 600;
}
.bottom-nav button.active { background: var(--primary-soft); color: var(--text-strong); border-color: rgba(34,211,238,0.35); }
.bottom-nav .icon { font-size: 18px; line-height: 1; }
@media (max-width: 768px) {
  .bottom-nav { display: flex; }
  nav#tabs { display: none !important; }
  .savebar { bottom: 84px !important; }
  main { padding-bottom: 150px; }
}
@media (min-width: 769px) { .bottom-nav { display: none !important; } }

main {
  max-width: 1120px; margin: 0 auto; padding: 22px 16px 120px;
  display: flex; flex-direction: column; gap: 18px;
}
@media (max-width: 640px) { main { padding: 14px 10px 150px; } }

.card, .hero, .addon-card {
  border-radius: var(--radius-lg); padding: 20px; margin: 0 0 14px;
  display: flex; flex-direction: column; gap: 12px;
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

input, select, textarea {
  font: inherit; color: var(--text); background: rgba(255,255,255,0.05);
  border: 1px solid var(--border); border-radius: 12px; padding: 10px 13px;
  min-height: 44px; min-width: 0; font-size: 14px;
  transition: border-color .15s ease, box-shadow .15s ease, background .15s ease;
}
@media (pointer: coarse) { input, select, textarea { font-size: 16px; } }
input[type="number"]::-webkit-outer-spin-button, input[type="number"]::-webkit-inner-spin-button { -webkit-appearance: none; margin: 0; }
input[type="number"] { -moz-appearance: textfield; }
input:focus, select:focus, textarea:focus {
  outline: none; border-color: rgba(34,211,238,0.55);
  box-shadow: 0 0 0 3px rgba(20,184,166,0.18); background: rgba(255,255,255,0.08);
}
textarea { width: 100%; min-height: 60vh; font: 13px/1.6 ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; white-space: pre; }
label { display: block; font-size: 12px; color: var(--muted); margin: 0 0 6px; font-weight: 600; letter-spacing: 0.03em; }
label.check { display: inline-flex; align-items: center; gap: 8px; margin: 0; font-size: 14px; color: var(--text); font-weight: 500; }
label.check input { width: 18px; height: 18px; min-height: 0; margin: 0; accent-color: var(--primary); }
.field { margin: 0 0 10px; }
.row-actions { display: flex; flex-wrap: wrap; gap: 10px; align-items: center; margin-top: 6px; }
.duration { display: flex; align-items: center; gap: 8px; }
.duration input { width: 6em; }

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

.grid2 { display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 14px; }
.grid3 { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 14px; }
.page-head { display: flex; align-items: center; justify-content: space-between; gap: 12px; flex-wrap: wrap; margin-bottom: 4px; }
.page-head h1, .page-head h2 { margin: 0; }
.section-title { font-size: 13px; font-weight: 700; letter-spacing: 0.08em; text-transform: uppercase; color: var(--muted); margin: 4px 0 2px; }
.stack { display: flex; flex-direction: column; gap: 14px; }
.split { display: grid; grid-template-columns: 1.2fr 0.8fr; gap: 14px; }
@media (max-width: 900px) { .split { grid-template-columns: 1fr; } }
@media (max-width: 640px) { .grid2 { grid-template-columns: 1fr; } }

.countdown { font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-size: clamp(44px, 12vw, 72px); font-weight: 800; color: var(--text-strong); margin: 8px 0; letter-spacing: -0.02em; }
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

.addon-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(240px, 1fr)); gap: 14px; margin: 10px 0; }
.addon-card h3 { font-size: 16px; }
.addon-desc { color: var(--muted); font-size: 13px; line-height: 1.5; margin: 0; }

.progress-end-bar { position: relative; height: 18px; border-radius: 10px; overflow: hidden; background: rgba(255,255,255,0.08); border: 1px solid rgba(255,255,255,0.12); width: 100%; }
.progress-seg.lead { background: #38bdf8; } .progress-seg.second { background: #34d399; } .progress-seg.vice { background: #fbbf24; } .progress-seg.skip { background: #f87171; }
.progress-end-fill { position: absolute; top:0; left:0; height:100%; background: rgba(2,10,20,0.55); pointer-events:none; z-index:1; transition: width .5s ease; }
.version-dot { display:inline-block; width:8px; height:8px; border-radius:50%; background: var(--warn); margin-left:6px; vertical-align:middle; cursor:pointer; }

@media (prefers-reduced-motion: reduce) { * { transition: none !important; animation: none !important; } }
</style>
</head>
<body>
<div id="login"><form class="card" id="login-form">
  <h2>Curling Timer <span class="version">v2026.11.126</span></h2>
  <p>ESP32-P4 curling timer — live countdown, schedule, and relay control for your club.</p>
  <label for="password">Password</label>
  <input id="password" type="password" autocomplete="current-password" required>
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
    <span class="header-version">v2026.11.126</span>
  </div>
  <span id="header-msg" class="banner hidden"></span>
  <div class="header-actions">
    <button class="btn" id="help" title="Help">Help</button>
    <button class="btn" id="logout" title="Log out">Log out</button>
  </div>
</header>
<nav id="tabs" aria-label="Primary"></nav>
<nav class="bottom-nav" id="bottom-nav" aria-label="Mobile"></nav>
<main>
  <section id="tab-status"></section>
  <section id="tab-quickstart" class="hidden"></section>
  <section id="tab-schedule" class="hidden"></section>
  <section id="tab-exceptions" class="hidden"></section>
  <section id="tab-history" class="hidden"></section>
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
  status: 'Status',
  quickstart: 'Quick Game',
  schedule: 'Schedule',
  leagues: 'Leagues',
  history: 'Report',
  config: 'Settings',
  diagnostics: 'Diagnostics'
};
const NAV_ICONS = {
  status: '📊',
  quickstart: '▶️',
  schedule: '📅',
  leagues: '🏉',
  history: '📋',
  config: '⚙️',
  diagnostics: '🔧'
};
const TABS = [['status', 'status', 'Status'], ['quickstart', 'quickstart', 'Quick Game'], ['schedule', 'schedule', 'Schedule'], ['leagues', 'leagues', 'Leagues'], ['history', 'history', 'Report'], ['config', 'config', 'Settings'], ['diagnostics', 'diagnostics', 'Diagnostics']];
const state = { tab: 'status', status: null, statusAt: 0, model: null, dirty: false, showPast: false, openDays: new Set(), openDates: new Set(), raw: null,
                configOriginal: null, configValues: null, configVersion: null, configDirty: false, editingLeagueIdx: null, leagues: null };

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
    info: [ 'Pin: GPIO45 (manual trigger input)', 'Setting: Manual trigger GPIO under Global options → Timer' ] },
  { key: 'pahub', label: 'PaHub I2C hub', desc: 'PCA9548A 8-channel multiplexer — multiple I2C devices on one Grove port.', auto: true,
    configPaths: ['config.addons.pahub'],
    info: [ 'I2C: address 0x70 (PCA9548A)', 'Port: Grove Port A — SDA GPIO5, SCL GPIO6', 'Channels: 0–7 (one per I2C device)' ] },
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
  for (const child of children.flat()) {
    if (child != null && child !== false) node.append(child instanceof Node ? child : String(child));
  }
  return node;
}
const $ = (id) => document.getElementById(id);
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
function countdownText(seconds) {
  if (seconds <= 0) return '00:00';
  const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60;
  return h ? `${h}:${pad(m)}:${pad(s)}` : `${pad(m)}:${pad(s)}`;
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
    method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) };
  let response;
  try {
    response = await fetch(path, options);
  } catch (err) {
    return { ok: false, status: 0, data: { error: 'cannot reach the timer, check the network' } };
  }
  const data = await response.json().catch(() => ({ error: `unexpected response (${response.status})` }));
  if (response.status === 401 && path !== '/api/login') showLogin();
  return { ok: response.ok, status: response.status, data };
}

// ---- login ----------------------------------------------------------------
function showLogin() { $('login').classList.add('show'); $('password').focus(); }
$('login-form').addEventListener('submit', async (event) => {
  event.preventDefault();
  const btn = $('login-form').querySelector('button[type=submit]');
  btn.disabled = true; btn.textContent = 'Logging in...';
  await new Promise(r => requestAnimationFrame(() => requestAnimationFrame(r)));
  const result = await api('/api/login', { password: $('password').value });
  btn.disabled = false; btn.textContent = 'Log in';
  if (!result.ok) {
    $('login-error').textContent = result.data.error;
    $('login-error').classList.remove('hidden');
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
}
window.addEventListener('resize', renderTabs);
async function selectTab(id) {
  state.tab = id;
  renderTabs();
  clearMessage();
  for (const [tab] of TABS) $(`tab-${tab}`).classList.toggle('hidden', tab !== id);
  if (id === 'status') { if (state.status) renderStatus(); loadStatus(); }
  if (id === 'quickstart') { if (!state.configValues) loadConfig(); loadStatus(); renderQuickstart(); }
  if (id === 'schedule') {
    if (!state.model) await loadSettings();
    else renderSchedule();
  }
  if (id === 'leagues') {
    if (!state.model) await loadSettings();
    renderLeagues();
  }
  if (id === 'config') {
    if (!state.configOriginal || !state.configDirty) await loadConfig();
    else await renderConfig();
    await loadRaw();
  }
  if (id === 'history') loadHistory();
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
  { label: 'Practice (7m)', name: 'PRACTICE', title: 'Practice', seconds: 7 * 60, game_completed: '--- PRACTICE DONE ---', header_hold: 60 },
  { label: 'Game (6 ends)', name: 'Game (6 ends)', seconds: 75 * 60 },
  { label: 'Game (8 ends)', name: 'Game (8 ends)', seconds: 105 * 60 },
  { label: 'Doubles (8 ends)', name: 'Doubles (8 ends)', seconds: 105 * 60 },
];

function buildQuickForm() {
  const minutes = el('input', { type: 'number', min: 0, max: 179, step: 1, inputmode: 'numeric', placeholder: '0', 'aria-label': 'Minutes' });
  const seconds = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', placeholder: '0', 'aria-label': 'Seconds' });
  const name = el('input', { type: 'text', maxlength: 60, placeholder: 'QUICK START' });
  const pregameMin = el('input', { type: 'number', min: 0, max: 60, step: 1, inputmode: 'numeric', value: '5', 'aria-label': 'Pre-game countdown minutes' });
  const pregameSec = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', value: '0', 'aria-label': 'Pre-game countdown seconds' });
  const expiresMin = el('input', { type: 'number', min: 0, max: 60, step: 1, inputmode: 'numeric', value: '10', 'aria-label': 'Expire minutes' });
  const stonesPerEnd = el('input', { type: 'number', min: 1, max: 10, step: 1, inputmode: 'numeric', value: '8', 'aria-label': 'Stones per end' });
  const endCount = el('input', { type: 'number', min: 1, max: 12, step: 1, inputmode: 'numeric', value: '8', 'aria-label': 'Total number of ends' });
  const untimedEnds = el('input', { type: 'number', min: 0, max: 6, step: 1, inputmode: 'numeric', value: '0', 'aria-label': 'Ends after expiration' });
  const showProgress = el('input', { type: 'checkbox', checked: true });
  const pregame = el('input', { type: 'checkbox', checked: true });
  const pace = el('input', { type: 'checkbox', checked: true });
  const replace = el('input', { type: 'checkbox', checked: false });

  const options = () => ({ pregame: pregame.checked, pace_of_play: pace.checked, replace_conflicts: replace.checked, show_progress_through_end: showProgress.checked });

  // Outputs driven from global settings: only enabled relays are offered,
  // each with its own duration field for Game Start and Game Expires.
  const relayNames = RELAY_NAMES();
  const makeRelayRow = (label, boxes, durs) => {
    const row = el('div', { class: 'row-actions', style: 'margin-bottom: 10px; gap: 10px; align-items: baseline' },
      el('span', { class: 'muted small', style: 'margin-right: 8px' }, label));
    for (let i = 0; i < boxes.length; i++) {
      row.append(el('label', { class: 'check' }, boxes[i], relayNames[i] || `RELAY${i + 1}`));
      row.append(el('div', { class: 'duration' }, durs[i], el('span', {}, 's')));
    }
    return row;
  };
  const startBoxes = relayNames.map(() => el('input', { type: 'checkbox', checked: false }));
  const startDurs = relayNames.map(() => el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric', placeholder: '0' }));
  const endBoxes = relayNames.map(() => el('input', { type: 'checkbox', checked: false }));
  const endDurs = relayNames.map(() => el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric', placeholder: '0' }));
  const startRow = relayNames.length ? makeRelayRow('Game Start:', startBoxes, startDurs) : el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.');
  const endRow = relayNames.length ? makeRelayRow('Game Expires:', endBoxes, endDurs) : el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.');

  const relayValues = (boxes) => boxes.filter((b, i) => b.checked).map((b, i) => relayNames[i] || `RELAY${i + 1}`);
  const relayMaxDuration = (durs) => { let m = 0; for (const d of durs) { const v = Number(d.value) || 0; if (v > m) m = v; } return m || null; };

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

  const refreshPreview = () => {
    const s = Number(minutes.value) || 0;
    const sec = Number(seconds.value) || 0;
    const gameSecs = s * 60 + sec;
    const expires = Number(expiresMin.value) || 10;
    const total = gameSecs + expires * 60;
    const when = new Date(Date.now() + total * 1000);
    const endN = Number(endCount.value) || 8;
    const untimed = Number(untimedEnds.value) || 0;
    const startRelays = relayValues(startBoxes);
    const endRelays = relayValues(endBoxes);
    const startDur = relayMaxDuration(startDurs);
    const endDur = relayMaxDuration(endDurs);

    preview.replaceChildren(
      el('div', { class: 'section-title', style: 'margin:0 0 10px' }, 'Preview — settings that will be applied'),
      previewLine('Game time', gameSecs === 0 ? '—' : durationText(gameSecs)),
      previewLine('Expires at', gameSecs === 0 ? '—' : pad(when.getHours()) + ':' + pad(when.getMinutes())),
      previewLine('Pre-game countdown', pregame.checked ? ((Number(pregameMin.value)||0) + 'm ' + (Number(pregameSec.value)||0) + 's') : 'Off'),
      previewLine('Pace of play', pace.checked ? 'On' : 'Off'),
      previewLine('Progress through end', showProgress.checked ? 'On' : 'Off'),
      previewLine('Replace same-time games', replace.checked ? 'On' : 'Off'),
      previewLine('Stones per end', stonesPerEnd.value || 8),
      previewLine('Total ends', endCount.value || 8),
      previewLine('Ends after expiration', untimed > 0 ? `${untimed} extra end(s) played after the buzzer so the game can finish naturally` : 'Game ends at the buzzer'),
      el('div', { style: 'margin-top:10px;border-top:1px solid rgba(255,255,255,0.10);padding-top:10px' }),
      previewLine('Outputs — Game Start', startRelays.length ? startRelays.map((r, i) => r + ' ' + ((Number(startDurs[i].value) || 0) || 'default') + 's').join(', ') : 'None'),
      previewLine('Outputs — Game Expires', endRelays.length ? endRelays.map((r, i) => r + ' ' + ((Number(endDurs[i].value) || 0) || 'default') + 's').join(', ') : 'None')
    );
  };

  const wire = (input) => { input.addEventListener('input', refreshPreview); input.addEventListener('change', refreshPreview); };
  wire(minutes); wire(seconds); wire(expiresMin);
  wire(pregameMin); wire(pregameSec);
  [pregame, pace, replace, showProgress].forEach(c => { c.addEventListener('change', refreshPreview); });
  [stonesPerEnd, endCount, untimedEnds].forEach(i => { i.addEventListener('input', refreshPreview); i.addEventListener('change', refreshPreview); });
  [...startBoxes, ...startDurs, ...endBoxes, ...endDurs].forEach(i => { i.addEventListener('input', refreshPreview); i.addEventListener('change', refreshPreview); });

  const presets = QUICK_PRESETS.map((preset) => el('button', { class: 'btn preset', type: 'button',
    title: `${preset.title || preset.name}, ${durationText(preset.seconds)}`,
    onclick: () => launchQuick({ seconds: preset.seconds, name: preset.name, title: preset.title,
                                 game_completed: preset.game_completed, header_hold: preset.header_hold },
                               options()) }, preset.label));
  const form = el('form', { class: 'card', onsubmit: (event) => {
      event.preventDefault();
      const timer = readQuickForm(minutes, seconds, name);
      if (!timer) return;
      launchQuick({
        seconds: timer.seconds,
        name: timer.name,
        header_hold: (Number(expiresMin.value) || 10) * 60,
        ...options(),
        pregame_countdown: (pregame.checked ? ((Number(pregameMin.value)||0) * 60 + (Number(pregameSec.value)||0)) : 0),
        start_relays: relayValues(startBoxes),
        start_duration: relayMaxDuration(startDurs),
        end_relays: relayValues(endBoxes),
        end_duration: relayMaxDuration(endDurs),
        end_count: Number(endCount.value) || 8,
        stones_per_end: Number(stonesPerEnd.value) || 8,
        untimed_ends: Number(untimedEnds.value) || 0,
      });
    } },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Quick Game'), el('h3', {}, 'Start a game now')),
      el('span', { class: 'pill' }, 'Ad hoc timer')),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Pick a preset or set the timing, options, and outputs below. The preview shows exactly what will be applied.'),
    el('div', { class: 'card', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 8px' }, 'Quick picks'),
      el('div', { class: 'row-actions', style: 'margin: 0; gap: 8px' }, ...presets)),
    el('div', { class: 'grid2' },
      el('div', { class: 'field', role: 'group', 'aria-label': 'Pre-game countdown duration' }, el('label', {}, 'Pre-game countdown'), el('div', { class: 'duration' }, pregameMin, el('span', {}, 'min'), pregameSec, el('span', {}, 's'))),
      el('div', { class: 'field', role: 'group', 'aria-label': 'Full game time' }, el('label', {}, 'Full game time'),
        el('div', { class: 'duration' }, minutes, el('span', {}, 'min'), seconds, el('span', {}, 's'))),
      el('div', { class: 'field' }, el('label', {}, 'Expire time (minutes)'), expiresMin),
      el('div', { class: 'field' }, el('label', {}, 'Name shown on display'), name)),
    el('div', { class: 'grid2' },
      el('div', { class: 'field' }, el('label', {}, 'Stones per end (per team; mixed doubles = 5)'), stonesPerEnd),
      el('div', { class: 'field' }, el('label', {}, 'Total number of ends'), endCount)),
    el('div', { class: 'field' },
      el('label', {}, 'Ends after expiration'),
      untimedEnds,
      el('div', { class: 'muted small', style: 'margin-top: 4px' }, 'Extra ends after the buzzer so the game can finish naturally instead of cutting off mid-end.')),
    el('div', { class: 'card', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 8px' }, 'Game options'),
      el('div', { class: 'row-actions', style: 'margin: 0' },
        el('label', { class: 'check' }, pregame, 'Enable pre-game countdown'),
        el('label', { class: 'check' }, pace, 'Pace of play'),
        el('label', { class: 'check' }, showProgress, 'Show progress through end'),
        el('label', { class: 'check' }, replace, 'Replace same-time games'))),
    el('div', { class: 'card', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 4px' }, 'Outputs'),
      el('p', { class: 'muted small', style: 'margin: 0 0 10px' }, 'Choose which enabled relay outputs ring at Game Start and Game Expires, and for how many seconds. Leave a duration blank to use the global buzzer duration.'),
      startRow,
      endRow),
    preview,
    el('div', { class: 'row-actions' }, button, hint));
  form._defaultsApplied = false;
  form.applyDefaults = () => {
    if (form._defaultsApplied) return;
    const st = state.status || {};
    const cv = state.configValues || ((state.configOriginal && state.configOriginal.config) || {});
    const pg = Number((cv && cv['config.pregame_countdown'] != null) ? cv['config.pregame_countdown'] : (st.pregame_countdown_seconds != null ? st.pregame_countdown_seconds : 0)) || 0;
    pregame.checked = pg > 0;
    pregameMin.value = String(Math.floor(pg / 60));
    pregameSec.value = String(pg % 60);
    const paceEnabled = cv && cv['config.pace_of_play.enabled'] != null ? !!cv['config.pace_of_play.enabled'] : true;
    pace.checked = !!paceEnabled;
    showProgress.checked = true;
    refreshPreview();
    form._defaultsApplied = true;
  };
  form.update = (enabled, clockOk = true) => {
    button.disabled = !enabled;
    presets.forEach((b) => { b.disabled = !enabled; });
    hint.textContent = enabled ? '' : (clockOk ? 'Enable the timer first' : 'Waiting for the clock to sync');
    if (enabled || clockOk || (state.status && state.status.pregame_countdown_seconds != null)) form.applyDefaults();
  };
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
      el('div', {}, el('div', { class: 'section-title' }, 'Live display'), el('h3', {}, 'On-display mirror')),
      el('span', { class: 'pill' }, `Scale ${Math.round((s.mirror_scale ?? 100))}%`)),
    mirror);
  $('status-now').replaceChildren(mirrorCard, now);

  if (!s.clock_synced) {
    $('status-today').replaceChildren();
    return;
  }
  const today = el('div', { class: 'card' },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Today'), el('h3', {}, s.weekday)),
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
    const baseCountdown = pregame ? Math.round(baseTimer * 0.55) : baseTimer;

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
  const centerBottom = px(20 + baseFooter * 1.4 + (showProgress ? 24 : 0));
  const centerWrap = el('div', { style: `position:absolute; left:${px(12)}px; right:${px(12)}px; top:${centerTop}px; bottom:${centerBottom}px; display:flex; flex-direction:column; align-items:center; justify-content:center;` });

  if (sm.show_countdown) {
    const cd = el('div', { id: 'mirror-countdown', style: `color:${unswap(sm.countdown_color) || '#fff'}; font-size:${fcd}px; font-weight:bold; text-align:center; line-height:1; width:100%; margin:0; white-space:nowrap; overflow:hidden;` }, sm.countdown || '');
    cd.dataset.maxFont = String(fcd);
    if (window._mirrorPregameBlinkIv) { clearInterval(window._mirrorPregameBlinkIv); window._mirrorPregameBlinkIv = null; }
    const reduceMotion = !!(window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches);
    if (sm.is_pregame && pregameBlink && !reduceMotion) {
      let visible = true;
      window._mirrorPregameBlinkIv = setInterval(() => {
        visible = !visible;
        const elcd = document.getElementById('mirror-countdown');
        if (!elcd) { clearInterval(window._mirrorPregameBlinkIv); window._mirrorPregameBlinkIv = null; return; }
        elcd.style.visibility = visible ? 'visible' : 'hidden';
      }, 1000);
      cd.style.visibility = 'visible';
    }
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
    centerWrap.append(el('div', { style: `color:${unswap(sm.pace_color) || '#fff'}; font-size:${fp}px; text-align:center; margin-top:${px(8)}px;` }, sm.pace_text));
  }
  stage.append(centerWrap);

  if (showProgress && progressEndCount > 0 && progressGameDuration > 0) {
    const endCount = progressEndCount;
    const gameDur = progressGameDuration;
    const endDur = gameDur / endCount;
    const unitsPerEnd = 4.4;
    const leadPct = (1 / unitsPerEnd * 100).toFixed(1);
    const secondPct = (1 / unitsPerEnd * 100).toFixed(1);
    const vicePct = (1 / unitsPerEnd * 100).toFixed(1);
    const skipPct = (1.4 / unitsPerEnd * 100).toFixed(1);
    const nowSecs = secondsNow();
    const sinceStart = (sm.progress_seconds_since_start != null) ? Number(sm.progress_seconds_since_start) : (s.current && s.current.seconds_since_start != null ? Number(s.current.seconds_since_start) : (s.current ? nowSecs - s.current.start : 0));
    const progWrap = el('div', { id: 'mirror-progress', style: `position:absolute; left:${px(12)}px; right:${px(12)}px; bottom:${px(20 + baseFooter * 1.5)}px; display:flex; flex-direction:column; gap:${Math.max(2, px(3))}px;` });
    for (let e = 0; e < endCount; e++) {
      const endStart = e * endDur;
      const endEnd = endStart + endDur;
      let fillPct = 0;
      if (sinceStart >= endEnd) fillPct = 100;
      else if (sinceStart > endStart) fillPct = ((sinceStart - endStart) / endDur * 100);
      const bar = el('div', { class: 'progress-end-bar', style: `width:100%; height:${Math.max(6, px(14))}px;` });
      bar.append(el('div', { class: 'progress-seg lead', style: `width:${leadPct}%` }));
      bar.append(el('div', { class: 'progress-seg second', style: `width:${secondPct}%` }));
      bar.append(el('div', { class: 'progress-seg vice', style: `width:${vicePct}%` }));
      bar.append(el('div', { class: 'progress-seg skip', style: `width:${skipPct}%` }));
      bar.append(el('div', { class: 'progress-end-fill', style: `width:${fillPct}%` }));
      progWrap.append(bar);
    }
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
  const s = state.status;
  if (!s) { quickForm.classList.add('hidden'); return; }
  if (quickForm) quickForm.classList.toggle('hidden', !s.valid);
  if (s.valid) quickForm.update(s.enabled && s.clock_synced, s.clock_synced);
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
  const fills = document.querySelectorAll('.progress-end-fill');
  if (fills.length && s && s.show_progress && s.progress_end_count && s.progress_game_duration) {
    const endCount = Number(s.progress_end_count);
    const endDur = Number(s.progress_game_duration) / endCount;
    const baseSince = (s.progress_seconds_since_start != null) ? Number(s.progress_seconds_since_start) : (s.current && s.current.seconds_since_start != null ? Number(s.current.seconds_since_start) : 0);
    const sinceStart = baseSince + Math.floor((performance.now() - state.statusAt) / 1000);
    fills.forEach((fill, e) => {
      const endStart = e * endDur;
      const endEnd = endStart + endDur;
      let pct = 0;
      if (sinceStart >= endEnd) pct = 100;
      else if (sinceStart > endStart) pct = (sinceStart - endStart) / endDur * 100;
      fill.style.width = `${Math.round(pct)}%`;
    });
  } else if (fills.length && s && s.current && s.current.show_progress_through_end && s.current.end_count && s.current.buzzer > s.current.start) {
    const endCount = s.current.end_count;
    const gameDur = s.current.buzzer - s.current.start;
    const endDur = gameDur / endCount;
    const nowSecs = secondsNow();
    fills.forEach((fill, e) => {
      const endStart = s.current.start + e * endDur;
      const endEnd = endStart + endDur;
      let pct = 0;
      if (nowSecs >= endEnd) pct = 100;
      else if (nowSecs > endStart) pct = (nowSecs - endStart) / endDur * 100;
      fill.style.width = `${Math.round(pct)}%`;
    });
  }
  const mc = $('mirror-countdown');
  if (mc && s) {
    if (s.is_pregame) {
      if (s.countdown) mc.textContent = s.countdown;
      return;
    }
    if (s.current && s.current.seconds_until_buzzer != null) {
      const left = s.current.seconds_until_buzzer - Math.floor((performance.now() - state.statusAt) / 1000);
      mc.textContent = countdownText(left);
    } else if (s.countdown) {
      mc.textContent = s.countdown;
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

function readQuickForm(minutesInput, secondsInput, nameInput) {
  const minutes = minutesInput.value === '' ? 0 : Number(minutesInput.value);
  const secs = secondsInput.value === '' ? 0 : Number(secondsInput.value);
  if (!Number.isInteger(minutes) || minutes < 0) { showMessage('Minutes must be a whole number'); return null; }
  if (!Number.isInteger(secs) || secs < 0 || secs > 59) { showMessage('Seconds must be a whole number from 0 to 59'); return null; }
  const seconds = minutes * 60 + secs;
  if (seconds < 1) { showMessage('Enter a duration of at least 1 second'); return null; }
  return { seconds, name: nameInput.value.trim() || null };
}

async function launchQuick({ seconds, name, title, game_completed, header_hold }, options) {
  const ends = new Date(Date.now() + seconds * 1000);
  const called = title || name;
  const what = called ? `"${called}" (${durationText(seconds)})` : `a ${durationText(seconds)} timer`;
  if (!confirm(`Start ${what} now? Bell/buzzer at about ${pad(ends.getHours())}:${pad(ends.getMinutes())}:${pad(ends.getSeconds())}.`)) return;
  if (!state.configVersion) await loadConfig();
  const result = await api('/api/quickstart', { seconds, name, game_completed, header_hold, version: state.configVersion, ...options });
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
           pregame_countdown: null, end_count: null, stones_per_end: null, untimed_ends: null, show_progress_through_end: null };
}

function entryEditor(entries, index, leagues, rerender) {
  const entry = entries[index];
  const changed = () => setDirty(true);
  const timeInput = (key) => el('input', { type: 'time', step: 60, value: hms(entry[key]).slice(0, 5), required: true,
    onchange: (event) => { const v = parseTime(event.target.value); if (v !== null) { entry[key] = v; changed(); } } });
  const text = (key, placeholder) => el('input', { type: 'text', value: entry[key] || '', placeholder, maxlength: 80,
    oninput: (event) => { entry[key] = event.target.value.trim() || null; changed(); } });
  const pace = el('select', { onchange: (event) => { entry.pace_of_play = { '': null, on: true, off: false }[event.target.value]; changed(); } },
    el('option', { value: '', selected: entry.pace_of_play === null }, 'Default'),
    el('option', { value: 'on', selected: entry.pace_of_play === true }, 'Show'),
    el('option', { value: 'off', selected: entry.pace_of_play === false }, 'Hide'));
  const unit = el('input', { type: 'number', min: 0.5, step: 0.5, inputmode: 'decimal', placeholder: 'default',
    value: entry.pace_unit_duration ? entry.pace_unit_duration / 60 : '',
    onchange: (event) => { const v = Number(event.target.value); entry.pace_unit_duration = v > 0 ? Math.round(v * 60) : null; changed(); } });
  const hold = el('input', { type: 'number', min: 0, step: 1, inputmode: 'numeric', placeholder: 'default',
    value: entry.header_hold != null ? entry.header_hold : '',
    onchange: (event) => { const v = Number(event.target.value);
                           entry.header_hold = event.target.value !== '' && v >= 0 ? Math.round(v) : null; changed(); } });
  const leagueSelect = el('select', { onchange: (event) => {
    const league = (leagues || []).find((l) => l.name === event.target.value);
    if (!league) { entry.league = null; entry.name = null; changed(); rerender(); return; }
    entry.league = league.name;
    entry.name = league.name;
    entry.buzzer = Math.min(entry.start + (league.game_duration_minutes || 0) * 60, 86340);
    entry.game_completed = league.header_text || null;
    if (league.pregame_countdown_seconds != null) entry.pregame_countdown = league.pregame_countdown_seconds;
    if (league.game_duration_minutes != null && entry.start != null) entry.buzzer = Math.min(entry.start + (league.game_duration_minutes || 0) * 60, 86340);
    if (league.pace_enabled != null) entry.pace_of_play = league.pace_enabled ? true : false;
    if (league.pace_minutes_per_end != null) entry.pace_unit_duration = (league.pace_minutes_per_end || 0) * 60 || null;
    if (league.stones_per_end != null) entry.stones_per_end = league.stones_per_end;
    if (league.end_count != null) entry.end_count = league.end_count;
    if (league.untimed_ends != null) entry.untimed_ends = league.untimed_ends;
    if (league.show_progress_through_end != null) entry.show_progress_through_end = league.show_progress_through_end;
    if (league.start_relays != null) entry.start_relays = league.start_relays;
    if (league.start_duration != null) entry.start_duration = league.start_duration;
    if (league.end_relays != null) entry.end_relays = league.end_relays;
    if (league.end_duration != null) entry.end_duration = league.end_duration;
    changed(); rerender();
  } },
    el('option', { value: '', selected: !entry.league }, '(none)'),
    ...(leagues || []).map((l) => el('option', { value: l.name, selected: entry.league === l.name }, l.name)));
  const pgTotal = (entry.pregame_countdown != null) ? Number(entry.pregame_countdown) : null;
  const pgM = el('input', { type: 'number', min: 0, step: 1, inputmode: 'numeric', placeholder: 'default', value: pgTotal != null ? Math.floor(pgTotal / 60) : '' });
  const pgS = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', placeholder: '0', value: pgTotal != null ? (pgTotal % 60) : '' });
  const syncPg = () => {
    if (pgM.value === '' && pgS.value === '') { entry.pregame_countdown = null; }
    else { entry.pregame_countdown = (parseInt(pgM.value || '0', 10) || 0) * 60 + (parseInt(pgS.value || '0', 10) || 0); }
    changed();
  };
  pgM.onchange = syncPg; pgS.onchange = syncPg;

  return el('div', { class: 'entry' },
    el('div', { class: 'league' }, el('label', {}, 'League'), leagueSelect),
    el('div', { class: 'start' }, el('label', {}, 'Start'), timeInput('start')),
    el('div', { class: 'buzzer' }, el('label', {}, 'Buzzer'), timeInput('buzzer')),
    el('div', { class: 'actions' },
      el('button', { class: 'btn icon danger', type: 'button', title: 'Delete game', onclick: () => { entries.splice(index, 1); changed(); rerender(); } }, '✕')),
    el('div', { class: 'name' }, text('name', 'League name'),
      entry.quick ? el('span', { class: 'pill quick', style: 'align-self: center' }, 'quick') : null),
    el('div', { class: 'more' },
      el('label', {}, 'Pre-game countdown'),
      el('div', { class: 'duration', role: 'group', 'aria-label': 'Pre-game countdown duration' }, pgM, el('span', {}, 'min'), pgS, el('span', {}, 's')),
      el('label', {}, 'Header when game completed'), text('game_completed', 'default'),
      el('label', {}, 'Header hold seconds'), hold,
      el('label', {}, 'Pace of play'), pace,
      el('label', {}, 'Minutes per end'), unit,
      el('label', {}, 'Total ends'), el('input', { type: 'number', min: 1, max: 12, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.end_count != null ? entry.end_count : '', onchange: (event) => { entry.end_count = event.target.value === '' ? null : Math.max(1, parseInt(event.target.value, 10) || 1); changed(); } }),
      el('label', {}, 'Stones per end'), el('input', { type: 'number', min: 1, max: 10, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.stones_per_end != null ? entry.stones_per_end : '', onchange: (event) => { entry.stones_per_end = event.target.value === '' ? null : Math.max(1, parseInt(event.target.value, 10) || 1); changed(); } }),
      el('label', {}, 'Untimed ends'), el('input', { type: 'number', min: 0, max: 6, step: 1, inputmode: 'numeric', placeholder: 'default', value: entry.untimed_ends != null ? entry.untimed_ends : '', onchange: (event) => { entry.untimed_ends = event.target.value === '' ? null : Math.max(0, parseInt(event.target.value, 10) || 0); changed(); } }),
      el('label', { class: 'check' }, el('input', { type: 'checkbox', checked: entry.show_progress_through_end === true, onchange: (event) => { entry.show_progress_through_end = event.target.checked ? true : null; changed(); } }), 'Show progress through end')));
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
      el('div', {}, el('div', { class: 'section-title' }, 'Schedule'), el('h3', {}, 'Weekly games & exceptions')),
      el('span', { class: 'pill' }, `${DAYS.length} days · ${m.exceptions.length} exceptions`))
  ];
  if (m.inner_comments) cards.push(commentWarning(m.inner_comments));
  const calCard = el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Import'),
    el('h3', {}, '📅 Calendar import'),
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
      el('div', {}, el('div', { class: 'section-title' }, 'Weekly schedule'), el('h3', {}, 'Default games by day')),
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
    cards.push(foldCard({ openSet: state.openDays, key: day, title: day, count: entries.length,
      badge: day === todayWeekday ? el('span', { class: 'pill ok' }, 'today') : null,
      body: [entryList(entries, state.leagues, renderSchedule),
        el('div', { class: 'row-actions' },
          el('button', { class: 'btn', onclick: () => {
            entries.push(newEntry(entries[entries.length - 1])); setDirty(true); renderSchedule(); } }, '+ Add game'),
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
  cards.push(el('div', { class: 'card' }, el('div', { class: 'section-title' }, 'Summary'), el('h3', {}, 'Schedule coverage'),
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
  const prefill = el('input', { type: 'checkbox', checked: true });
  const add = el('form', { class: 'card', onsubmit: (event) => {
      event.preventDefault();
      if (m.exceptions.some((d) => d.date === date.value)) return showMessage(`${date.value} already has an exception`);
      const entries = prefill.checked ? structuredClone(m.schedule[weekdayOf(date.value)]) : [];
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
    el('div', { class: 'row-actions' }, el('label', { class: 'check' }, prefill, 'Start from the weekly schedule for that day'),
      el('button', { class: 'btn primary', type: 'submit' }, 'Add date')));

  const past = m.exceptions.filter((d) => d.date < today);
  let shown = [];
  const tools = el('div', { class: 'card', style: 'padding: 14px' },
    el('div', { class: 'row-actions', style: 'margin: 0' },
      el('label', { class: 'check' }, el('input', { type: 'checkbox', checked: state.showPast,
        onchange: (event) => { state.showPast = event.target.checked; renderExceptions(); } }), `Show past dates (${past.length})`),
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
            day.entries.push(newEntry(day.entries[day.entries.length - 1])); setDirty(true); renderExceptions(); } }, '+ Add game'),
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
  { group: 'Global options', title: 'Timer', hint: 'Master on/off for the entire timer. When off, the display shows the disabled header text and stops counting down. Changes are written to the settings file only when you press Save & apply.',
    fields: [
      ['config.enabled', 'bool', 'Timer enabled (master on/off)'],
      ['config.allow_midnight_wrap', 'bool', 'Allow timers past midnight']] },
  { group: 'Global options', title: 'Manual trigger (pushbutton)', hint: 'GPIO press rings the start relays for the next manually-triggered game. Only shown when the pushbutton add-on is enabled.',
    fields: [
      ['config.manual_trigger_gpio', 'int', 'Manual trigger GPIO (pushbutton add-on)', 0, 40]] },
  { group: 'Global options', title: 'Curling Club Manager', hint: 'Base website URL of the club (Curling Club Manager). Used to build league/import paths.',
    fields: [
      ['config.club_website_url', 'text', 'Club website URL (Curling Club Manager)'],
      ['config.league_page_url', 'text', 'League page URL (optional override)']] },
  { group: 'Global options', title: 'On the Display Now', hint: 'Text scaling for the on-display mirror card.',
    fields: [
      ['config.mirror_scale', 'slider', '', 10, 100, 5]] },
  { group: 'Global options', title: 'Relays', hint: 'How many relays are active and what each is mapped to.',
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
  { group: 'Display (HDMI output)', title: 'Pre-game countdown', hint: 'Countdown displayed before the game starts. Set seconds to 0 to disable.',
    fields: [
      ['config.pregame_countdown', 'pregame_hms', 'Countdown', 0, 600],
      ['config.pregame_blink', 'bool', 'Flash/blink pre-game countdown (safe, gentle)'],
      ['config.pregame_font_size', 'int', 'Font size', 0, 100000],
      ['config.pregame_header_color', 'color', 'Header colour'],
      ['config.pregame_countdown_color', 'color', 'Countdown colour'],
      ['config.pregame_header_text', 'text', 'Header text (e.g. Get ready)']] },
  { group: 'Display (HDMI output)', title: 'Header text',
    fields: [
      ['config.header.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.header.text.when_disabled', 'text', 'Text when disabled'],
      ['config.header.text.when_idle', 'text', 'Text when idle'],
      ['config.header.text.counting_down', 'text', 'Text when Countdown'],
      ['config.header.text.game_completed', 'text', 'Text when Warning Countdown Ends/Output'],
      ['config.header.text.game_ended', 'text', 'Text when Game Ends'],
      ['config.header.color.when_disabled', 'color', 'Colour when disabled'],
      ['config.header.color.when_idle', 'color', 'Colour when idle'],
      ['config.header.color.counting_down', 'color', 'Colour while counting down'],
      ['config.header.color.game_completed', 'color', 'Colour while the buzzer rings'],
      ['config.header.color.game_ended', 'color', 'Colour when game ends']] },
  { group: 'Display (HDMI output)', title: 'Countdown clock',
    fields: [
      ['config.countdown.font_size_4_digits', 'int', 'Timer font size under an hour', 0, 100000],
      ['config.countdown.font_size_5_digits', 'int', 'Timer font size an hour or more', 0, 100000],
      ['config.countdown.color.counting_down', 'color', 'Colour while counting down'],
      ['config.countdown.color.game_completed', 'color', 'Colour while the buzzer rings'],
      ['config.countdown.color.game_ended', 'color', 'Colour when game ends']] },
  { group: 'Display (HDMI output)', title: 'Pace of play',
    fields: [
      ['config.pace_of_play.enabled', 'bool', 'Show pace of play'],
      ['config.pace_of_play.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.pace_of_play.color', 'color', 'Colour'],
      ['config.pace_of_play.unit_name', 'text', 'Unit name, e.g. END'],
      ['config.pace_of_play.unit_duration', 'minutes', 'Minutes per unit', 0.5, 43200]] },
  { group: 'Display (HDMI output)', title: 'Footer',
    fields: [
      ['config.footer.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.footer.datetime', 'bool', 'Show date and time'],
      ['config.footer.entry_name', 'bool', 'Show entry name'],
      ['config.footer.color', 'color', 'Colour']] },
  { group: 'Display (HDMI output)', title: 'Background',
    fields: [
      ['config.background_color', 'color', 'Background colour']] },
  { group: 'Outputs 🔊', title: 'Game Start',
    fields: [
      ['config.buzzer.ring_at_start', 'relays', 'Output'],
      ['config.buzzer.start_duration', 'output_duration', 'Duration', 'start']] },
  { group: 'Outputs 🔊', title: 'Countdown Warning',
    hint: 'Only applies to games longer than the minutes set here. For 8-end games, typical pace warning is 15-20 min before buzzer (7th end). For 6-end games, 10-12 min.',
    fields: [
      ['config.buzzer.countdown.minutes_before', 'int', 'Countdown warning: minutes before the buzzer (0 = off)', 0, 100000],
      ['config.buzzer.countdown.relays', 'relays', 'Output'],
      ['config.buzzer.countdown_duration', 'output_duration', 'Duration', 'countdown']] },
  { group: 'Outputs 🔊', title: 'Game End',
    hint: 'Header hold: seconds the during-buzzer header text stays after ringing.',
    fields: [
      ['config.buzzer.relays', 'relays', 'Output'],
      ['config.buzzer.end_duration', 'output_duration', 'Duration', 'end'],
      ['config.buzzer.header_hold', 'int', 'Header hold after the buzzer (seconds)', 0, 100000]] },
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
    } else if (path.endsWith('.font_size') || path.endsWith('.font_size_4_digits') ||
                path.endsWith('.font_size_5_digits') || path === 'config.buzzer.duration' ||
                path === 'config.buzzer.header_hold' ||
                path === 'config.buzzer.countdown.minutes_before' ||
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
  state.configOriginal = result.data.config;
  state.configValues = JSON.parse(JSON.stringify(result.data.config));
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
    return el('div', { class: 'field' }, el('label', { class: 'check' },
      el('input', { type: 'checkbox', checked: value === true, onchange: (event) => set(event.target.checked) }), label));
  }
  if (type === 'pregame_hms') {
    const total = parseInt(value || '0', 10) || 0;
    const m = el('input', { type: 'number', min: 0, max: 60, step: 1, inputmode: 'numeric', value: Math.floor(total/60), 'aria-label': label + ' minutes' });
    const sec = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', value: total % 60, 'aria-label': label + ' seconds' });
    const sync = () => set((parseInt(m.value||'0',10)||0)*60 + (parseInt(sec.value||'0',10)||0));
    m.oninput = sync; sec.oninput = sync;
    return el('div', { class: 'field' }, el('label', {}, label), el('div', { class: 'duration' }, m, el('span', {}, 'min'), sec, el('span', {}, 's')));
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
          el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric',
            style: 'width: 7ch',
            value: val == null ? '' : String(val),
            oninput: (event) => { state.configValues[path] = event.target.value === '' ? null : Number(event.target.value); configTouched(); } })));
    });
    if (selected.length === 0) inputs.push(el('p', { class: 'muted small' }, 'No outputs selected.'));
    return el('div', {}, inputs);
  }
  if (type === 'text') {
    return el('label', {}, label, el('input', { type: 'text', maxlength: 80, value: value == null ? '' : value,
      placeholder: 'not set', oninput: (event) => set(event.target.value) }));
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
    return el('label', {}, label, el('input', { type: 'number', min, max, step: 0.5, inputmode: 'decimal',
      value: value == null ? '' : String(value / 60),
      oninput: (event) => set(event.target.value === '' ? null : Math.round(Number(event.target.value) * 60)) }));
  }
   if (type === 'relays') {
     const selected = Array.isArray(value) ? value : [];
     return el('div', { class: 'field' }, label, el('div', { class: 'row-actions', style: 'margin-top: 2px' },
       RELAY_NAMES().map((name) => {
         const i = Number(name.replace('RELAY', ''));
         const display = relayDisplayName(i) || name;
         return el('label', { class: 'check' },
        el('input', { type: 'checkbox', checked: selected.includes(name), onchange: (event) => {
          const list = Array.isArray(state.configValues[path]) ? [...state.configValues[path]] : [];
          const at = list.indexOf(name);
          if (event.target.checked && at < 0) list.push(name);
          if (!event.target.checked && at >= 0) {
            list.splice(at, 1);
            // Clearing a relay also drops its per-event duration overrides so
            // stale values are not saved for an output that is no longer selected.
            for (const ev of ['start', 'countdown', 'end']) {
              const dp = `config.buzzer.${ev}_duration_${i}`;
              if (dp in state.configValues) delete state.configValues[dp];
            }
          }
          set(list);
        } }), display);
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
    nodes.push(el('div', { class: 'card' },
      el('div', { class: 'section-title' }, section.group),
      el('h3', {}, section.title),
      section.hint ? el('p', { class: 'muted small', style: 'margin: 0' }, section.hint) : null,
      el('div', { class: 'grid2' }, fields.map(configField)),
      section.group.startsWith('Outputs') ? isAddonEnabled('relays') ? el('button', { class: 'btn primary', onclick: (e) => testRelay(section.title, e.target) }, 'Test') : null : null));
  }
  $('tab-config').replaceChildren(
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Settings'), el('h3', {}, 'Configuration & add-ons')),
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
      el('div', { class: 'section-title' }, 'Hardware'),
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
    const enabled = intent && (!a.auto || detected);
    const showDetail = enabled || detected;
    const pillCls = a.auto ? (detected ? (intent ? 'enabled' : 'detected') : 'notdetected') : 'manual';
    const pillText = a.auto ? (detected ? (intent ? 'Enabled' : 'Detected') : 'Not detected') : 'Manual';
    const btnDisabled = a.auto && !detected;
    const btnText = intent ? 'Disable' : 'Enable';
    const btnCls = intent ? 'btn-toggle danger' : 'btn-toggle primary';

    const infoLines = (a.info || []).map((line) => el('div', { class: 'muted small', style: 'margin: 2px 0' }, line));
    const detail = el('details', { class: 'card fold', open: false },
      el('summary', {},
        el('span', { class: 'chevron', 'aria-hidden': 'true' }, '▸'),
        el('span', { class: 'fold-date' }, 'Configuration'),
        el('span', { class: 'fold-count muted' }, 'details')),
      el('div', { class: 'fold-body' },
        ...infoLines));
    const card = el('div', { class: 'addon-card' },
      el('h3', {}, a.label),
      el('p', { class: 'addon-desc' }, a.desc),
      el('div', { class: 'row-actions', style: 'margin-top: 8px; gap: 8px' },
        el('span', { class: `pill ${pillCls}` }, pillText),
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

// ---- history --------------------------------------------------------------
async function loadHistory() {
  const result = await api('/api/backups');
  if (!result.ok) { if (result.status !== 401) showMessage(result.data.error); return; }
  const view = el('pre', { class: 'view hidden' });
  const list = el('ul', { class: 'backups' }, result.data.backups.map((b) => el('li', {},
    el('span', {}, new Date(b.mtime * 1000).toLocaleString()),
    el('button', { class: 'btn icon', onclick: async () => {
      const got = await api(`/api/backups/${encodeURIComponent(b.name)}`);
      if (!got.ok) return showMessage(got.data.error);
      view.textContent = got.data.text; view.classList.remove('hidden'); view.scrollIntoView({ behavior: 'smooth' });
    } }, 'View'),
    el('button', { class: 'btn icon', onclick: async () => {
      if (!confirm(`Restore the settings from ${new Date(b.mtime * 1000).toLocaleString()}? The current file is backed up first.`)) return;
      const raw = await api('/api/raw');
      if (!raw.ok) return showMessage(raw.data.error);
      const restored = await api('/api/restore', { name: b.name, version: raw.data.version });
      if (!restored.ok) return showMessage(`Not restored: ${restored.data.error}`);
      showMessage('Restored; the display updates within a second', 'ok');
      state.model = null; setDirty(false); loadHistory();
    } }, 'Restore'))));
  $('tab-history').replaceChildren(
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Report'), el('h3', {}, 'History & backups')),
      el('span', { class: 'pill' }, `${result.data.backups.length} backup${result.data.backups.length === 1 ? '' : 's'}`)),
    el('div', { class: 'card' },
      el('div', { class: 'section-title' }, 'Previous versions'),
      el('h3', {}, 'Restore or inspect saved settings'),
      el('p', { class: 'muted small', style: 'margin: 0' }, 'A copy is kept automatically before every change (latest 30). Restore creates a backup of the current file first.'),
      result.data.backups.length ? list : el('p', { class: 'muted', style: 'margin:0' }, 'No backups yet.'),
      view));
}

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
  const section2 = el('div', { class: 'card' },
    el('h2', {}, 'NTP Time Sync'),
    el('div', { class: 'grid2' },
      el('div', { class: 'field' }, 'Hardware (ESP32) time', hwTime),
      el('div', { class: 'field' }, 'NTP server time', ntpTime),
      el('div', { class: 'field' }, 'NTP server', ntpServer),
      el('div', { class: 'field' }, 'Drift', ntpDrift)),
    el('div', { class: 'row-actions' }, syncBtn));
  const cfgRes = await api('/api/config');
  const savedLevel = (cfgRes.ok && cfgRes.data && cfgRes.data.config && cfgRes.data.config['config.log_level'] != null) ? cfgRes.data.config['config.log_level'] : 3;
  const levelSelect = el('select', { onchange: async () => {
    const level = parseInt(levelSelect.value, 10);
    const r = await api('/api/log_level', { level: level });
    await api('/api/config', { version: state.configVersion, config: { 'config.log_level': level } });
    if (r.ok) showMessage('Log level set', 'ok'); else showMessage('Failed: ' + r.status, 'bad');
  } },
    el('option', { value: '0' }, 'NONE'),
    el('option', { value: '1' }, 'ERROR'),
    el('option', { value: '2' }, 'WARN'),
    el('option', { value: '3' }, 'INFO'),
    el('option', { value: '4' }, 'DEBUG'),
    el('option', { value: '5' }, 'VERBOSE'));
  levelSelect.value = String(savedLevel);
  const section3 = el('div', { class: 'card' },
    el('div', { class: 'section-title' }, 'Logs'),
    el('h3', {}, 'Device logs'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Download captured logs (500-line ring buffer) and adjust log verbosity.'),
    el('div', { class: 'row-actions' },
      el('a', { class: 'btn', href: '/api/logs', target: '_blank', rel: 'noopener' }, 'Download logs'),
      el('label', { style: 'min-width: 180px' }, 'Log level', levelSelect)));
  root.replaceChildren(
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Diagnostics'), el('h3', {}, 'Display, time, and logs')),
      el('span', { class: 'pill' }, 'Troubleshooting')),
    el('div', { class: 'stack' }, section1, section2, section3));
}

// ---- leagues tab -----------------------------------------------------------
async function renderLeagues() {
  const root = $('tab-leagues');
  const r = await api('/api/leagues');
  const leagues = (r.ok && Array.isArray(r.data)) ? r.data : [];

  const saveLeagues = async (arr) => {
    const result = await api('/api/leagues', { leagues: arr });
    if (!result.ok) { showMessage('Save failed: ' + (result.data.error || 'unknown'), 'bad'); return false; }
    return true;
  };

  const leagueFields = (lp, prefix) => {
    const nameInput = el('input', { type: 'text', id: prefix + '-name', value: lp.name || '', maxlength: 80,
      oninput: (ev) => { lp.name = ev.target.value.trim(); } });
    const durInput = el('input', { type: 'number', id: prefix + '-dur', value: lp.game_duration_minutes || '', min: 0,
      oninput: (ev) => { lp.game_duration_minutes = parseInt(ev.target.value || '0', 10); } });
    const pgSecsTotal = parseInt(lp.pregame_countdown_seconds || '0', 10) || 0;
    const pregameMinIn = el('input', { type: 'number', id: prefix + '-pregame-min', value: Math.floor(pgSecsTotal/60), min: 0, step: 1, inputmode: 'numeric', 'aria-label': 'Pre-game countdown minutes' });
    const pregameSecIn = el('input', { type: 'number', id: prefix + '-pregame-sec', value: pgSecsTotal % 60, min: 0, max: 59, step: 1, inputmode: 'numeric', 'aria-label': 'Pre-game countdown seconds' });
    const syncPregame = () => { lp.pregame_countdown_seconds = (parseInt(pregameMinIn.value||'0',10)||0)*60 + (parseInt(pregameSecIn.value||'0',10)||0); };
    pregameMinIn.oninput = syncPregame; pregameSecIn.oninput = syncPregame;
    const pregameInput = el('div', { class: 'duration', id: prefix + '-pregame' }, pregameMinIn, el('span', {}, 'min'), pregameSecIn, el('span', {}, 's'));
    const headerInput = el('input', { type: 'text', id: prefix + '-header', value: lp.header_text || '', maxlength: 80,
      oninput: (ev) => { lp.header_text = ev.target.value.trim(); } });
    const paceCheck = el('input', { type: 'checkbox', id: prefix + '-pace', checked: !!lp.pace_enabled,
      onchange: (ev) => { lp.pace_enabled = ev.target.checked; } });
    const stonesInput = el('input', { type: 'number', id: prefix + '-stones', value: lp.stones_per_end || '8', min: 1, max: 10,
      oninput: (ev) => { lp.stones_per_end = parseInt(ev.target.value || '8', 10); } });
    const endsInput = el('input', { type: 'number', id: prefix + '-ends', value: lp.end_count || '8', min: 1, max: 12,
      oninput: (ev) => { lp.end_count = parseInt(ev.target.value || '8', 10); } });
    const untimedInput = el('input', { type: 'number', id: prefix + '-untimed', value: lp.untimed_ends || '0', min: 0, max: 6,
      oninput: (ev) => { lp.untimed_ends = parseInt(ev.target.value || '0', 10); } });
    const progressCheck = el('input', { type: 'checkbox', id: prefix + '-progress', checked: lp.show_progress_through_end !== false,
      onchange: (ev) => { lp.show_progress_through_end = ev.target.checked; } });
    const cmbInput = el('input', { type: 'number', id: prefix + '-cmb', value: lp.countdown_minutes_before || '', min: 0,
      oninput: (ev) => { lp.countdown_minutes_before = parseInt(ev.target.value || '0', 10); } });
    const manualCheck = el('input', { type: 'checkbox', id: prefix + '-manual', checked: !!lp.manual_trigger,
      disabled: !isAddonEnabled('pushbutton'), onchange: (ev) => { lp.manual_trigger = ev.target.checked; } });

    // Outputs driven from global settings: only enabled relays offered, each with one duration per event.
    const relayNames = RELAY_NAMES();
    const startBoxes = relayNames.map(() => el('input', { type: 'checkbox', checked: false }));
    const startDur = el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric', placeholder: '0' });
    const endBoxes = relayNames.map(() => el('input', { type: 'checkbox', checked: false }));
    const endDur = el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric', placeholder: '0' });
    // restore saved values
    const savedSR = lp.start_relays || [];
    const savedER = lp.end_relays || [];
    startBoxes.forEach((b, i) => { b.checked = savedSR.includes(relayNames[i]); });
    endBoxes.forEach((b, i) => { b.checked = savedER.includes(relayNames[i]); });
    startDur.value = String(lp.start_duration || '');
    endDur.value = String(lp.end_duration || '');

    const relayRow = (label, boxes, durInput) => {
      boxes.forEach((b, i) => b.addEventListener('change', () => {
        const checked = boxes.filter((bb, j) => bb.checked).map((bb, j) => relayNames[j] || `RELAY${j + 1}`);
        lp.start_relays = label.startsWith('Game Start') ? checked : lp.start_relays || [];
        lp.end_relays = label.startsWith('Game Expires') ? checked : lp.end_relays || [];
      }));
      durInput.addEventListener('input', () => {
        const v = parseInt(durInput.value || '0', 10);
        if (label.startsWith('Game Start')) lp.start_duration = v; else lp.end_duration = v;
      });
      return el('div', { class: 'row-actions', style: 'margin-bottom: 8px; gap: 10px; align-items: baseline' },
        el('span', { class: 'muted small', style: 'margin-right: 8px' }, label),
        ...boxes.flatMap((b, i) => el('label', { class: 'check' }, b, relayNames[i] || `RELAY${i + 1}`)),
        el('div', { class: 'duration' }, durInput, el('span', {}, 's')));
    };
    const outputsCard = relayNames.length ? el('div', { class: 'card', style: 'margin: 10px 0 0' },
      el('h3', {}, 'Outputs 🔊'),
      el('p', { class: 'muted small', style: 'margin: 0 0 8px' }, 'Choose which enabled relay outputs ring at Game Start and Game Expires, and for how many seconds. Leave a duration blank to use the global buzzer duration.'),
      relayRow('Game Start:', startBoxes, startDur),
      relayRow('Game Expires:', endBoxes, endDur)) : el('div', { class: 'card', style: 'margin: 10px 0 0' },
      el('h3', {}, 'Outputs 🔊'),
      el('div', { class: 'muted small' }, 'No relay outputs enabled in Settings.'));

    return [
      el('div', { class: 'grid2' },
        el('label', {}, 'Name', nameInput),
        el('label', {}, 'Duration (min)', durInput)),
      el('div', { class: 'grid2' },
        el('label', {}, 'Pre-game countdown', pregameInput),
        el('label', {}, 'Header text', headerInput)),
      el('div', { class: 'grid2' },
        el('label', {}, 'Stones per end', stonesInput),
        el('label', {}, 'Total ends', endsInput)),
      el('div', { class: 'grid2' },
        el('label', {}, 'Ends after expiration', untimedInput),
        el('label', { class: 'check' }, progressCheck, 'Show progress through end?')),
      el('div', { class: 'grid2' },
        el('label', { class: 'check' }, paceCheck, 'Pace of play (auto: game duration ÷ ends)'),
        el('label', {}, 'Countdown warning minutes before', cmbInput)),
      el('div', { class: 'grid2' },
        el('label', { class: 'check' }, manualCheck, 'Manual push-button trigger')),
      outputsCard
    ];
  };

  let addLeagueDefaults;
  const addForm = el('form', { class: 'card', onsubmit: async (e) => {
    e.preventDefault();
    const name = document.getElementById('lg-add-name').value.trim();
    if (!name) { showMessage('Name required', 'bad'); return; }
    const entry = {
      name,
      game_duration_minutes: addLeagueDefaults.game_duration_minutes,
      pregame_countdown_seconds: addLeagueDefaults.pregame_countdown_seconds,
      header_text: document.getElementById('lg-add-header').value || addLeagueDefaults.header_text,
      stones_per_end: addLeagueDefaults.stones_per_end,
      end_count: addLeagueDefaults.end_count,
      untimed_ends: addLeagueDefaults.untimed_ends,
      show_progress_through_end: addLeagueDefaults.show_progress_through_end,
      pace_enabled: addLeagueDefaults.pace_enabled,
      countdown_minutes_before: addLeagueDefaults.countdown_minutes_before,
      manual_trigger: addLeagueDefaults.manual_trigger,
      start_relays: addLeagueDefaults.start_relays,
      start_duration: addLeagueDefaults.start_duration,
      end_relays: addLeagueDefaults.end_relays,
      end_duration: addLeagueDefaults.end_duration,
    };
    leagues.push(entry);
    if (!(await saveLeagues(leagues))) return;
    showMessage('League added', 'ok');
    renderLeagues();
  } },
    el('div', { class: 'section-title' }, 'Leagues'),
    el('h3', {}, 'Add league profile'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Define reusable league settings (timing, outputs, display options) to apply to scheduled games.'),
    ...leagueFields(addLeagueDefaults = {
      name:'', game_duration_minutes:0, pregame_countdown_seconds:0, header_text:'',
      stones_per_end:8, end_count:8, untimed_ends:0, show_progress_through_end:true,
      pace_enabled:false, countdown_minutes_before:0, manual_trigger:false,
      start_relays:[], start_duration:0, end_relays:[], end_duration:0
    }, 'lg-add'),
    el('div', { class: 'row-actions' }, el('button', { class: 'btn primary', type: 'submit' }, 'Add League')));

  const list = el('div', { class: 'card league-list' },
    el('div', { class: 'section-title' }, 'Library'),
    el('h3', {}, 'Configured leagues'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Edit or remove league profiles used by the schedule.'));
  if (!leagues.length) {
    list.append(el('p', { class: 'muted', style: 'margin:0' }, 'No leagues configured yet.'));
  } else {
    leagues.forEach((lp, idx) => {
      const editing = state.editingLeagueIdx === idx;
      const badge = el('span', { class: 'fold-badge muted' },
        lp.manual_trigger && isAddonEnabled('pushbutton') ? 'manual trigger' :
        lp.manual_trigger && !isAddonEnabled('pushbutton') ? 'manual trigger (pushbutton disabled)' :
        '');
      const note = el('span', { class: 'fold-note muted' }, `${lp.game_duration_minutes ? lp.game_duration_minutes + ' min' : 'no duration set'}`);
      const body = el('div', { class: 'fold-body' },
        ...leagueFields(lp, 'lg-' + idx),
        el('div', { class: 'row-actions', style: 'margin-top: 10px' },
          el('button', { class: 'btn primary', onclick: async () => {
            if (!(await saveLeagues(leagues))) return;
            state.editingLeagueIdx = null;
            renderLeagues();
          } }, 'Save'),
          el('button', { class: 'btn', onclick: () => {
            state.editingLeagueIdx = null;
            renderLeagues();
          } }, 'Cancel'),
          el('button', { class: 'btn icon danger', onclick: async () => {
            if (!confirm(`Delete league "${lp.name || ''}"?`)) return;
            leagues.splice(idx, 1);
            state.editingLeagueIdx = null;
            if (!(await saveLeagues(leagues))) return;
            renderLeagues();
          } }, '✕')));
      list.append(el('details', { class: 'card fold', open: editing,
          ontoggle: (event) => { if (event.target.open) { state.editingLeagueIdx = idx; } else { state.editingLeagueIdx = null; } } },
        el('summary', {},
          el('span', { class: 'chevron', 'aria-hidden': 'true' }, '▸'),
          el('span', { class: 'fold-date' }, lp.name || '(unnamed)'),
          badge,
          note,
          el('span', { class: 'fold-count muted' }, '')),
        body));
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
      leagues.push({ name: n, game_duration_minutes: 0, pregame_countdown_seconds: 0, header_text: '', pace_enabled: false, pace_minutes_per_end: 0, countdown_minutes_before: 0, format: '', team_size: 0, manual_trigger: false });
      existing.add(n.toLowerCase());
      added++;

    });
     if (!(await saveLeagues(leagues))) return;
     renderLeagues();
     showMessage('Imported ' + added + ' new league(s).', 'ok');
   } }, 'Import from Club Manager');

  root.replaceChildren(el('div', { class: 'row-actions', style: 'margin: 0 0 16px' }, importBtn),
    addForm,
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
