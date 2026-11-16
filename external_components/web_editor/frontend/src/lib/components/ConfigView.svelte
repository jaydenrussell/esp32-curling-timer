<script>
  import { state as appState, CONFIG_SECTIONS, isAddonEnabled, pathToAddon, loadConfig, api, validateConfigDiff, configDiff, toggleAddon, RELAY_NAMES, PYGAME_COLORS, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Input from './Input.svelte';
  import Checkbox from './Checkbox.svelte';
  import Select from './Select.svelte';
  import Button from './Button.svelte';
  import Card from './Card.svelte';
  import AddonMarketplace from './AddonMarketplace.svelte';

  let addonDetection = $state({});
  let showRaw = $state(false);
  let rawText = $state('');

  async function load() {
    await loadConfig();
    const result = await api('/api/addons/detection');
    if (result.ok) addonDetection = result.data || {};
    const rawResult = await api('/api/raw');
    if (rawResult.ok) rawText = rawResult.data.text;
  }

  onMount(() => { load(); });

  function getFieldType(type) {
    if (type === 'int' || type === 'float') return 'number';
    if (type === 'bool') return 'checkbox';
    if (type === 'color') return 'color';
    if (type === 'select') return 'select';
    if (type === 'relays') return 'relays';
    if (type === 'output_duration') return 'output_duration';
    if (type === 'pregame_hms') return 'pregame_hms';
    if (type === 'slider') return 'range';
    if (type === 'minutes') return 'number';
    return 'text';
  }

  function getFieldOptions(type, rest) {
    if (type === 'select') return rest;
    if (type === 'relays') return RELAY_NAMES().map((n, i) => ({ value: n || `RELAY${i + 1}`, label: n || `RELAY${i + 1}` }));
    return [];
  }

  async function saveConfig() {
    if (!appState.configVersion) return await loadConfig().then(saveConfig);
    const diff = configDiff();
    if (!Object.keys(diff).length) return appState.configDirty = false;
    const problem = validateConfigDiff(diff);
    if (problem) return showMessage(problem);
    const result = await api('/api/config', { version: appState.configVersion, config: diff });
    if (!result.ok) return showMessage(`Not saved: ${result.data.error}`);
    showMessage('Saved; the display updates within a second', 'ok');
    await loadConfig();
  }

  async function testRelay(event) {
    // handled by addon marketplace
  }
</script>

<svelte:head>
  <datalist id="pygame-colors">
    {#each PYGAME_COLORS as color}
      <option value={color}></option>
    {/each}
  </datalist>
</svelte:head>

<div id="tab-config">
  <div class="page-head">
    <div>
      <div class="section-title">Settings</div>
      <h3>Configuration & add-ons</h3>
    </div>
    <span class="pill">Device config</span>
  </div>

  <AddonMarketplace bind:addonDetection={addonDetection} bind:configValues={appState.configValues} on:toggleAddon={(e) => toggleAddon(e.detail.key, e.detail.next)} />

  {#if appState.configValues}
    {#each CONFIG_SECTIONS as section, sectionIndex}
      {#if section.title !== 'Manual trigger (pushbutton)' || isAddonEnabled('pushbutton', appState.configValues, addonDetection)}
        {#if sectionIndex === 0 || section.group !== CONFIG_SECTIONS[sectionIndex - 1]?.group}
          <div class="section-title" style="margin: 1rem 0 0; font-size: var(--step--2);">{section.group}</div>
        {/if}
        <Card>
          <div class="section-title">{section.group}</div>
          <h3>{section.title}</h3>
          {#if section.hint}<p class="muted small" style="margin: 0;">{section.hint}</p>{/if}
          <div class="grid2">
            {#if section.title === 'Relay outputs & functions'}
              {#each RELAY_NAMES() as relayName, i}
                <Checkbox
                  bind:checked={appState.configValues[`config.relay${i + 1}_enabled`]}
                  label={`${relayName || `RELAY${i + 1}`} enabled`}
                  onchange={() => appState.configDirty = true}
                />
                <Select
                  label={`${relayName || `RELAY${i + 1}`} function`}
                  bind:value={appState.configValues[`config.relay${i + 1}_function`]}
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
              {/each}
            {:else}
              {#each section.fields as [path, type, label, ...rest]}
                {#if type !== 'relays' && type !== 'output_duration' && type !== 'pregame_hms' && type !== 'slider' && type !== 'minutes'}
                  <Input
                    label={label}
                    type={getFieldType(type)}
                    bind:value={appState.configValues[path]}
                    tabular={type === 'int' || type === 'float'}
                    min={type === 'int' || type === 'float' ? rest[0] : undefined}
                    max={type === 'int' || type === 'float' ? rest[1] : undefined}
                    step={type === 'float' ? 0.1 : 1}
                    list={type === 'color' ? 'pygame-colors' : undefined}
                    options={getFieldOptions(type, rest)}
                    oninput={() => appState.configDirty = true}
                  />
                {/if}
              {/each}
            {/if}
          </div>
          {#if section.group.startsWith('Outputs')}
            {#if isAddonEnabled('relays', appState.configValues, addonDetection)}
              <Button variant="primary" onclick={(e) => testRelay(section.title, e.target)}>Test</Button>
            {/if}
          {/if}
        </Card>
      {/if}
    {/each}

    <Card>
      <div class="section-title">Advanced</div>
      <h3>Raw settings file</h3>
      <p class="muted small" style="margin: 0;">Full access to every setting (fonts, colours, relays…). Changes are checked before they are applied.</p>
      <textarea bind:value={rawText} spellcheck="false" style="width: 100%; min-height: 200px; border-radius: var(--radius-sm); border: 1px solid var(--border); background: rgba(4,14,26,0.5); color: var(--text); padding: 0.75rem; font: var(--step--2)/1.5 'JetBrains Mono', monospace;"></textarea>
      <div class="row-actions">
        <Button variant="secondary" onclick={async () => { const r = await api('/api/validate', { text: rawText }); showMessage(r.data.ok ? 'No errors found' : r.data.error, r.data.ok ? 'ok' : 'bad'); }}>Check</Button>
        <Button variant="primary" onclick={async () => { const r = await api('/api/raw', { version: appState.raw?.version, text: rawText }); if (!r.ok) return showMessage(`Not saved: ${r.data.error}`); showMessage('Saved; the display updates within a second', 'ok'); appState.model = null; appState.dirty = false; await load(); }}>Save & apply</Button>
        <Button variant="secondary" onclick={load}>Reload</Button>
        <Button variant="secondary" onclick={() => { const blob = new Blob([rawText], { type: 'text/yaml' }); const url = URL.createObjectURL(blob); const a = document.createElement('a'); a.href = url; a.download = 'settings.yaml'; a.click(); URL.revokeObjectURL(url); }}>Export YAML</Button>
        <label class="btn">Import YAML
          <input type="file" accept=".yaml,.yml,text/*" style="display:none" onchange={(e) => { const file = e.target.files[0]; if (!file) return; const r = new FileReader(); r.onload = () => { rawText = r.result; }; r.readAsText(file); }} />
        </label>
      </div>
    </Card>
  {/if}

  <div class="save-bar show" style="display: {appState.configDirty ? 'flex' : 'none'};" role="status" aria-live="polite">
    <span>Unsaved config changes</span>
    <Button variant="ghost" onclick={() => { if (confirm('Discard your unsaved config changes?')) load(); }}>Discard</Button>
    <Button variant="primary" onclick={saveConfig}>Save & apply</Button>
  </div>
</div>