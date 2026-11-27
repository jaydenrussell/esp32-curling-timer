import sys

with open('external_components/web_editor/web_ui.html', 'r', encoding='utf-8') as f:
    content = f.read()

start_marker = 'async function renderDiagnostics() {'
start_idx = content.find(start_marker)

if start_idx < 0:
    print('FAILED: Could not find renderDiagnostics start')
    sys.exit(1)

# Find end of renderDiagnostics function by counting braces
depth = 0
start_pos = content.find('{', start_idx)
if start_idx < 0:
    print('FAILED: Could not find opening brace')
    sys.exit(1)

depth = 0
end_idx = -1
for i in range(start_idx, len(content)):
    ch = content[i]
    if ch == '{':
        depth += 1
    elif ch == '}':
        depth -= 1
        if depth == 0:
            end_idx = i + 1
            break

if end_idx < 0:
    print('FAILED: Could not find end of renderDiagnostics')
    sys.exit(1)

new_func = '''async function renderDiagnostics() {
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
'''

content = content[:start_idx] + new_func + content[end_idx:]

with open('external_components/web_editor/web_ui.html', 'w', encoding='utf-8') as f:
    f.write(content)

print('SUCCESS: Replaced renderDiagnostics function')