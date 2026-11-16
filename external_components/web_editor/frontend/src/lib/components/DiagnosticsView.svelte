<script>
  import { state as appState, api, formatBytes, formatUptime, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Card from './Card.svelte';
  import Button from './Button.svelte';

  let diag = $state(null);
  let log = $state([]);
  let refreshing = $state(false);
  let autoRefresh = $state(true);
  let interval = $state(null);

  async function fetchDiag() {
    refreshing = true;
    const [diagRes, logRes] = await Promise.all([
      api('/api/diagnostics'),
      api('/api/log')
    ]);
    if (diagRes.ok) diag = diagRes.data;
    if (logRes.ok) log = logRes.data.entries || [];
    refreshing = false;
  }

  onMount(() => {
    fetchDiag();
    if (autoRefresh) interval = setInterval(fetchDiag, 10000);
    return () => { if (interval) clearInterval(interval); };
  });

  function copyDiag() {
    navigator.clipboard.writeText(JSON.stringify(diag, null, 2));
    showMessage('Diagnostics copied', 'ok');
  }

  function copyLog() {
    navigator.clipboard.writeText(log.map(e => e.msg).join('\n'));
    showMessage('Log copied', 'ok');
  }
</script>

<div id="tab-diagnostics">
  <Card title="Diagnostics" subtitle="Device health, network, and logs" hint="Auto-refreshes every 10 seconds.">
    <div class="row-actions" style="margin-bottom: 0.5rem;">
      <Button variant="secondary" size="sm" onclick={fetchDiag} disabled={refreshing}>Refresh</Button>
      <Checkbox bind:checked={autoRefresh} label="Auto-refresh (10s)" />
      <Button variant="secondary" size="sm" onclick={copyDiag}>Copy JSON</Button>
      <Button variant="secondary" size="sm" onclick={copyLog}>Copy Log</Button>
    </div>

    {#if diag}
      <div class="diag-grid">
        <div class="diag-section">
          <h4>System</h4>
          <dl class="diag-list">
            <dt>Uptime</dt><dd>{formatUptime(diag.uptime || 0)}</dd>
            <dt>Free heap</dt><dd>{formatBytes(diag.free_heap || 0)}</dd>
            <dt>Min free heap</dt><dd>{formatBytes(diag.min_free_heap || 0)}</dd>
            <dt>PSRAM free</dt><dd>{diag.psram_free != null ? formatBytes(diag.psram_free) : 'N/A'}</dd>
            <dt>Firmware</dt><dd>{diag.firmware_version || 'Unknown'}</dd>
            <dt>IDF</dt><dd>{diag.idf_version || 'Unknown'}</dd>
            <dt>Chip</dt><dd>{diag.chip_model || 'Unknown'} rev {diag.chip_revision || '?'}</dd>
            <dt>CPU</dt><dd>{diag.cpu_freq_mhz || '?'} MHz, {diag.cpu_cores || '?'} cores</dd>
            <dt>Reset reason</dt><dd>{diag.reset_reason || 'Unknown'}</dd>
          </dl>
        </div>

        <div class="diag-section">
          <h4>Network</h4>
          <dl class="diag-list">
            <dt>Mode</dt><dd>{diag.network_mode || 'Ethernet'}</dd>
            <dt>IP</dt><dd>{diag.ip || 'Not connected'}</dd>
            <dt>Netmask</dt><dd>{diag.netmask || '—'}</dd>
            <dt>Gateway</dt><dd>{diag.gateway || '—'}</dd>
            <dt>DNS</dt><dd>{diag.dns || '—'}</dd>
            <dt>MAC</dt><dd>{diag.mac || '—'}</dd>
            <dt>Hostname</dt><dd>{diag.hostname || '—'}</dd>
            {#if diag.wifi}
              <dt>WiFi SSID</dt><dd>{diag.wifi.ssid || '—'}</dd>
              <dt>WiFi RSSI</dt><dd>{diag.wifi.rssi != null ? diag.wifi.rssi + ' dBm' : '—'}</dd>
              <dt>WiFi Channel</dt><dd>{diag.wifi.channel || '—'}</dd>
            {/if}
          </dl>
        </div>

        <div class="diag-section">
          <h4>Time</h4>
          <dl class="diag-list">
            <dt>Synced</dt><dd>{diag.time_synced ? 'Yes' : 'No'}</dd>
            <dt>Epoch</dt><dd>{diag.time_epoch || 0}</dd>
            <dt>ISO</dt><dd>{diag.time_iso || '—'}</dd>
            <dt>Timezone</dt><dd>{diag.timezone || '—'}</dd>
            <dt>SNTP server</dt><dd>{diag.sntp_server || '—'}</dd>
            <dt>DST active</dt><dd>{diag.dst_active ? 'Yes' : 'No'}</dd>
          </dl>
        </div>

        <div class="diag-section">
          <h4>Storage</h4>
          <dl class="diag-list">
            <dt>FS type</dt><dd>{diag.fs_type || 'LittleFS'}</dd>
            <dt>Total</dt><dd>{formatBytes(diag.fs_total || 0)}</dd>
            <dt>Used</dt><dd>{formatBytes(diag.fs_used || 0)}</dd>
            <dt>Free</dt><dd>{formatBytes(diag.fs_free || 0)}</dd>
          </dl>
        </div>

        <div class="diag-section">
          <h4>Display</h4>
          <dl class="diag-list">
            <dt>Enabled</dt><dd>{diag.display_enabled ? 'Yes' : 'No'}</dd>
            <dt>HDMI active</dt><dd>{diag.hdmi_active ? 'Yes' : 'No'}</dd>
            <dt>LT8912B</dt><dd>{diag.lt8912b_detected ? 'Detected' : 'Not found'}</dd>
            <dt>Resolution</dt><dd>{diag.hdmi_width || '?'}×{diag.hdmi_height || '?'}</dd>
            <dt>Refresh</dt><dd>{diag.hdmi_refresh || '?'} Hz</dd>
            <dt>Rotation</dt><dd>{diag.display_rotation || '0'}</dd>
            <dt>Mirror scale</dt><dd>{diag.mirror_scale != null ? diag.mirror_scale + '%' : '100%'}</dd>
          </dl>
        </div>

        <div class="diag-section">
          <h4>Relays</h4>
          <dl class="diag-list">
            {#each diag.relays || [] as relay, i}
              <dt>RELAY{i + 1}</dt>
              <dd>{relay.enabled ? 'Enabled' : 'Disabled'} — {relay.function || 'None'} {relay.state ? '(ON)' : '(OFF)'}</dd>
            {/each}
          </dl>
        </div>
      </div>
    {:else if refreshing}
      <p class="muted" style="text-align: center; padding: 2rem;">Loading diagnostics…</p>
    {/if}

    <Card style="margin-top: 1rem;">
      <h4>Recent Log</h4>
      <textarea spellcheck="false" readonly style="width: 100%; min-height: 200px; border-radius: var(--radius-sm); border: 1px solid var(--border); background: rgba(4,14,26,0.5); color: var(--text); padding: 0.75rem; font: var(--step--2)/1.5 'JetBrains Mono', monospace;">{log.map(e => `[${e.level}] ${e.msg}`).join('\n')}</textarea>
    </Card>
  </Card>
</div>