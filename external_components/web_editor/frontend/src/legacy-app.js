import { api, setUnauthorizedHandler } from './lib/api.js';
import { startPoller } from './lib/poll.js';

export function initLegacyApp() {
  const cleanupFns = [];
  const registerCleanup = (fn) => { if (typeof fn === 'function') cleanupFns.push(fn); };


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

// api() provided by lib/api.js

// ---- login ----------------------------------------------------------------
function showLogin() { $('login').classList.add('show'); $('password')?.focus(); }
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
async function loadStatus(opts = {}) {
  const result = await api('/api/status', undefined, opts);
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



// ---- standardized game UI helpers (Quick Game / Schedule / Leagues) --------
function gameGlobalDefaults() {
  const cv = state.configValues || ((state.configOriginal && state.configOriginal.config) || {});
  const st = state.status || {};
  return {
    pregameSeconds: Number((cv && cv['config.pregame_countdown'] != null) ? cv['config.pregame_countdown'] : (st.pregame_countdown_seconds != null ? st.pregame_countdown_seconds : 0)) || 0,
    paceEnabled: cv && cv['config.pace_of_play.enabled'] != null ? !!cv['config.pace_of_play.enabled'] : true,
    stonesPerEnd: 8,
    endCount: 8,
    untimedEnds: 0,
    showProgress: true,
    countdownMinutesBefore: Number((cv && cv['config.buzzer.countdown.minutes_before'] != null) ? cv['config.buzzer.countdown.minutes_before'] : 0) || 0,
    buzzerEndDuration: Number((cv && cv['config.buzzer.end_duration'] != null) ? cv['config.buzzer.end_duration'] : 0) || 0,
    buzzerStartDuration: Number((cv && cv['config.buzzer.start_duration'] != null) ? cv['config.buzzer.start_duration'] : 0) || 0,
    headerHold: Number((cv && cv['config.buzzer.header_hold'] != null) ? cv['config.buzzer.header_hold'] : 299) || 0,
  };
}
function resolvedDefaultText(v) { return v == null || v === '' ? 'uses global/default' : `override: ${v}`; }
function makePregameToggleControl(secondsValue, opts = {}) {
  const total = secondsValue == null ? null : Math.max(0, Number(secondsValue) || 0);
  const enabled = opts.enabled != null ? !!opts.enabled : (total != null ? total > 0 : !!opts.defaultEnabled);
  const defSec = Math.max(0, Number(opts.defaultSeconds || 0) || 0);
  const showSec = total != null ? total : defSec;
  const cb = el('input', { type: 'checkbox', checked: enabled });
  const min = el('input', { type: 'number', min: 0, max: 180, step: 1, inputmode: 'numeric', value: String(Math.floor(showSec / 60)), placeholder: String(Math.floor(defSec / 60) || 0), 'aria-label': 'Pre-game countdown minutes' });
  const sec = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', value: String(showSec % 60), placeholder: String(defSec % 60 || 0), 'aria-label': 'Pre-game countdown seconds' });
  const dur = el('div', { class: 'duration' }, min, el('span', {}, 'min'), sec, el('span', {}, 's'));
  const hint = el('div', { class: 'muted small' }, opts.hint || (opts.allowInherit ? 'When disabled in schedule/league, leave unchecked and blank to inherit default/global behavior.' : ''));
  const wrap = el('div', { class: 'game-option-block' },
    el('label', { class: 'check' }, cb, 'Pre-Game Countdown'),
    el('div', { class: 'game-option-detail' }, el('label', {}, 'Pre-game countdown time'), dur, hint));
  const syncVisibility = () => { wrap.classList.toggle('is-collapsed', !cb.checked); };
  const getSeconds = () => {
    if (!cb.checked) return opts.uncheckedValue !== undefined ? opts.uncheckedValue : 0;
    if (opts.allowInherit && min.value === '' && sec.value === '') return null;
    return (parseInt(min.value || '0', 10) || 0) * 60 + (parseInt(sec.value || '0', 10) || 0);
  };
  cb.addEventListener('change', () => { syncVisibility(); opts.onChange && opts.onChange(getSeconds(), cb.checked); });
  [min, sec].forEach((i) => { i.addEventListener('input', () => opts.onChange && opts.onChange(getSeconds(), cb.checked)); i.addEventListener('change', () => opts.onChange && opts.onChange(getSeconds(), cb.checked)); });
  syncVisibility();
  return { el: wrap, cb, min, sec, getSeconds, set(secondsVal, enabledVal) {
    const en = enabledVal != null ? !!enabledVal : (secondsVal != null ? Number(secondsVal) > 0 : cb.checked);
    cb.checked = en;
    if (secondsVal == null) { if (opts.allowInherit) { min.value = ''; sec.value = ''; } }
    else { const t = Math.max(0, Number(secondsVal) || 0); min.value = String(Math.floor(t/60)); sec.value = String(t%60); }
    syncVisibility();
  } };
}
function buildStandardizedOutputsUI(ctx = {}) {
  const outputInfos = enabledOutputInfos();
  const defStart = Number((state.configValues && state.configValues['config.buzzer.start_duration']) || 0) || 0;
  const defEnd = Number((state.configValues && state.configValues['config.buzzer.end_duration']) || 0) || 0;
  const makeRow = (title, subtitle, selectedIds, durationValue, defDur) => {
    const boxes = outputInfos.map((info) => el('input', { type: 'checkbox', checked: Array.isArray(selectedIds) && selectedIds.includes(info.id) }));
    const durs = outputInfos.map((info, i) => el('input', { type: 'number', min: 0, max: 999, step: 1, inputmode: 'numeric', placeholder: String(defDur || 'default'), value: boxes[i].checked && durationValue != null ? String(durationValue) : '' }));
    const read = () => {
      const ids = outputInfos.filter((_, i) => boxes[i].checked).map((o) => o.id);
      if (!ids.length) return { relays: [], duration: null };
      let hasExplicit = false;
      let max = 0;
      outputInfos.forEach((_, i) => {
        if (!boxes[i].checked) return;
        if (durs[i].value !== '') { hasExplicit = true; max = Math.max(max, parseInt(durs[i].value || '0', 10) || 0); }
      });
      return { relays: ids, duration: hasExplicit ? max : null };
    };
    const emit = () => ctx.onChange && ctx.onChange();
    outputInfos.forEach((_, i) => {
      boxes[i].addEventListener('change', () => { if (!boxes[i].checked) durs[i].value = ''; emit(); });
      durs[i].addEventListener('input', emit); durs[i].addEventListener('change', emit);
    });
    const row = el('div', { class: 'outputs-row-card' },
      el('div', { class: 'outputs-row-head' },
        el('div', {}, el('div', { class: 'outputs-row-title' }, title), subtitle ? el('div', { class: 'muted small' }, subtitle) : null),
        el('span', { class: 'pill' }, outputInfos.length ? `${outputInfos.length} enabled` : 'none enabled')),
      outputInfos.length ? el('div', { class: 'outputs-relay-grid' },
        ...outputInfos.flatMap((info, i) => [el('label', { class: 'check outputs-relay-check' }, boxes[i], el('span', {}, info.label)), el('label', { class: 'outputs-dur-field' }, el('span', {}, 'Duration (s)'), durs[i])]))
        : el('div', { class: 'muted small' }, 'No enabled relay outputs found in global config. Enable relays in Settings → Relays.'),
      el('div', { class: 'muted small outputs-row-hint' }, outputInfos.length ? `Blank duration uses global/default (${defDur || 0}s) when outputs are selected.` : ''));
    return { el: row, boxes, durs, read, set(sel, dur) {
      outputInfos.forEach((info, i) => { boxes[i].checked = Array.isArray(sel) && sel.includes(info.id); durs[i].value = (boxes[i].checked && dur != null) ? String(dur) : ''; });
    } };
  };
  const start = makeRow('Game Start', 'Outputs fired when the game/start countdown begins.', ctx.startRelays, ctx.startDuration, defStart);
  const warning = makeRow('Game Warning', ctx.warningSubtitle || 'Outputs fired at game warning/expiration time (buzzer/warning event).', ctx.endRelays, ctx.endDuration, defEnd);
  const card = el('div', { class: 'card game-subcard outputs-card' },
    el('div', { class: 'section-title', style: 'margin-bottom: 2px' }, 'Outputs'),
    el('p', { class: 'muted small', style: 'margin: 0 0 8px' }, 'Condensed output controls. Relay names come from enabled outputs and their configured function in global settings. Leave duration blank to use global/default when selected.'),
    start.el, warning.el);
  return { el: card, start, warning, read() { return { start: start.read(), warning: warning.read() }; } };
}
function createStandardGameForm(opts = {}) {
  const g = gameGlobalDefaults();
  const mode = opts.mode || 'quick'; // quick|schedule|league
  const vals = opts.values || {};
  const onChange = () => { opts.onChange && opts.onChange(apiRead()); };

  const isQuick = mode === 'quick';
  const gameDurationMin = el('input', { type: 'number', min: 0, max: 720, step: 1, inputmode: 'numeric', placeholder: '0', 'aria-label': 'Game duration minutes' });
  const gameDurationSec = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', placeholder: '0', 'aria-label': 'Game duration seconds' });
  const warningAfterMin = el('input', { type: 'number', min: 0, max: 180, step: 1, inputmode: 'numeric', placeholder: String(Math.floor(g.headerHold/60)||10), 'aria-label': 'Game warning minutes' });
  const warningAfterSec = el('input', { type: 'number', min: 0, max: 59, step: 1, inputmode: 'numeric', placeholder: String(g.headerHold%60||0), 'aria-label': 'Game warning seconds' });
  const stonesPerEnd = el('input', { type: 'number', min: 1, max: 10, step: 1, inputmode: 'numeric', placeholder: String(g.stonesPerEnd), value: (vals.stonesPerEnd != null ? String(vals.stonesPerEnd) : (isQuick ? String(g.stonesPerEnd) : '')) });
  const endCount = el('input', { type: 'number', min: 1, max: 12, step: 1, inputmode: 'numeric', placeholder: String(g.endCount), value: (vals.endCount != null ? String(vals.endCount) : (isQuick ? String(g.endCount) : '')) });
  const untimedEnds = el('input', { type: 'number', min: 0, max: 6, step: 1, inputmode: 'numeric', placeholder: String(g.untimedEnds), value: (vals.untimedEnds != null ? String(vals.untimedEnds) : (isQuick ? String(g.untimedEnds) : '')) });

  const pregame = makePregameToggleControl(vals.pregameSeconds != null ? vals.pregameSeconds : (mode === 'quick' ? g.pregameSeconds : null), {
    defaultSeconds: g.pregameSeconds, defaultEnabled: mode === 'quick' ? g.pregameSeconds > 0 : false,
    allowInherit: mode !== 'quick', uncheckedValue: mode === 'quick' ? 0 : 0,
    hint: mode === 'quick' ? 'When enabled, set pre-game countdown minutes and seconds.' : 'Enable to set an override, or disable/leave blank to use existing default/global behavior where applicable.',
    onChange
  });
  const pace = el('input', { type: 'checkbox', checked: vals.paceOfPlay != null ? !!vals.paceOfPlay : !!g.paceEnabled });
  const showProgress = el('input', { type: 'checkbox', checked: vals.showProgress != null ? !!vals.showProgress : !!g.showProgress });
  const replaceConflicts = mode === 'quick' ? el('input', { type: 'checkbox', checked: !!vals.replaceConflicts }) : null;

  [gameDurationMin, gameDurationSec, warningAfterMin, warningAfterSec, stonesPerEnd, endCount, untimedEnds, pace, showProgress].forEach((i) => { if (!i) return; i.addEventListener('input', onChange); i.addEventListener('change', onChange); });
  if (replaceConflicts) replaceConflicts.addEventListener('change', onChange);

  const outputs = buildStandardizedOutputsUI({
    startRelays: vals.startRelays || [], startDuration: vals.startDuration,
    endRelays: vals.endRelays || [], endDuration: vals.endDuration,
    warningSubtitle: mode === 'quick' ? 'Outputs fired when the game expires/warning period ends (after full game time).' : 'Outputs fired at the game warning/expiration time (buzzer/warning event).',
    onChange
  });

  const isLeague = mode === 'league';
  const warningFieldLabel = '2. Game Warning, also called Expired time';
  const warningHint = mode === 'quick'
    ? 'Time in warning/expired state after buzzer (maps to quick start header_hold).'
    : mode === 'schedule'
      ? 'Warning/completed hold after buzzer (maps to schedule entry header_hold). Leave blank to use global/default behavior.'
      : 'League profiles have no per-league header_hold API field. This standardized slot maps to countdown warning minutes before buzzer (countdown_minutes_before). Seconds are rounded to whole minutes on save; leave blank/0 for no pre-buzzer warning countdown.';
  const timingHint = mode === 'quick'
    ? 'Full game time until the buzzer/warning event.'
    : isLeague
      ? 'Full game duration for this league profile (saved to game_duration_minutes; seconds rounded to nearest minute for API compatibility).'
      : 'Full game duration for this scheduled game (updates buzzer time from start).';
  const outputsWarningSubtitle = mode === 'quick'
    ? 'Outputs fired when the game expires/warning period ends (after full game time).'
    : isLeague
      ? 'Outputs fired at buzzer/countdown-warning event (maps to league end_relays/end_duration). Separate header_hold is global-only, not a league field.'
      : 'Outputs fired at the game warning/expiration time (buzzer/warning event).';
  try { const t = outputs.warning && outputs.warning.el && outputs.warning.el.querySelector('.outputs-row-head .muted.small'); if (t) t.textContent = outputsWarningSubtitle; } catch(e){}

  const timingCard = el('div', { class: 'card game-subcard' },
    el('div', { class: 'section-title', style: 'margin-bottom: 6px' }, 'Game timing'),
    el('div', { class: 'game-fields-grid' },
      el('div', { class: 'field game-field-span-2' }, el('label', {}, '1. Game Duration'), el('div', { class: 'duration' }, gameDurationMin, el('span', {}, 'min'), gameDurationSec, el('span', {}, 's')), el('div', { class: 'muted small' }, timingHint)),
      el('div', { class: 'field game-field-span-2' }, el('label', {}, warningFieldLabel), el('div', { class: 'duration' }, warningAfterMin, el('span', {}, 'min'), warningAfterSec, el('span', {}, isLeague ? 'sec (rounded to min on save)' : 's')), el('div', { class: 'muted small' }, warningHint)),
      el('div', { class: 'field' }, el('label', {}, '3. Stones per End'), stonesPerEnd),
      el('div', { class: 'field' }, el('label', {}, '4. Total Number of Ends'), endCount),
      el('div', { class: 'field' }, el('label', {}, '5. Ends after Expiration'), untimedEnds, el('div', { class: 'muted small' }, 'Extra ends played after expiration so the game can finish naturally.'))));

  const optionsCard = el('div', { class: 'card game-subcard' },
    el('div', { class: 'section-title', style: 'margin-bottom: 6px' }, 'Game options'),
    pregame.el,
    el('div', { class: 'game-option-block' }, el('label', { class: 'check' }, pace, 'Pace of play'), el('div', { class: 'muted small' }, 'Auto-calculated pace guidance when enabled for this context.')),
    el('div', { class: 'game-option-block' }, el('label', { class: 'check' }, showProgress, 'Show progress through end'), el('div', { class: 'muted small' }, 'Shows a graphical end-progress bar on the display/status mirror below pace of play and above the footer.')),
    replaceConflicts ? el('div', { class: 'game-option-block' }, el('label', { class: 'check' }, replaceConflicts, 'Replace Same-time Games'), el('div', { class: 'muted small' }, 'Override option: replace conflicting scheduled/quick games at the same time when starting this quick game.')) : null);

  const container = el('div', { class: 'standard-game-form' }, timingCard, optionsCard, outputs.el);

  function set(values = {}) {
    if (values.gameDurationSeconds != null) { const t = Math.max(0, Number(values.gameDurationSeconds)||0); gameDurationMin.value = String(Math.floor(t/60)); gameDurationSec.value = String(t%60); }
    if (values.gameDurationMinutesOnly != null) { gameDurationMin.value = String(Math.max(0, Number(values.gameDurationMinutesOnly)||0)); gameDurationSec.value = '0'; }
    if (values.warningSeconds != null) { const t = Math.max(0, Number(values.warningSeconds)||0); warningAfterMin.value = String(Math.floor(t/60)); warningAfterSec.value = String(t%60); }
    else if (values.warningSeconds === null && mode !== 'quick') { warningAfterMin.value = ''; warningAfterSec.value = ''; }
    stonesPerEnd.value = values.stonesPerEnd == null ? '' : String(values.stonesPerEnd);
    endCount.value = values.endCount == null ? '' : String(values.endCount);
    untimedEnds.value = values.untimedEnds == null ? '' : String(values.untimedEnds);
    if (values.paceOfPlay != null) pace.checked = !!values.paceOfPlay;
    if (values.showProgress != null) showProgress.checked = !!values.showProgress;
    if (replaceConflicts && values.replaceConflicts != null) replaceConflicts.checked = !!values.replaceConflicts;
    if (values.pregameSeconds !== undefined || values.pregameEnabled !== undefined) pregame.set(values.pregameSeconds, values.pregameEnabled);
    outputs.start.set(values.startRelays || [], values.startDuration);
    outputs.warning.set(values.endRelays || [], values.endDuration);
  }

  function apiRead() {
    const out = outputs.read();
    const durTotal = (parseInt(gameDurationMin.value || '0', 10) || 0) * 60 + (parseInt(gameDurationSec.value || '0', 10) || 0);
    const warningTotal = (warningAfterMin.value === '' && warningAfterSec.value === '') ? null : (parseInt(warningAfterMin.value || '0', 10) || 0) * 60 + (parseInt(warningAfterSec.value || '0', 10) || 0);
    const stones = stonesPerEnd.value === '' ? null : Math.max(1, parseInt(stonesPerEnd.value, 10) || 1);
    const ends = endCount.value === '' ? null : Math.max(1, parseInt(endCount.value, 10) || 1);
    const untimed = untimedEnds.value === '' ? null : Math.max(0, parseInt(untimedEnds.value, 10) || 0);
    const warningForMode = isLeague
      ? ((warningAfterMin.value === '' && warningAfterSec.value === '') ? 0 : Math.max(0, Math.round(((parseInt(warningAfterMin.value || '0', 10) || 0) * 60 + (parseInt(warningAfterSec.value || '0', 10) || 0)) / 60)))
      : warningTotal;
    return {
      gameDurationSeconds: durTotal,
      gameDurationMinutes: isLeague ? Math.max(0, Math.round(durTotal / 60)) : Math.floor(durTotal / 60),
      warningSeconds: warningForMode,
      countdownMinutesBefore: isLeague ? (warningForMode == null ? 0 : warningForMode) : null,
      stonesPerEnd: stones,
      endCount: ends,
      untimedEnds: untimed,
      paceOfPlay: pace.checked,
      showProgress: showProgress.checked,
      replaceConflicts: replaceConflicts ? !!replaceConflicts.checked : false,
      pregameSeconds: pregame.getSeconds(),
      startRelays: out.start.relays,
      startDuration: out.start.duration,
      endRelays: out.warning.relays,
      endDuration: out.warning.duration,
      pregame: pregame, pace, showProgress, replaceConflicts, outputs,
      gameDurationMin, gameDurationSec, warningAfterMin, warningAfterSec, stonesPerEnd, endCount, untimedEnds,
    };
  }

  set(vals);
  return { el: container, set, read: apiRead, inputs: { gameDurationMin, gameDurationSec, warningAfterMin, warningAfterSec, stonesPerEnd, endCount, untimedEnds, pace, showProgress, replaceConflicts, pregame, outputs } };
}


function buildQuickForm() {
  const g = gameGlobalDefaults();
  const name = el('input', { type: 'text', maxlength: 60, placeholder: 'QUICK START' });
  const button = el('button', { class: 'btn primary', type: 'submit' }, 'Start game');
  const hint = el('span', { class: 'muted small' });

  const previewLine = (label, value) => el('div', { style: 'display:flex;justify-content:space-between;gap:12px;font-size:13px;color:var(--text);margin:4px 0' },
    el('div', { style: 'color:var(--text-dim);font-size:11px;text-transform:uppercase;letter-spacing:0.05em' }, label),
    el('div', { style: 'text-align:right;font-weight:600;color:var(--text-strong)' }, value));
  const preview = el('div', { class: 'card game-subcard', style: 'margin: 0 0 14px; background:rgba(2,10,20,0.45); border:1px solid rgba(255,255,255,0.12); padding:16px' });

  const formGame = createStandardGameForm({
    mode: 'quick',
    values: {
      gameDurationSeconds: 0,
      warningSeconds: g.headerHold || 600,
      stonesPerEnd: g.stonesPerEnd,
      endCount: g.endCount,
      untimedEnds: g.untimedEnds,
      paceOfPlay: g.paceEnabled,
      showProgress: g.showProgress,
      pregameSeconds: g.pregameSeconds,
      pregameEnabled: g.pregameSeconds > 0,
      replaceConflicts: false,
      startRelays: [], endRelays: [], startDuration: null, endDuration: null,
    },
    onChange: () => refreshPreview(),
  });

  const refreshPreview = () => {
    const v = formGame.read();
    const gameSecs = v.gameDurationSeconds || 0;
    const warnSecs = v.warningSeconds == null ? (g.headerHold || 600) : v.warningSeconds;
    const total = gameSecs + warnSecs;
    const when = new Date(Date.now() + total * 1000);
    const startOut = v.startRelays.length ? v.startRelays.map((r) => `${r} ${v.startDuration == null ? '(default)' : v.startDuration + 's'}`).join(', ') : 'None';
    const endOut = v.endRelays.length ? v.endRelays.map((r) => `${r} ${v.endDuration == null ? '(default)' : v.endDuration + 's'}`).join(', ') : 'None';
    preview.replaceChildren(
      el('div', { class: 'section-title', style: 'margin:0 0 10px' }, 'Preview — settings that will be applied'),
      previewLine('Game Duration', gameSecs === 0 ? '—' : durationText(gameSecs)),
      previewLine('Game Warning / Expired time', durationText(warnSecs)),
      previewLine('Expires at', gameSecs === 0 ? '—' : pad(when.getHours()) + ':' + pad(when.getMinutes())),
      previewLine('Pre-game countdown', v.pregameSeconds > 0 ? durationText(v.pregameSeconds) : 'Off'),
      previewLine('Pace of play', v.paceOfPlay ? 'On' : 'Off'),
      previewLine('Show progress through end', v.showProgress ? 'On' : 'Off'),
      previewLine('Replace Same-time Games', v.replaceConflicts ? 'On' : 'Off'),
      previewLine('Stones per end', v.stonesPerEnd == null ? `${g.stonesPerEnd} (default)` : v.stonesPerEnd),
      previewLine('Total number of ends', v.endCount == null ? `${g.endCount} (default)` : v.endCount),
      previewLine('Ends after expiration', (v.untimedEnds || 0) > 0 ? `${v.untimedEnds} extra end(s)` : '0'),
      el('div', { style: 'margin-top:10px;border-top:1px solid rgba(255,255,255,0.10);padding-top:10px' }),
      previewLine('Outputs — Game Start', startOut),
      previewLine('Outputs — Game Warning', endOut)
    );
  };

  name.addEventListener('input', refreshPreview);

  const presets = QUICK_PRESETS.map((preset) => el('button', { class: 'btn preset', type: 'button',
    title: `${preset.title || preset.name}, ${durationText(preset.seconds)}`,
    onclick: () => launchQuick({ seconds: preset.seconds, name: preset.name, title: preset.title,
                                 game_completed: preset.game_completed, header_hold: preset.header_hold },
                               { pregame: formGame.read().pregameSeconds > 0, pace_of_play: formGame.read().paceOfPlay, replace_conflicts: formGame.read().replaceConflicts, show_progress_through_end: formGame.read().showProgress }) }, preset.label));

  const form = el('form', { class: 'card quick-game-card', onsubmit: (event) => {
      event.preventDefault();
      const v = formGame.read();
      if (!v.gameDurationSeconds || v.gameDurationSeconds < 1) return showMessage('Enter a full game duration of at least 1 second');
      const warnSecs = v.warningSeconds == null ? (gameGlobalDefaults().headerHold || 600) : v.warningSeconds;
      launchQuick({
        seconds: v.gameDurationSeconds,
        name: name.value.trim() || null,
        header_hold: warnSecs,
        ...{ pregame: v.pregameSeconds > 0, pace_of_play: v.paceOfPlay, replace_conflicts: v.replaceConflicts, show_progress_through_end: v.showProgress },
        pregame_countdown: v.pregameSeconds || 0,
        start_relays: v.startRelays,
        start_duration: v.startDuration,
        end_relays: v.endRelays,
        end_duration: v.endDuration,
        end_count: v.endCount == null ? g.endCount : v.endCount,
        stones_per_end: v.stonesPerEnd == null ? g.stonesPerEnd : v.stonesPerEnd,
        untimed_ends: v.untimedEnds == null ? g.untimedEnds : v.untimedEnds,
      });
    } },
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Quick Game'), el('h3', {}, 'Start a game now')),
      el('span', { class: 'pill' }, 'Ad hoc timer')),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Standardized game setup shared with Schedule and Leagues. New/unpopulated values resolve from global/default config.'),
    el('div', { class: 'card game-subcard', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'section-title', style: 'margin-bottom: 8px' }, 'Quick picks'),
      el('div', { class: 'row-actions', style: 'margin: 0; gap: 8px; flex-wrap: wrap' }, ...presets)),
    el('div', { class: 'card game-subcard', style: 'margin: 0; padding: 14px' },
      el('div', { class: 'field' }, el('label', {}, 'Game name shown on display'), name)),
    formGame.el,
    preview,
    el('div', { class: 'row-actions' }, button, hint));
  form._defaultsApplied = false;
  form.applyDefaults = () => {
    if (form._defaultsApplied) return;
    const gd = gameGlobalDefaults();
    formGame.set({
      warningSeconds: gd.headerHold || 600,
      stonesPerEnd: gd.stonesPerEnd,
      endCount: gd.endCount,
      untimedEnds: gd.untimedEnds,
      paceOfPlay: gd.paceEnabled,
      showProgress: gd.showProgress,
      pregameSeconds: gd.pregameSeconds,
      pregameEnabled: gd.pregameSeconds > 0,
    });
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
  const g = gameGlobalDefaults();
  const changed = () => setDirty(true);

  const startInput = el('input', { type: 'time', step: 60, value: hms(entry.start).slice(0, 5), required: true });
  const buzzerInput = el('input', { type: 'time', step: 60, value: hms(entry.buzzer).slice(0, 5), required: true });
  const nameInput = el('input', { type: 'text', value: entry.name || '', placeholder: 'League/game name', maxlength: 80,
    oninput: (event) => { entry.name = event.target.value.trim() || null; changed(); } });
  const gameCompletedInput = el('input', { type: 'text', value: entry.game_completed || '', placeholder: 'default (global header text)', maxlength: 80,
    oninput: (event) => { entry.game_completed = event.target.value.trim() || null; changed(); } });
  const manualCheck = el('input', { type: 'checkbox', checked: !!entry.manual_trigger, disabled: !isAddonEnabled('pushbutton'),
    onchange: (event) => { entry.manual_trigger = !!event.target.checked; changed(); } });

  const gameForm = createStandardGameForm({
    mode: 'schedule',
    values: {
      gameDurationSeconds: Math.max(0, (entry.buzzer || 0) - (entry.start || 0)),
      warningSeconds: entry.header_hold != null ? entry.header_hold : null,
      stonesPerEnd: entry.stones_per_end,
      endCount: entry.end_count,
      untimedEnds: entry.untimed_ends,
      paceOfPlay: entry.pace_of_play === true,
      showProgress: entry.show_progress_through_end === true,
      pregameSeconds: entry.pregame_countdown != null ? entry.pregame_countdown : null,
      pregameEnabled: entry.pregame_countdown != null ? Number(entry.pregame_countdown) > 0 : false,
      startRelays: Array.isArray(entry.start_relays) ? entry.start_relays : [],
      startDuration: entry.start_duration,
      endRelays: Array.isArray(entry.end_relays) ? entry.end_relays : [],
      endDuration: entry.end_duration,
    },
    onChange: (v) => {
      const s = parseTime(startInput.value);
      if (s != null && v.gameDurationSeconds > 0) {
        entry.buzzer = Math.min(s + v.gameDurationSeconds, 86340);
        buzzerInput.value = hms(entry.buzzer).slice(0, 5);
      }
      entry.header_hold = v.warningSeconds;
      entry.stones_per_end = v.stonesPerEnd;
      entry.end_count = v.endCount;
      entry.untimed_ends = v.untimedEnds;
      entry.pace_of_play = v.paceOfPlay ? true : (v.paceOfPlay === false ? false : null);
      entry.show_progress_through_end = v.showProgress ? true : null;
      entry.pregame_countdown = v.pregameSeconds; // null = inherit, 0 = explicitly off, >0 = override
      if (v.paceOfPlay && v.endCount && entry.buzzer > entry.start) {
        entry.pace_unit_duration = Math.round((entry.buzzer - entry.start) / v.endCount);
      } else if (!v.paceOfPlay) {
        entry.pace_unit_duration = null;
      }
      entry.start_relays = v.startRelays && v.startRelays.length ? v.startRelays : null;
      entry.start_duration = v.startRelays && v.startRelays.length ? v.startDuration : null;
      entry.end_relays = v.endRelays && v.endRelays.length ? v.endRelays : null;
      entry.end_duration = v.endRelays && v.endRelays.length ? v.endDuration : null;
      changed();
    }
  });

  const syncStartBuzzer = () => {
    const s = parseTime(startInput.value);
    const b = parseTime(buzzerInput.value);
    if (s != null) entry.start = s;
    if (b != null) entry.buzzer = b;
    const v = gameForm.read();
    if (s != null && b != null && b > s) {
      gameForm.inputs.gameDurationMin.value = String(Math.floor((b - s) / 60));
      gameForm.inputs.gameDurationSec.value = String((b - s) % 60);
    }
    if (v.paceOfPlay && (entry.end_count || v.endCount) && entry.buzzer > entry.start) {
      entry.pace_unit_duration = Math.round((entry.buzzer - entry.start) / (entry.end_count || v.endCount));
    }
    changed();
  };
  startInput.onchange = syncStartBuzzer;
  buzzerInput.onchange = syncStartBuzzer;

  const applyLeagueToEntry = (league) => {
    if (!league) { entry.league = null; changed(); rerender(); return; }
    entry.league = league.name;
    if (!entry.name) entry.name = league.name;
    if (league.header_text) entry.game_completed = league.header_text;
    if (league.game_duration_minutes != null && entry.start != null) {
      entry.buzzer = Math.min(entry.start + (league.game_duration_minutes || 0) * 60, 86340);
      buzzerInput.value = hms(entry.buzzer).slice(0, 5);
    }
    if (league.pregame_countdown_seconds != null) entry.pregame_countdown = league.pregame_countdown_seconds;
    if (league.pace_enabled != null) entry.pace_of_play = !!league.pace_enabled;
    if (league.stones_per_end != null) entry.stones_per_end = league.stones_per_end;
    if (league.end_count != null) entry.end_count = league.end_count;
    if (league.untimed_ends != null) entry.untimed_ends = league.untimed_ends;
    if (league.show_progress_through_end != null) entry.show_progress_through_end = league.show_progress_through_end;
    if (league.manual_trigger != null) entry.manual_trigger = !!league.manual_trigger;
    if (Array.isArray(league.start_relays)) entry.start_relays = [...league.start_relays];
    if (league.start_duration != null) entry.start_duration = league.start_duration;
    if (Array.isArray(league.end_relays)) entry.end_relays = [...league.end_relays];
    if (league.end_duration != null) entry.end_duration = league.end_duration;
    if (entry.pace_of_play === true && entry.end_count && entry.buzzer > entry.start) {
      entry.pace_unit_duration = Math.round((entry.buzzer - entry.start) / entry.end_count);
    }
    changed();
    rerender();
  };
  const leagueSelect = el('select', { onchange: (event) => applyLeagueToEntry((leagues || []).find((l) => l.name === event.target.value) || null) },
    el('option', { value: '', selected: !entry.league }, '(none)'),
    ...(leagues || []).map((l) => el('option', { value: l.name, selected: entry.league === l.name }, l.name)));

  const timingMeta = el('div', { class: 'card game-subcard schedule-meta-card' },
    el('div', { class: 'section-title', style: 'margin-bottom: 6px' }, 'Schedule timing & identity'),
    el('div', { class: 'grid2' },
      el('div', { class: 'field' }, el('label', {}, 'League'), leagueSelect),
      el('div', { class: 'field' }, el('label', {}, 'Game name'), nameInput),
      el('div', { class: 'field' }, el('label', {}, 'Start time'), startInput),
      el('div', { class: 'field' }, el('label', {}, 'Game Warning / buzzer time'), buzzerInput, el('div', { class: 'muted small' }, 'Warning Countdown Ends / buzzer time. Game Duration above is kept in sync with start time.')),
      el('div', { class: 'field' }, el('label', {}, 'Header when game completed'), gameCompletedInput),
      el('div', { class: 'field' }, el('label', { class: 'check', style: 'margin-top: 22px' }, manualCheck, 'Manual push-button trigger'))),
    el('div', { class: 'muted small' }, 'Standardized game fields below map to existing schedule entry fields. New/unpopulated fields should prefer league/global defaults; blank/inherit where supported by API semantics.'));

  return el('div', { class: 'entry entry-standardized' },
    el('div', { class: 'entry-head' },
      el('div', { class: 'entry-head-main' },
        el('span', { class: 'pill' }, hms(entry.start).slice(0,5) + '–' + hms(entry.buzzer).slice(0,5)),
        el('strong', { class: 'entry-title' }, entry.name || 'Untitled game'),
        entry.quick ? el('span', { class: 'pill quick' }, 'quick') : null),
      el('button', { class: 'btn icon danger', type: 'button', title: 'Delete game', onclick: () => { entries.splice(index, 1); changed(); rerender(); } }, '✕')),
    timingMeta,
    gameForm.el);
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
  { group: 'Global defaults & timing', title: 'Timer & schedule behavior', hint: 'Global defaults used across Quick Game, Schedule, and Leagues. Standardized game timing/options can inherit or override these values where supported.',
    fields: [
      ['config.enabled', 'bool', 'Timer enabled (master on/off)'],
      ['config.allow_midnight_wrap', 'bool', 'Allow timers past midnight'],
      ['config.mirror_scale', 'slider', 'On the Display Now: mirror scale', 10, 100, 5]] },
  { group: 'Global defaults & timing', title: 'Pre-Game Countdown (default)', hint: 'Default pre-game countdown used by standardized game forms when not overridden. Set 0 to disable by default.',
    fields: [
      ['config.pregame_countdown', 'pregame_hms', 'Default pre-game countdown', 0, 600]] },
  { group: 'Global defaults & timing', title: 'Pace of play (default)', hint: 'Default pace-of-play behavior and display settings. Schedule/league/game overrides may inherit these defaults.',
    fields: [
      ['config.pace_of_play.enabled', 'bool', 'Default pace of play enabled'],
      ['config.pace_of_play.unit_name', 'text', 'Unit name, e.g. END'],
      ['config.pace_of_play.unit_duration', 'minutes', 'Minutes per unit', 0.5, 43200],
      ['config.pace_of_play.font_size', 'int', 'Pace font size (pixels)', 0, 100000],
      ['config.pace_of_play.color', 'color', 'Pace color']] },
  { group: 'Global defaults & timing', title: 'Game progress & club info', hint: 'Progress bar appears below pace of play and above footer on the display/mirror when enabled for a game. Club URLs support league import defaults.',
    fields: [
      ['config.footer.entry_name', 'bool', 'Show game name in footer (supports progress context)'],
      ['config.club_website_url', 'text', 'Club website URL (Curling Club Manager)'],
      ['config.league_page_url', 'text', 'League page URL (optional override)']] },

  { group: 'Outputs & relays', title: 'Relay outputs & functions', hint: 'Enable outputs and assign relay functions. These function labels/names are shown in standardized Game Start/Game Warning output selectors.',
    fields: [
      ['config.relay_count', 'int', 'Number of relays', 0, 4]] },
  { group: 'Outputs & relays', title: 'Game Start outputs', hint: 'Condensed default outputs for game start. Per-game standardized forms can override selected outputs/durations.',
    fields: [
      ['config.buzzer.ring_at_start', 'relays', 'Game Start output'],
      ['config.buzzer.start_duration', 'output_duration', 'Game Start duration', 'start']] },
  { group: 'Outputs & relays', title: 'Game Warning outputs (countdown warning before buzzer)', hint: 'Standardized as "Game Warning" in game setup: this global setting controls warning/countdown outputs fired minutes before the buzzer for eligible games. Set minutes to 0 to disable the pre-buzzer warning countdown.',
    fields: [
      ['config.buzzer.countdown.minutes_before', 'int', 'Game Warning: minutes before buzzer (0 = off)', 0, 100000],
      ['config.buzzer.countdown.relays', 'relays', 'Game Warning output'],
      ['config.buzzer.countdown_duration', 'output_duration', 'Game Warning duration', 'countdown']] },
  { group: 'Outputs & relays', title: 'Game completion/expiration outputs', hint: 'Outputs fired at buzzer/expiration. Header hold after buzzer is a separate global setting (Quick Game/Schedule can override expired/warning hold via header_hold; leagues map warning timing to countdown-before-buzzer instead).',
    fields: [
      ['config.buzzer.relays', 'relays', 'Game completion output'],
      ['config.buzzer.end_duration', 'output_duration', 'Game completion duration', 'end'],
      ['config.buzzer.header_hold', 'int', 'Header hold after buzzer / expired state (seconds)', 0, 100000]] },

  { group: 'Display appearance', title: 'Pre-game countdown display', hint: 'Visual styling for pre-game countdown. Default timing is configured in Global defaults & timing.',
    fields: [
      ['config.pregame_blink', 'bool', 'Flash/blink pre-game countdown'],
      ['config.pregame_font_size', 'int', 'Font size', 0, 100000],
      ['config.pregame_header_color', 'color', 'Header color'],
      ['config.pregame_countdown_color', 'color', 'Countdown color'],
      ['config.pregame_header_text', 'text', 'Header text (e.g. Get ready)']] },
  { group: 'Display appearance', title: 'Header text',
    fields: [
      ['config.header.font_size', 'int', 'Font size (pixels)', 0, 100000],
      ['config.header.text.when_disabled', 'text', 'Text when disabled'],
      ['config.header.text.when_idle', 'text', 'Text when idle'],
      ['config.header.text.counting_down', 'text', 'Text while counting down'],
      ['config.header.text.game_completed', 'text', 'Text when warning/expired state starts'],
      ['config.header.text.game_ended', 'text', 'Text when game ends'],
      ['config.header.color.when_disabled', 'color', 'Color when disabled'],
      ['config.header.color.when_idle', 'color', 'Color when idle'],
      ['config.header.color.counting_down', 'color', 'Color while counting down'],
      ['config.header.color.game_completed', 'color', 'Color in warning/expired state'],
      ['config.header.color.game_ended', 'color', 'Color when game ends']] },
  { group: 'Display appearance', title: 'Countdown clock',
    fields: [
      ['config.countdown.font_size_4_digits', 'int', 'Timer font size under an hour', 0, 100000],
      ['config.countdown.font_size_5_digits', 'int', 'Timer font size an hour or more', 0, 100000],
      ['config.countdown.color.counting_down', 'color', 'Color while counting down'],
      ['config.countdown.color.game_completed', 'color', 'Color in warning/expired state'],
      ['config.countdown.color.game_ended', 'color', 'Color when game ends']] },
  { group: 'Display appearance', title: 'Footer & background',
    fields: [
      ['config.footer.font_size', 'int', 'Footer font size (pixels)', 0, 100000],
      ['config.footer.datetime', 'bool', 'Show date and time'],
      ['config.footer.color', 'color', 'Footer color'],
      ['config.background_color', 'color', 'Background color']] },

  { group: 'Hardware & integrations', title: 'Manual trigger (pushbutton)', hint: 'GPIO press rings the start relays for the next manually-triggered game. Only shown when the pushbutton add-on is enabled.',
    fields: [
      ['config.manual_trigger_gpio', 'int', 'Manual trigger GPIO (pushbutton add-on)', 0, 40]] },
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
    if (section.title === 'Relay outputs & functions') {
      const count = state.configValues['config.relay_count'] || 0;
      const relayFields = [];
      for (let i = 1; i <= count; i++) {
        relayFields.push(['config.relay' + i + '_enabled', 'bool', 'RELAY' + i + ' enabled']);
        relayFields.push(['config.relay' + i + '_function', 'select', 'RELAY' + i + ' function', 'None', 'Bell', 'Buzzer', 'Speaker', 'Light', 'Custom']);
      }
      fields = [['config.relay_count', 'int', 'Number of relays', 0, 4]].concat(relayFields);
    }
    const testEventForSection = (title) => {
      if (title === 'Game Start outputs') return 'Game Start';
      if (title === 'Game Warning outputs (countdown warning before buzzer)') return 'Countdown Warning';
      if (title === 'Game completion/expiration outputs') return 'Game End';
      return null;
    };
    const testEvent = testEventForSection(section.title);
    nodes.push(el('div', { class: 'card' },
      el('div', { class: 'section-title' }, section.group),
      el('h3', {}, section.title),
      section.hint ? el('p', { class: 'muted small', style: 'margin: 0' }, section.hint) : null,
      el('div', { class: 'grid2' }, fields.map(configField)),
      testEvent ? (isAddonEnabled('relays') ? el('button', { class: 'btn primary', onclick: (e) => testRelay(testEvent, e.target) }, 'Test') : null) : null));
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
  if (!state.configValues) await loadConfig();

  const saveLeagues = async (arr) => {
    const result = await api('/api/leagues', { leagues: arr });
    if (!result.ok) { showMessage('Save failed: ' + (result.data.error || 'unknown'), 'bad'); return false; }
    return true;
  };

  const makeLeagueEditor = (lp, opts = {}) => {
    const working = lp;
    const onAnyChange = () => { if (opts.onChange) opts.onChange(); };

    const nameInput = el('input', { type: 'text', value: working.name || '', maxlength: 80, placeholder: 'League name',
      oninput: (ev) => { working.name = ev.target.value.trim(); onAnyChange(); } });
    const headerInput = el('input', { type: 'text', value: working.header_text || '', maxlength: 80, placeholder: 'Header text shown when completed',
      oninput: (ev) => { working.header_text = ev.target.value.trim(); onAnyChange(); } });
    const manualCheck = el('input', { type: 'checkbox', checked: !!working.manual_trigger, disabled: !isAddonEnabled('pushbutton'),
      onchange: (ev) => { working.manual_trigger = !!ev.target.checked; onAnyChange(); } });
    const hasExtra = working.format != null || working.team_size != null;
    const formatInput = hasExtra ? el('input', { type: 'text', value: working.format || '', maxlength: 80, placeholder: 'Format (optional)',
      oninput: (ev) => { const v = ev.target.value.trim(); if (v) working.format = v; else delete working.format; onAnyChange(); } }) : null;
    const teamSizeInput = hasExtra ? el('input', { type: 'number', min: 0, max: 16, step: 1, value: working.team_size || '', placeholder: 'Team size (optional)',
      oninput: (ev) => { const n = parseInt(ev.target.value || '0', 10) || 0; if (n > 0) working.team_size = n; else delete working.team_size; onAnyChange(); } }) : null;

    const gameForm = createStandardGameForm({
      mode: 'league',
      values: {
        gameDurationSeconds: Math.max(0, Number(working.game_duration_minutes || 0) * 60),
        warningSeconds: Math.max(0, Number(working.countdown_minutes_before || 0) * 60),
        stonesPerEnd: working.stones_per_end != null ? working.stones_per_end : null,
        endCount: working.end_count != null ? working.end_count : null,
        untimedEnds: working.untimed_ends != null ? working.untimed_ends : null,
        paceOfPlay: working.pace_enabled != null ? !!working.pace_enabled : false,
        showProgress: working.show_progress_through_end != null ? !!working.show_progress_through_end : true,
        pregameSeconds: working.pregame_countdown_seconds != null ? Number(working.pregame_countdown_seconds) : 0,
        pregameEnabled: Number(working.pregame_countdown_seconds || 0) > 0,
        startRelays: Array.isArray(working.start_relays) ? working.start_relays : [],
        startDuration: working.start_duration != null ? working.start_duration : null,
        endRelays: Array.isArray(working.end_relays) ? working.end_relays : [],
        endDuration: working.end_duration != null ? working.end_duration : null,
      },
      onChange: () => syncToObject(),
    });

    const syncToObject = () => {
      const v = gameForm.read();
      working.name = (nameInput.value || '').trim();
      working.header_text = (headerInput.value || '').trim();
      working.game_duration_minutes = Math.max(0, parseInt(v.gameDurationMinutes != null ? v.gameDurationMinutes : Math.round((v.gameDurationSeconds || 0) / 60), 10) || 0);
      working.countdown_minutes_before = Math.max(0, parseInt(v.countdownMinutesBefore != null ? v.countdownMinutesBefore : (v.warningSeconds == null ? 0 : v.warningSeconds), 10) || 0);
      working.pregame_countdown_seconds = Math.max(0, parseInt(v.pregameSeconds || 0, 10) || 0);
      working.pace_enabled = !!v.paceOfPlay;
      working.show_progress_through_end = !!v.showProgress;
      working.stones_per_end = v.stonesPerEnd == null ? 8 : v.stonesPerEnd;
      working.end_count = v.endCount == null ? 8 : v.endCount;
      working.untimed_ends = v.untimedEnds == null ? 0 : v.untimedEnds;
      working.manual_trigger = !!manualCheck.checked;
      working.start_relays = Array.isArray(v.startRelays) ? v.startRelays : [];
      working.start_duration = (Array.isArray(v.startRelays) && v.startRelays.length && v.startDuration != null) ? v.startDuration : 0;
      working.end_relays = Array.isArray(v.endRelays) ? v.endRelays : [];
      working.end_duration = (Array.isArray(v.endRelays) && v.endRelays.length && v.endDuration != null) ? v.endDuration : 0;
      if (formatInput) { const val = (formatInput.value || '').trim(); if (val) working.format = val; else delete working.format; }
      if (teamSizeInput) { const n = parseInt(teamSizeInput.value || '0', 10) || 0; if (n > 0) working.team_size = n; else delete working.team_size; }
      onAnyChange();
    };

    syncToObject();

    const identityCard = el('div', { class: 'card game-subcard' },
      el('div', { class: 'section-title', style: 'margin-bottom: 6px' }, 'League identity'),
      el('div', { class: 'grid2' },
        el('div', { class: 'field game-field-span-2' }, el('label', {}, 'League name'), nameInput),
        el('div', { class: 'field game-field-span-2' }, el('label', {}, 'Header text when completed'), headerInput, el('div', { class: 'muted small' }, 'Shown on display when a game using this league completes (if applied).')),
        el('div', { class: 'field' }, el('label', { class: 'check', style: 'margin-top: 22px' }, manualCheck, 'Manual push-button trigger')),
        hasExtra ? el('div', { class: 'field' }, el('label', {}, 'Format (optional)'), formatInput) : null,
        hasExtra ? el('div', { class: 'field' }, el('label', {}, 'Team size (optional)'), teamSizeInput) : null),
      el('div', { class: 'muted small', style: 'margin-top: 8px' }, 'Standardized with Quick Game/Schedule. “Game Warning, also called Expired time” maps to league countdown warning before buzzer (countdown_minutes_before), since leagues have no header_hold field. Output durations left blank resolve to global/default when outputs are selected.'));
    manualCheck.addEventListener('change', syncToObject);

    const rootEl = el('div', { class: 'standard-game-form league-standard-form' }, identityCard, gameForm.el);
    return { el: rootEl, sync: syncToObject, gameForm, working };
  };

  const addDraft = {
    name: '', game_duration_minutes: 0, pregame_countdown_seconds: 0, header_text: '',
    stones_per_end: 8, end_count: 8, untimed_ends: 0, show_progress_through_end: true,
    pace_enabled: false, countdown_minutes_before: 0, manual_trigger: false,
    start_relays: [], start_duration: 0, end_relays: [], end_duration: 0,
  };
  const addEditor = makeLeagueEditor(addDraft, { isAdd: true });
  const addForm = el('form', { class: 'card', onsubmit: async (e) => {
    e.preventDefault();
    addEditor.sync();
    const name = (addDraft.name || '').trim();
    if (!name) { showMessage('Name required', 'bad'); return; }
    leagues.push(JSON.parse(JSON.stringify(addDraft)));
    if (!(await saveLeagues(leagues))) { leagues.pop(); return; }
    showMessage('League added', 'ok');
    renderLeagues();
  } },
    el('div', { class: 'section-title' }, 'Leagues'),
    el('h3', {}, 'Add league profile'),
    el('p', { class: 'muted small', style: 'margin: 0' }, 'Reusable league settings using the same standardized game components as Quick Game and Schedule.'),
    addEditor.el,
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
      const editor = makeLeagueEditor(lp, { isAdd: false });
      const badge = el('span', { class: 'fold-badge muted' },
        lp.manual_trigger && isAddonEnabled('pushbutton') ? 'manual trigger' :
        lp.manual_trigger && !isAddonEnabled('pushbutton') ? 'manual trigger (pushbutton disabled)' : '');
      const durTxt = lp.game_duration_minutes ? `${lp.game_duration_minutes} min` : '0 min';
      const warnTxt = ` • warning ${Number(lp.countdown_minutes_before || 0)}m before buzzer`;
      const note = el('span', { class: 'fold-note muted' }, durTxt + warnTxt);
      const body = el('div', { class: 'fold-body' },
        editor.el,
        el('div', { class: 'row-actions', style: 'margin-top: 10px' },
          el('button', { class: 'btn primary', onclick: async () => {
            editor.sync();
            if (!(lp.name || '').trim()) { showMessage('League name is required', 'bad'); return; }
            if (!(await saveLeagues(leagues))) return;
            state.editingLeagueIdx = null;
            renderLeagues();
          } }, 'Save'),
          el('button', { class: 'btn', onclick: () => { state.editingLeagueIdx = null; renderLeagues(); } }, 'Cancel'),
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
      leagues.push({ name: n, game_duration_minutes: 0, pregame_countdown_seconds: 0, header_text: '', pace_enabled: false, countdown_minutes_before: 0, format: '', team_size: 0, manual_trigger: false, stones_per_end: 8, end_count: 8, untimed_ends: 0, show_progress_through_end: true, start_relays: [], start_duration: 0, end_relays: [], end_duration: 0 });
      existing.add(n.toLowerCase());
      added++;
    });
    if (!(await saveLeagues(leagues))) return;
    renderLeagues();
    showMessage('Imported ' + added + ' new league(s).', 'ok');
  } }, 'Import from Club Manager');

  root.replaceChildren(
    el('div', { class: 'page-head' },
      el('div', {}, el('div', { class: 'section-title' }, 'Leagues'), el('h3', {}, 'Standardized league profiles')),
      el('span', { class: 'pill' }, `${leagues.length} league${leagues.length === 1 ? '' : 's'}`)),
    el('div', { class: 'row-actions', style: 'margin: 0 0 16px' }, importBtn),
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
    const localMeta = document.querySelector('meta[name="curling-timer-version"]');
    const current = (localMeta?.content || '{{version}}' || '').replace(/^v/, '');
    const v = document.querySelector('.header-version, .version'); if (v && latest) v.textContent = 'v' + latest;
    if (latest && current && latest !== current) {
      const dot = document.createElement('span');
      dot.className = 'version-dot';
      dot.title = `New version ${latest} available - click to view`;
      dot.onclick = () => window.open('https://github.com/jaydenrussell/esp32-curling-timer/releases', '_blank');
      const v2 = document.querySelector('.header-version, .version');
      if (v2 && !v2.querySelector('.version-dot')) v2.appendChild(dot);
    }
  } catch(e) {}
}
let stopStatusPoll = null;
let stopTick = null;
async function start() {
  renderTabs();
  selectTab(state.tab);
  await loadConfig();
  loadStatus();
  if (started) return;
  started = true;
  // tick loop for countdown/progress rendering (no /api polling) - recursive setTimeout, paused when hidden
  stopTick = startPoller(async () => { if (state.tab === 'status' && !document.hidden) tick(); }, { intervalMs: 1000, timeoutMs: 2500, immediate: true });
  // status polling: on status tab as original (1s visible)
  stopStatusPoll = startPoller(async ({ signal }) => {
    if (state.tab === 'status') await loadStatus({ signal, timeoutMs: 2500 });
  }, { intervalMs: 1000, timeoutMs: 2500, immediate: false });
  registerCleanup(() => { try { stopTick && stopTick(); } catch {} try { stopStatusPoll && stopStatusPoll(); } catch {} });
}
setUnauthorizedHandler(showLogin);
Promise.resolve().then(() => start());

if ('serviceWorker' in navigator) {
  navigator.serviceWorker.register('/sw.js').catch(() => {});
}

  return () => { for (const fn of cleanupFns.splice(0)) try { fn(); } catch {} };
}
