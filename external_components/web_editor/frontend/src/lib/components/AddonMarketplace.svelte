<script>
  import { state as appState, isAddonEnabled, pathToAddon, RELAY_NAMES, api, loadConfig, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Input from './Input.svelte';
  import Checkbox from './Checkbox.svelte';
  import Select from './Select.svelte';
  import Button from './Button.svelte';
  import Card from './Card.svelte';

  let { addonDetection = {}, configValues = {} } = $props();
  let addonDefs = $state({});

  onMount(async () => {
    const res = await api('/api/addons/definitions');
    if (res.ok) addonDefs = res.data || {};
  });

  const addonOrder = ['relays', 'pushbutton', 'grove_rtc', 'grove_lcd', 'i2c_rtc', 'i2c_lcd', 'spi_display', 'esp_hosted'];

  function isDetected(key) {
    return addonDetection[key]?.available === true;
  }

  function isEnabling(key) {
    const def = addonDefs[key];
    if (!def) return false;
    return def.config_enable && configValues[def.config_enable] === true;
  }

  function relayName(idx) {
    return RELAY_NAMES()[idx] || `RELAY${idx + 1}`;
  }

  async function toggleAddon(key, next) {
    const def = addonDefs[key];
    if (!def) return;
    const patch = {};
    if (def.config_enable) patch[def.config_enable] = next;
    if (def.config) patch[def.config] = next;
    if (def.extra) {
      for (const [k, v] of Object.entries(def.extra)) patch[k] = v;
    }
    const result = await api('/api/config', { version: appState.configVersion, config: patch });
    if (!result.ok) return showMessage(`Failed: ${result.data.error}`);
    showMessage(`${def.name} ${next ? 'enabled' : 'disabled'}`, 'ok');
    await loadConfig();
  }

  function getBorderColor(def) {
    const detected = isDetected(def.key);
    const conflicts = def.conflicts?.filter(c => isEnabling(c)) || [];
    if (detected) return 'var(--lime-green)';
    if (conflicts.length) return 'var(--house-red)';
    return 'var(--hack-yellow)';
  }

  function getConflicts(def) {
    return def.conflicts?.filter(c => isEnabling(c)) || [];
  }
</script>

<div id="tab-addons">
  <Card title="Add-ons" subtitle="Optional hardware features" hint="Enable only the hardware you have connected. Conflicting add-ons cannot be enabled together.">
    {#each addonOrder as key}
      {#if addonDefs[key]}
        <div class="addon-card" style="border-left-color: {getBorderColor(addonDefs[key])};">
          <div style="display: flex; align-items: flex-start; justify-content: space-between; gap: 1rem; flex-wrap: wrap;">
            <div>
              <div style="display: flex; align-items: center; gap: 0.5rem; flex-wrap: wrap;">
                <h4 style="margin: 0;">{addonDefs[key].name}</h4>
                {#if isDetected(key)}<span class="pill ok">Detected</span>{/if}
                {#if getConflicts(addonDefs[key]).length}<span class="pill bad">Conflicts: {getConflicts(addonDefs[key]).join(', ')}</span>{/if}
              </div>
              <p class="muted small" style="margin: 0.25rem 0 0;">{addonDefs[key].description}</p>
              {#if addonDefs[key].hint}<p class="muted small" style="margin: 0.25rem 0 0;">{addonDefs[key].hint}</p>{/if}
              {#if key === 'relays' && configValues['config.relay_count']}
                <div class="grid2" style="margin-top: 0.5rem; font-size: var(--step--2);">
                  <label style="display: flex; flex-direction: column; gap: 0.125rem;">
                    <span class="muted">Relay count</span>
                    <Input type="number" bind:value={configValues['config.relay_count']} min="0" max="12" step="1" inputmode="numeric" tabular style="width: 5rem;" oninput={() => appState.configDirty = true} />
                  </label>
                  <label style="display: flex; flex-direction: column; gap: 0.125rem;">
                    <span class="muted">Buzzer duration (ms)</span>
                    <Input type="number" bind:value={configValues['config.buzzer_duration_ms']} min="0" max="5000" step="10" inputmode="numeric" tabular style="width: 6rem;" oninput={() => appState.configDirty = true} />
                  </label>
                </div>
              {/if}
              {#if key === 'grove_rtc'}
                <p class="muted small" style="margin-top: 0.5rem;">Pins: SDA=GPIO53, SCL=GPIO54 (shared with relays — only one can be active)</p>
              {/if}
              {#if key === 'i2c_rtc'}
                <div class="grid2" style="margin-top: 0.5rem;">
                  <Input label="SDA" type="number" bind:value={configValues['config.i2c_sda']} min="0" max="47" step="1" inputmode="numeric" tabular />
                  <Input label="SCL" type="number" bind:value={configValues['config.i2c_scl']} min="0" max="47" step="1" inputmode="numeric" tabular />
                </div>
              {/if}
              {#if key === 'i2c_lcd'}
                <div class="grid2" style="margin-top: 0.5rem;">
                  <Input label="Address" type="number" bind:value={configValues['config.i2c_lcd_addr']} min="0" max="127" step="1" inputmode="numeric" tabular />
                  <Input label="SDA" type="number" bind:value={configValues['config.i2c_sda']} min="0" max="47" step="1" inputmode="numeric" tabular />
                  <Input label="SCL" type="number" bind:value={configValues['config.i2c_scl']} min="0" max="47" step="1" inputmode="numeric" tabular />
                </div>
              {/if}
              {#if key === 'pushbutton'}
                <div class="grid2" style="margin-top: 0.5rem;">
                  <Input label="Pin" type="number" bind:value={configValues['config.pushbutton_pin']} min="0" max="47" step="1" inputmode="numeric" tabular />
                  <Select label="Action" bind:value={configValues['config.pushbutton_action']} options={[
                    { value: 'none', label: 'None' },
                    { value: 'pause', label: 'Pause/Resume' },
                    { value: 'quick', label: 'Quick Start' }
                  ]} />
                </div>
              {/if}
              {#if key === 'esp_hosted'}
                <p class="muted small" style="margin-top: 0.5rem;">Provides Wi-Fi failover for ESP32-C6. Requires matching firmware in <code>firmware/network_adapter_esp32c6.bin</code>.</p>
              {/if}
            </div>
            <Checkbox
              checked={isEnabling(key)}
              disabled={getConflicts(addonDefs[key]).length && !isEnabling(key)}
              onchange={(e) => toggleAddon(key, e.detail)}
            />
          </div>
        </div>
      {/if}
    {/each}

    <div style="margin-top: 1rem; padding: 1rem; background: rgba(4,14,26,0.3); border-radius: var(--radius-md); border: 1px solid var(--border);">
      <h4 style="margin: 0 0 0.5rem;">Relay outputs & functions</h4>
      <p class="muted small" style="margin: 0 0 0.5rem;">Configure what each relay does when enabled. Changes apply immediately on save.</p>
      <div class="grid2">
        {#each Array.from({ length: configValues['config.relay_count'] || 0 }, (_, i) => i) as i}
          <div style="display: flex; flex-direction: column; gap: 0.125rem;">
            <label style="display: flex; align-items: center; gap: 0.375rem; font-weight: 500;">
              <Checkbox bind:checked={configValues[`config.relay${i + 1}_enabled`]} onchange={() => appState.configDirty = true} />
              <span>{relayName(i)}</span>
            </label>
            <Select
              label=""
              bind:value={configValues[`config.relay${i + 1}_function`]}
              options={[
                { value: 'None', label: 'None' },
                { value: 'Bell', label: 'Bell' },
                { value: 'Buzzer', label: 'Buzzer' },
                { value: 'Speaker', label: 'Speaker' },
                { value: 'Light', label: 'Light' },
                { value: 'Custom', label: 'Custom' }
              ]}
              onchange={() => appState.configDirty = true}
            />
          </div>
        {/each}
      </div>
    </div>
  </Card>
</div>