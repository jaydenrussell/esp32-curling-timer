import { api } from './api.js';
import { startPoller } from './poll.js';
import { CONFIG_SECTIONS, PYGAME_COLORS } from './constants.js';
export { api, CONFIG_SECTIONS, PYGAME_COLORS, ADDONS };

export const DAYS = ['Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday', 'Sunday'];
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
  leagues: '🏆',
  history: '📋',
  config: '⚙️',
  diagnostics: '🔧'
};
const TABS = [
  ['status', 'status', 'Status'],
  ['quickstart', 'quickstart', 'Quick Game'],
  ['schedule', 'schedule', 'Schedule'],
  ['leagues', 'leagues', 'Leagues'],
  ['history', 'history', 'Report'],
  ['config', 'config', 'Settings'],
  ['diagnostics', 'diagnostics', 'Diagnostics']
];

const QUICK_PRESETS = [
  { label: 'Practice (7m)', name: 'PRACTICE', title: 'Practice', seconds: 7 * 60, game_completed: '--- PRACTICE DONE ---', header_hold: 60 },
  { label: 'Game (6 ends)', name: 'Game (6 ends)', seconds: 75 * 60 },
  { label: 'Game (8 ends)', name: 'Game (8 ends)', seconds: 105 * 60 },
  { label: 'Doubles (8 ends)', name: 'Doubles (8 ends)', seconds: 105 * 60 },
];

const ADDONS = [
  { key: 'rtc', label: 'RTC time source', desc: 'Battery-backed clock (HYM8563 / PCF8563) — keeps time across power loss; timer knows the time on boot before SNTP syncs.', auto: true,
    configPaths: ['config.addons.rtc'],
    info: ['I2C: address 0x51 (HYM8563 / PCF8563)', 'Default build bus: SDA GPIO5, SCL GPIO6 (separate from GPIO relay pins)', 'Addon/example wiring may use Grove Port B (GPIO53/GPIO54); do not share those pins with GPIO relay outputs', 'No address conflict with common I2C relay modules (typically 0x25/0x26), but I2C relay modules are not supported by this firmware'] },
  { key: 'relays', label: 'Relay outputs', desc: 'GPIO relay outputs driving the bell circuit + aux (Unit 2Relay / direct GPIO).', auto: false,
    configPaths: ['config.addons.relays', 'config.relay_count', 'config.relay1_enabled', 'config.relay1_function', 'config.relay2_enabled', 'config.relay2_function', 'config.relay3_enabled', 'config.relay3_function', 'config.relay4_enabled', 'config.relay4_function', 'config.buzzer.ring_at_start', 'config.buzzer.countdown.relays', 'config.buzzer.relays'],
    info: ['Current GPIO pins (main config): RELAY1 GPIO53, RELAY2 GPIO54', 'Grove Port B HY2.0-4P wiring: G53/G54 — do not use same pins for I2C RTC/add-ons at the same time', 'Each selected output now pulses for its own configured/test duration', 'I2C addressable 2-relay modules (e.g. 0x25/0x26) are not supported by this firmware'] },
  { key: 'pushbutton', label: 'Manual trigger button', desc: 'GPIO press rings the start relays for the next manually-triggered game.', auto: false,
    configPaths: ['config.addons.pushbutton', 'config.manual_trigger_gpio'],
    info: ['Pin: GPIO45 (manual trigger input)', 'Setting: Manual trigger GPIO under Global options → Timer'] },
  { key: 'pahub', label: 'PaHub I2C hub', desc: 'PCA9548A 8-channel multiplexer — multiple I2C devices on one Grove port.', auto: true,
    configPaths: ['config.addons.pahub'],
    info: ['I2C: address 0x70 (PCA9548A)', 'Port: Grove Port A — SDA GPIO5, SCL GPIO6', 'Channels: 0–7 (one per I2C device)'] },
  { key: 'wifi', label: 'Wi-Fi failover', desc: 'ESP32-C6 SDIO Wi-Fi failover.', auto: true,
    configPaths: ['config.addons.wifi'],
    info: ['Module: ESP32-C6 SuperMini on SDIO header', 'Reference: docs/flash-c6.md'] },
  { key: 'uart', label: 'UART0 serial port', desc: 'Hat2-Bus UART0 serial (GPIO37 TX / GPIO38 RX) — one-line status stream to a serial device.', auto: false,
    configPaths: ['config.addons.uart'],
    info: ['Pins: TX GPIO37, RX GPIO38 (INPUT_PULLUP)', 'Hat: Hat2-Bus UART0'] },
];

const PATH_TO_ADDON = {};
for (const a of ADDONS) {
  for (const path of a.configPaths) PATH_TO_ADDON[path] = a.key;
}
for (const key of ['rtc', 'relays', 'pushbutton', 'pahub', 'wifi', 'uart']) {
  delete PATH_TO_ADDON[`config.addons.${key}`];
}

export function isAddonEnabled(key, configValues, addonDetection) {
  const a = ADDONS.find((x) => x.key === key);
  if (!a) return false;
  const intent = configValues && configValues[`config.addons.${key}`] === true;
  const detected = addonDetection && addonDetection[key] === true;
  return intent && (!a.auto || detected);
}

export const state = $state({
  tab: 'status',
  status: null,
  statusAt: 0,
  model: null,
  dirty: false,
  showPast: false,
  openDays: new Set(),
  openDates: new Set(),
  raw: null,
  configOriginal: null,
  configValues: null,
  configVersion: null,
  configDirty: false,
  editingLeagueIdx: null,
  leagues: null,
  addonDetection: {},
  messageTimer: 0,
  headerMsg: { text: '', kind: 'bad', visible: false },
  quickFormDefaultsApplied: false,
  authenticated: null,
  helpOpen: false,
});

export const tabs = TABS;
export const navLabels = NAV_LABELS;
export const navIcons = NAV_ICONS;
export const quickPresets = QUICK_PRESETS;
export const addons = ADDONS;
export const pathToAddon = PATH_TO_ADDON;

export async function selectTab(id) {
  state.tab = id;
  state.headerMsg = { text: '', kind: 'bad', visible: false };
  if (id === 'status') { await loadStatus(); }
  if (id === 'quickstart') { if (!state.configValues) await loadConfig(); await loadStatus(); }
  if (id === 'schedule') { if (!state.model) await loadSettings(); }
  if (id === 'leagues') { if (!state.model) await loadSettings(); }
  if (id === 'config') { if (!state.configOriginal || !state.configDirty) await loadConfig(); await loadRaw(); }
  if (id === 'history') { await loadHistory(); }
  if (id === 'diagnostics') { /* DiagnosticsView self-loads on mount */ }
}

export function showMessage(text, kind = 'bad') {
  state.headerMsg = { text, kind, visible: true };
  clearTimeout(state.messageTimer);
  if (kind === 'ok') {
    state.messageTimer = setTimeout(() => {
      state.headerMsg = { text: '', kind: 'bad', visible: false };
    }, 4000);
  }
}

export function clearMessage() {
  state.headerMsg = { text: '', kind: 'bad', visible: false };
  clearTimeout(state.messageTimer);
}

export async function loadStatus() {
  const result = await api('/api/status');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  state.status = result.data;
  state.statusAt = performance.now();
}

export async function loadConfig() {
  const result = await api('/api/config');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  state.configOriginal = result.data.config;
  state.configValues = JSON.parse(JSON.stringify(result.data.config));
  state.configVersion = result.data.version;
  state.configDirty = false;
  const detectionResult = await api('/api/addons/detection');
  if (detectionResult.ok) state.addonDetection = detectionResult.data || {};
}

export async function loadSettings() {
  const result = await api('/api/settings');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  state.model = result.data;
  state.dirty = false;
}

export async function loadRaw() {
  const result = await api('/api/raw');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  state.raw = result.data;
}

export async function loadHistory() {
  const result = await api('/api/backups');
  if (!result.ok) {
    if (result.status !== 401) showMessage(result.data.error);
    return;
  }
  // Render handled by component
}

export function setDirty(val = true) {
  state.dirty = val;
}

export function setConfigDirty(val = true) {
  state.configDirty = val;
}

export function configDiff() {
  const diff = {};
  if (!state.configOriginal || !state.configValues) return diff;
  for (const [key, value] of Object.entries(state.configValues)) {
    if (JSON.stringify(value) !== JSON.stringify(state.configOriginal[key])) {
      diff[key] = value;
    }
  }
  return diff;
}

export function validateConfigDiff(diff) {
  for (const [path, value] of Object.entries(diff)) {
    if (path === 'config.mirror_scale') {
      if (typeof value !== 'number' || !Number.isInteger(value) || value < 10 || value > 100)
        return 'Display mirror scale must be an integer between 10 and 100 (percentage)';
    } else if (path === 'config.progress_skip_multiplier') {
      if (typeof value !== 'number' || !Number.isFinite(value) || value < 0.1 || value > 5.0)
        return 'Progress bar skip multiplier must be a number between 0.1 and 5.0';
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

export function toggleEnabled(enabled) {
  return api('/api/config', { version: state.configVersion, config: { 'config.enabled': enabled } });
}

export function cancelQuick(cur) {
  return api('/api/quickstart', { action: 'cancel', game_id: cur.id });
}

export function launchQuick(payload) {
  return api('/api/quickstart', { action: 'start', ...payload });
}

export function testRelay(event, btn) {
  btn.disabled = true; btn.textContent = 'Testing...';
  api('/api/test_relay', { event }).finally(() => {
    btn.disabled = false; btn.textContent = 'Test';
  });
}

export function toggleAddon(key, next) {
  if (!state.configVersion) return loadConfig().then(() => toggleAddon(key, next));
  return api('/api/config', { version: state.configVersion, config: { [`config.addons.${key}`]: next } })
    .then(result => {
      if (!result.ok) throw new Error(result.data.error);
      showMessage('Saved; reloading configuration', 'ok');
      return loadConfig();
    })
    .catch(err => showMessage(`Not saved: ${err.message}`));
}

export function pad(n) { return String(n).padStart(2, '0'); }
export function hms(seconds) { const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60; return `${pad(h)}:${pad(m)}:${pad(s)}`; }
export function shortTime(seconds) { return hms(seconds).replace(/:00$/, '').replace(/^0/, ''); }
export function parseTime(value) { const match = /^(\d{1,2}):(\d{2})(?::(\d{2}))?$/.exec(value || ''); if (!match) return null; const secs = (+match[1]) * 3600 + (+match[2]) * 60 + (+(match[3] || 0)); return secs < 86400 ? secs : null; }
export function countdownText(seconds) { if (seconds <= 0) return '00:00'; const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60; return h ? `${h}:${pad(m)}:${pad(s)}` : `${pad(m)}:${pad(s)}`; }
export function todayIso() { const d = new Date(); return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())}`; }
export function weekdayOf(iso) { return DAYS[(new Date(iso + 'T12:00:00').getDay() + 6) % 7]; }
export function secondsNow() { const d = new Date(); return d.getHours() * 3600 + d.getMinutes() * 60 + d.getSeconds(); }
export function durationText(seconds) { if (seconds <= 0) return '0:00'; const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60; return h ? `${h}:${pad(m)}:${pad(s)}` : `${m}:${pad(s)}`; }

export function RELAY_NAMES() {
  if (!state.configValues) return [];
  const count = state.configValues['config.relay_count'] || 0;
  const names = [];
  for (let i = 1; i <= count; i++) {
    const fn = state.configValues[`config.relay${i}_function`];
    const en = state.configValues[`config.relay${i}_enabled`];
    if (en && fn && fn !== 'None') names.push(fn);
    else if (en) names.push(`RELAY${i}`);
    else names.push('');
  }
  return names;
}

export function start() {
  loadConfig().then(() => {
    if (state.configValues) {
      startPoller(loadStatus, { intervalMs: 5000 });
      selectTab('status');
    } else {
      state.authenticated = false;
    }
  });
}

export async function handleLogin(password) {
  const result = await api('/api/login', { password });
  if (!result.ok) {
    showMessage(result.data.error);
    return result.data.error;
  }
  state.authenticated = true;
  state.headerMsg = { text: '', kind: 'bad', visible: false };
  start();
  return null;
}

export async function handleLogout() {
  await api('/api/logout', {});
  location.reload();
}

export function handleHelp() {
  state.helpOpen = true;
}

export function closeHelp() {
  state.helpOpen = false;
}

export function formatBytes(bytes) {
  if (bytes === 0) return '0 B';
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

export function formatUptime(seconds) {
  if (seconds < 60) return `${seconds}s`;
  if (seconds < 3600) return `${Math.floor(seconds / 60)}m ${seconds % 60}s`;
  if (seconds < 86400) return `${Math.floor(seconds / 3600)}h ${Math.floor((seconds % 3600) / 60)}m`;
  const days = Math.floor(seconds / 86400);
  const hours = Math.floor((seconds % 86400) / 3600);
  return `${days}d ${hours}h`;
}

let statusPoller = null;
export function startPolling() {
  if (statusPoller) return;
  statusPoller = startPoller(loadStatus, { intervalMs: 5000 });
}

export function stopPolling() {
  if (statusPoller) {
    statusPoller();
    statusPoller = null;
  }
}

export const auth = {
  async check() {
    const result = await api('/api/status');
    if (!result.ok && result.status === 401) {
      state.authenticated = false;
      return false;
    }
    state.authenticated = true;
    return true;
  }
};

export function renderAddonMarketplace() {
  // Rendered by AddonMarketplace.svelte component
}