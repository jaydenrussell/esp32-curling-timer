<script>
  import { state as appState, quickPresets, RELAY_NAMES, loadStatus, launchQuick, showMessage, shortTime, parseTime, durationText, pad } from '../stores.svelte.js';
  import { isAddonEnabled } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Input from './Input.svelte';
  import Checkbox from './Checkbox.svelte';
  import Button from './Button.svelte';
  import Card from './Card.svelte';

  let minutes = $state(0);
  let seconds = $state(0);
  let name = $state('QUICK START');
  let pregameMin = $state(5);
  let pregameSec = $state(0);
  let expiresMin = $state(10);
  let stonesPerEnd = $state(8);
  let endCount = $state(8);
  let untimedEnds = $state(0);
  let showProgress = $state(true);
  let pregameEnabled = $state(true);
  let paceEnabled = $state(true);
  let replaceConflicts = $state(false);
  let startBoxes = $state([]);
  let startDurs = $state([]);
  let endBoxes = $state([]);
  let endDurs = $state([]);
  let defaultsApplied = $state(false);
  let enabled = $state(false);
  let clockOk = $state(true);

  function relayNames() {
    return RELAY_NAMES();
  }

  let names = $derived.by(() => relayNames());
  let relayCount = $derived.by(() => names.length);
  let startBoxesArray = $derived.by(() => names.map(() => false));
  let startDursArray = $derived.by(() => names.map(() => ''));
  let endBoxesArray = $derived.by(() => names.map(() => false));
  let endDursArray = $derived.by(() => names.map(() => ''));

  onMount(() => {
    if (names.length !== startBoxes.length) {
      startBoxes = names.map(() => false);
      startDurs = names.map(() => '');
      endBoxes = names.map(() => false);
      endDurs = names.map(() => '');
    }
    if (appState.status) {
      enabled = appState.status.enabled && appState.status.clock_synced;
      clockOk = appState.status.clock_synced;
      if (enabled || clockOk) applyDefaults();
    }
  });

  $effect(() => {
    if (appState.status) {
      enabled = appState.status.enabled && appState.status.clock_synced;
      clockOk = appState.status.clock_synced;
      if ((enabled || clockOk) && !defaultsApplied) applyDefaults();
    }
  });

  function applyDefaults() {
    if (defaultsApplied) return;
    const st = appState.status || {};
    const cv = appState.configValues || {};
    const pg = Number((cv && cv['config.pregame_countdown'] != null) ? cv['config.pregame_countdown'] : (st.pregame_countdown_seconds != null ? st.pregame_countdown_seconds : 0)) || 0;
    pregameEnabled = pg > 0;
    pregameMin = Math.floor(pg / 60);
    pregameSec = pg % 60;
    const paceEn = cv && cv['config.pace_of_play.enabled'] != null ? !!cv['config.pace_of_play.enabled'] : true;
    paceEnabled = !!paceEn;
    showProgress = true;
    defaultsApplied = true;
  }

  function gameSeconds() {
    return (Number(minutes) || 0) * 60 + (Number(seconds) || 0);
  }

  function totalSeconds() {
    return gameSeconds() + (Number(expiresMin) || 10) * 60;
  }

  function expireTime() {
    const total = totalSeconds();
    const when = new Date(Date.now() + total * 1000);
    return `${pad(when.getHours())}:${pad(when.getMinutes())}`;
  }

  function startRelays() {
    return startBoxes.filter((b, i) => b).map((b, i) => names[i] || `RELAY${i + 1}`);
  }

  function endRelays() {
    return endBoxes.filter((b, i) => b).map((b, i) => names[i] || `RELAY${i + 1}`);
  }

  function startDur() {
    let m = 0;
    for (let i = 0; i < startBoxes.length; i++) {
      if (startBoxes[i]) {
        const v = Number(startDurs[i]) || 0;
        if (v > m) m = v;
      }
    }
    return m || null;
  }

  function endDur() {
    let m = 0;
    for (let i = 0; i < endBoxes.length; i++) {
      if (endBoxes[i]) {
        const v = Number(endDurs[i]) || 0;
        if (v > m) m = v;
      }
    }
    return m || null;
  }

  function handleSubmit(e) {
    e.preventDefault();
    const gs = gameSeconds();
    if (gs <= 0) return;
    launchQuick({
      seconds: gs,
      name: name.trim() || 'QUICK START',
      header_hold: (Number(expiresMin) || 10) * 60,
      pregame: pregameEnabled,
      pace_of_play: paceEnabled,
      replace_conflicts: replaceConflicts,
      show_progress_through_end: showProgress,
      pregame_countdown: pregameEnabled ? ((Number(pregameMin) || 0) * 60 + (Number(pregameSec) || 0)) : 0,
      start_relays: startRelays(),
      start_duration: startDur(),
      end_relays: endRelays(),
      end_duration: endDur(),
      end_count: Number(endCount) || 8,
      stones_per_end: Number(stonesPerEnd) || 8,
      untimed_ends: Number(untimedEnds) || 0,
    });
  }

  function usePreset(preset, opts) {
    minutes = Math.floor(preset.seconds / 60);
    seconds = preset.seconds % 60;
    name = preset.name;
    // opts are handled by the form state
  }
</script>

<div id="tab-quickstart">
  <Card title="Quick Game" subtitle="Start a game now" hint="Pick a preset or set the timing, options, and outputs below. The preview shows exactly what will be applied.">
    <form onsubmit={handleSubmit} class="standard-game-form">
      <div class="section-title" style="margin-bottom: 0.5rem;">Quick picks</div>
      <div class="row-actions" style="gap: 0.5rem; flex-wrap: wrap;">
        {#each quickPresets as preset}
          <Button variant="secondary" size="sm" type="button" onclick={() => usePreset(preset, { pregame: pregameEnabled, pace_of_play: paceEnabled, replace_conflicts: replaceConflicts, show_progress_through_end: showProgress })}>
            {preset.label}
          </Button>
        {/each}
      </div>

      <div class="grid2">
        <Input label="Pre-game countdown" type="number" bind:value={pregameMin} min="0" max="60" step="1" inputmode="numeric" aria-label="Pre-game countdown minutes" hint="Minutes" style="width: 5rem;" />
        <Input label="" type="number" bind:value={pregameSec} min="0" max="59" step="1" inputmode="numeric" aria-label="Pre-game countdown seconds" hint="Seconds" style="width: 5rem;" />
        <Input label="Full game time" type="number" bind:value={minutes} min="0" max="179" step="1" inputmode="numeric" hint="Minutes" style="width: 5rem;" />
        <Input label="" type="number" bind:value={seconds} min="0" max="59" step="1" inputmode="numeric" hint="Seconds" style="width: 5rem;" />
        <Input label="Expire time (minutes)" type="number" bind:value={expiresMin} min="0" max="60" step="1" inputmode="numeric" />
        <Input label="Name shown on display" bind:value={name} maxlength="60" />
      </div>

      <div class="grid2">
        <Input label="Stones per end (per team; mixed doubles = 5)" type="number" bind:value={stonesPerEnd} min="1" max="10" step="1" inputmode="numeric" tabular />
        <Input label="Total number of ends" type="number" bind:value={endCount} min="1" max="12" step="1" inputmode="numeric" tabular />
      </div>

      <Input label="Ends after expiration" type="number" bind:value={untimedEnds} min="0" max="6" step="1" inputmode="numeric" tabular hint="Extra ends after the buzzer so the game can finish naturally instead of cutting off mid-end." />

      <Card class="game-option-block">
        <div class="section-title" style="margin-bottom: 0.5rem;">Game options</div>
        <div class="row-actions" style="gap: 1rem; flex-wrap: wrap;">
          <Checkbox bind:checked={pregameEnabled} label="Enable pre-game countdown" />
          <Checkbox bind:checked={paceEnabled} label="Pace of play" />
          <Checkbox bind:checked={showProgress} label="Show progress through end" />
          <Checkbox bind:checked={replaceConflicts} label="Replace same-time games" />
        </div>
      </Card>

      {#if relayCount > 0}
        <Card class="game-option-block">
          <div class="section-title" style="margin-bottom: 0.5rem;">Outputs</div>
          <p class="muted small" style="margin: 0 0 0.625rem;">Choose which enabled relay outputs ring at Game Start and when the warning countdown ends, and for how many seconds. Leave a duration blank to use the global buzzer duration.</p>

          <div class="outputs-row-card">
            <div class="outputs-row-head">
              <span class="outputs-row-title">Game Start</span>
            </div>
            <div class="outputs-relay-grid">
              {#each names as relayName, i}
                <Checkbox bind:checked={startBoxes[i]} label={relayName || `RELAY${i + 1}`} class="outputs-relay-check" />
                <div class="outputs-dur-field">
                  <span class="muted small">Duration (s)</span>
                  <Input bind:value={startDurs[i]} type="number" min="0" max="999" step="1" inputmode="numeric" placeholder="0" tabular style="width: 5.5rem;" />
                </div>
              {/each}
            </div>
          </div>

          <div class="outputs-row-card">
            <div class="outputs-row-head">
              <span class="outputs-row-title">Warning Countdown Ends</span>
            </div>
            <div class="outputs-relay-grid">
              {#each names as relayName, i}
                <Checkbox bind:checked={endBoxes[i]} label={relayName || `RELAY${i + 1}`} class="outputs-relay-check" />
                <div class="outputs-dur-field">
                  <span class="muted small">Duration (s)</span>
                  <Input bind:value={endDurs[i]} type="number" min="0" max="999" step="1" inputmode="numeric" placeholder="0" tabular style="width: 5.5rem;" />
                </div>
              {/each}
            </div>
          </div>
        </Card>
      {:else}
        <Card class="game-option-block">
          <div class="section-title" style="margin-bottom: 0.5rem;">Outputs</div>
          <p class="muted small">No relay outputs enabled in Settings.</p>
        </Card>
      {/if}

      <Card style="background: rgba(2,10,20,0.35); border: 1px solid rgba(255,255,255,0.1);">
        <div class="section-title" style="margin: 0 0 0.625rem;">Preview — settings that will be applied</div>
        <div style="display: flex; flex-direction: column; gap: 0.375rem; font-size: var(--step--1);">
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Game time</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong); font-family: 'JetBrains Mono', monospace;">{gameSeconds() === 0 ? '—' : durationText(gameSeconds())}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Expires at</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong); font-family: 'JetBrains Mono', monospace;">{gameSeconds() === 0 ? '—' : expireTime()}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Pre-game countdown</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">{pregameEnabled ? ((Number(pregameMin)||0) + 'm ' + (Number(pregameSec)||0) + 's') : 'Off'}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Pace of play</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">{paceEnabled ? 'On' : 'Off'}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Progress through end</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">{showProgress ? 'On' : 'Off'}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Replace same-time games</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">{replaceConflicts ? 'On' : 'Off'}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Stones per end</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong); font-family: 'JetBrains Mono', monospace;">{stonesPerEnd || 8}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Total ends</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong); font-family: 'JetBrains Mono', monospace;">{endCount || 8}</span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Ends after expiration</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">{Number(untimedEnds) > 0 ? `${untimedEnds} extra end(s) played after the buzzer so the game can finish naturally` : 'Game ends at the buzzer'}</span>
          </div>
          <div style="border-top: 1px solid rgba(255,255,255,0.1); padding-top: 0.625rem; margin-top: 0.375rem;"></div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Outputs — Game Start</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">
              {#if startRelays().length}
                {startRelays().map((r, i) => r + ' ' + ((Number(startDurs[names.findIndex(n => n === r)]) || 0) || 'default') + 's').join(', ')}
              {:else}
                None
              {/if}
            </span>
          </div>
          <div style="display: flex; justify-content: space-between; gap: 1rem;">
            <span style="color: var(--text-dim); font-size: var(--step--2); text-transform: uppercase; letter-spacing: 0.05em;">Outputs — Game Expires</span>
            <span style="text-align: right; font-weight: 600; color: var(--text-strong);">
              {#if endRelays().length}
                {endRelays().map((r, i) => r + ' ' + ((Number(endDurs[names.findIndex(n => n === r)]) || 0) || 'default') + 's').join(', ')}
              {:else}
                None
              {/if}
            </span>
          </div>
        </div>
      </Card>

      <div class="row-actions">
        <Button variant="primary" type="submit" disabled={!enabled || gameSeconds() <= 0} style="flex: 1;">Start game</Button>
        <span class="muted small">{!enabled ? (clockOk ? 'Enable the timer first' : 'Waiting for the clock to sync') : ''}</span>
      </div>
    </form>
  </Card>
</div>