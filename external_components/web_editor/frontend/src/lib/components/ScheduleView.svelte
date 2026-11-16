<script>
  import { state as appState, DAYS, shortTime, parseTime, setDirty, loadSettings, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Input from './Input.svelte';
  import Checkbox from './Checkbox.svelte';
  import Button from './Button.svelte';
  import Card from './Card.svelte';

  let openDays = $state(new Set());
  let openDates = $state(new Set());
  let showPast = $state(false);
  let weeklySchedule = $state({});
  let exceptions = $state([]);

  function initSchedule() {
    if (!appState.model) return;
    weeklySchedule = JSON.parse(JSON.stringify(appState.model.schedule || {}));
    exceptions = JSON.parse(JSON.stringify(appState.model.exceptions || []));
    for (const d of DAYS) {
      if (!weeklySchedule[d]) weeklySchedule[d] = [];
    }
  }

  onMount(() => {
    initSchedule();
  });

  $effect(() => {
    if (appState.model && !appState.dirty) initSchedule();
  });

  function entryKey(day, idx) { return `w-${day}-${idx}`; }
  function excEntryKey(excIdx, entryIdx) { return `e-${excIdx}-${entryIdx}`; }

  function addEntry(day) {
    weeklySchedule[day] = [...(weeklySchedule[day] || []), { start: 0, buzzer: 0, name: '', quick: false, header_hold: null, pregame_countdown: null, pace_of_play: true, replace_conflicts: false, show_progress_through_end: true, start_relays: [], start_duration: null, end_relays: [], end_duration: null, end_count: 8, stones_per_end: 8, untimed_ends: 0 }];
    setDirty();
  }

  function removeEntry(day, idx) {
    weeklySchedule[day] = weeklySchedule[day].filter((_, i) => i !== idx);
    setDirty();
  }

  function addException() {
    const today = new Date().toISOString().split('T')[0];
    exceptions = [...exceptions, { date: today, entries: [] }];
    setDirty();
  }

  function removeException(idx) {
    exceptions = exceptions.filter((_, i) => i !== idx);
    setDirty();
  }

  function addExcEntry(excIdx) {
    exceptions[excIdx].entries = [...(exceptions[excIdx].entries || []), { start: 0, buzzer: 0, name: '', quick: false, header_hold: null, pregame_countdown: null, pace_of_play: true, replace_conflicts: false, show_progress_through_end: true, start_relays: [], start_duration: null, end_relays: [], end_duration: null, end_count: 8, stones_per_end: 8, untimed_ends: 0 }];
    setDirty();
  }

  function removeExcEntry(excIdx, entryIdx) {
    exceptions[excIdx].entries = exceptions[excIdx].entries.filter((_, i) => i !== entryIdx);
    setDirty();
  }

  function sortEntries(arr) {
    return [...arr].sort((a, b) => a.start - b.start);
  }

  function validateModel() {
    const lists = [...DAYS.map((d) => [d, weeklySchedule[d]]), ...exceptions.map((e) => [e.date, e.entries])];
    for (const [where, entries] of lists) {
      for (const e of entries) {
        if (e.buzzer <= e.start) return `${where}: buzzer ${shortTime(e.buzzer)} is not after start ${shortTime(e.start)}`;
      }
    }
    return null;
  }

  async function saveSchedule() {
    const problem = validateModel();
    if (problem) return showMessage(problem);
    const result = await api('/api/settings', { version: appState.model.version, schedule: weeklySchedule, exceptions });
    if (!result.ok) return showMessage(`Not saved: ${result.data.error}`);
    showMessage('Saved; the display updates within a second', 'ok');
    await loadSettings();
  }
</script>

<div id="tab-schedule">
  <Card title="Weekly Schedule" subtitle="Recurring games by weekday" hint="Games repeat every week. Use Exceptions for one-off changes.">
    <div class="row-actions" style="margin-bottom: 0.5rem;">
      <Checkbox bind:checked={showPast} label="Show past games" />
    </div>
    {#each DAYS as day}
      <details class="fold" open={openDays.has(day)}>
        <summary onclick={() => openDays.has(day) ? openDays.delete(day) : openDays.add(day)}>
          <span class="chevron">▸</span>
          <span class="fold-date">{day}</span>
          <span class="fold-count muted">{(weeklySchedule[day] || []).length} game{(weeklySchedule[day] || []).length !== 1 ? 's' : ''}</span>
        </summary>
        <div class="fold-body">
          {#each sortEntries(weeklySchedule[day] || []) as entry, idx}
            <div class="entry-standardized">
              <div class="entry-head">
                <div class="entry-head-main">
                  <span class="entry-title">{shortTime(entry.start)} – {shortTime(entry.buzzer)}</span>
                  <span class="pill">{entry.name || 'Untitled'}</span>
                  {#if entry.quick}<span class="pill quick">quick</span>{/if}
                  {#if showPast && entry.buzzer + (entry.header_hold || 0) < (new Date().getHours()*3600 + new Date().getMinutes()*60 + new Date().getSeconds())}<span class="pill bad">past</span>{/if}
                </div>
                <div class="row-actions">
                  <Button variant="ghost" size="sm" onclick={() => removeEntry(day, idx)}>Remove</Button>
                </div>
              </div>
              <div class="grid2" style="margin-top: 0.5rem;">
                <Input label="Start" type="time" bind:value={entry.start} oninput={() => { entry.start = parseTime(entry.start) || 0; setDirty(); }} />
                <Input label="Buzzer" type="time" bind:value={entry.buzzer} oninput={() => { entry.buzzer = parseTime(entry.buzzer) || 0; setDirty(); }} />
                <Input label="Name" bind:value={entry.name} maxlength="60" />
                <Input label="Header hold (s)" type="number" bind:value={entry.header_hold} min="0" max="300" step="1" inputmode="numeric" tabular />
              </div>
              <div class="grid2">
                <Input label="Pre-game (s)" type="number" bind:value={entry.pregame_countdown} min="0" max="3600" step="1" inputmode="numeric" tabular />
                <Input label="Stones/end" type="number" bind:value={entry.stones_per_end} min="1" max="10" step="1" inputmode="numeric" tabular />
                <Input label="Total ends" type="number" bind:value={entry.end_count} min="1" max="12" step="1" inputmode="numeric" tabular />
                <Input label="Untimed ends" type="number" bind:value={entry.untimed_ends} min="0" max="6" step="1" inputmode="numeric" tabular />
              </div>
              <div class="game-option-block">
                <div class="row-actions" style="gap: 1rem; flex-wrap: wrap;">
                  <Checkbox bind:checked={entry.pace_of_play} label="Pace of play" />
                  <Checkbox bind:checked={entry.replace_conflicts} label="Replace conflicts" />
                  <Checkbox bind:checked={entry.show_progress_through_end} label="Show progress" />
                  <Checkbox bind:checked={entry.quick} label="Quick start" />
                </div>
              </div>
            </div>
          {/each}
          <div class="row-actions" style="margin-top: 0.5rem;">
            <Button variant="secondary" size="sm" onclick={() => addEntry(day)}>+ Add game</Button>
          </div>
        </div>
      </details>
    {/each}
  </Card>

  <Card title="Date Exceptions" subtitle="One-off schedule overrides" hint="These replace the weekly schedule for the specific date.">
    <div class="row-actions" style="margin-bottom: 0.5rem;">
      <Button variant="secondary" size="sm" onclick={addException}>+ Add exception</Button>
    </div>
    {#each exceptions as exception, excIdx}
      <details class="fold" open={openDates.has(exception.date)}>
        <summary onclick={() => openDates.has(exception.date) ? openDates.delete(exception.date) : openDates.add(exception.date)}>
          <span class="chevron">▸</span>
          <span class="fold-date">{exception.date}</span>
          <span class="fold-count muted">{(exception.entries || []).length} game{(exception.entries || []).length !== 1 ? 's' : ''}</span>
        </summary>
        <div class="fold-body">
          <Input label="Date" type="date" bind:value={exception.date} style="max-width: 14rem; margin-bottom: 0.5rem;" />
          {#each sortEntries(exception.entries || []) as entry, entryIdx}
            <div class="entry-standardized">
              <div class="entry-head">
                <div class="entry-head-main">
                  <span class="entry-title">{shortTime(entry.start)} – {shortTime(entry.buzzer)}</span>
                  <span class="pill">{entry.name || 'Untitled'}</span>
                  {#if entry.quick}<span class="pill quick">quick</span>{/if}
                </div>
                <div class="row-actions">
                  <Button variant="ghost" size="sm" onclick={() => removeExcEntry(excIdx, entryIdx)}>Remove</Button>
                </div>
              </div>
              <div class="grid2" style="margin-top: 0.5rem;">
                <Input label="Start" type="time" bind:value={entry.start} oninput={() => { entry.start = parseTime(entry.start) || 0; setDirty(); }} />
                <Input label="Buzzer" type="time" bind:value={entry.buzzer} oninput={() => { entry.buzzer = parseTime(entry.buzzer) || 0; setDirty(); }} />
                <Input label="Name" bind:value={entry.name} maxlength="60" />
                <Input label="Header hold (s)" type="number" bind:value={entry.header_hold} min="0" max="300" step="1" inputmode="numeric" tabular />
              </div>
              <div class="grid2">
                <Input label="Pre-game (s)" type="number" bind:value={entry.pregame_countdown} min="0" max="3600" step="1" inputmode="numeric" tabular />
                <Input label="Stones/end" type="number" bind:value={entry.stones_per_end} min="1" max="10" step="1" inputmode="numeric" tabular />
                <Input label="Total ends" type="number" bind:value={entry.end_count} min="1" max="12" step="1" inputmode="numeric" tabular />
                <Input label="Untimed ends" type="number" bind:value={entry.untimed_ends} min="0" max="6" step="1" inputmode="numeric" tabular />
              </div>
              <div class="game-option-block">
                <div class="row-actions" style="gap: 1rem; flex-wrap: wrap;">
                  <Checkbox bind:checked={entry.pace_of_play} label="Pace of play" />
                  <Checkbox bind:checked={entry.replace_conflicts} label="Replace conflicts" />
                  <Checkbox bind:checked={entry.show_progress_through_end} label="Show progress" />
                  <Checkbox bind:checked={entry.quick} label="Quick start" />
                </div>
              </div>
            </div>
          {/each}
          <div class="row-actions" style="margin-top: 0.5rem;">
            <Button variant="secondary" size="sm" onclick={() => addExcEntry(excIdx)}>+ Add game</Button>
            <Button variant="danger" size="sm" onclick={() => removeException(excIdx)}>Remove exception</Button>
          </div>
        </div>
      </details>
    {/each}
  </Card>

  <div class="save-bar show" role="status" aria-live="polite" style="display: {appState.dirty ? 'flex' : 'none'};">
    <span>Unsaved schedule changes</span>
    <Button variant="ghost" onclick={() => { if (confirm('Discard your unsaved changes?')) loadSettings(); }}>Discard</Button>
    <Button variant="primary" onclick={saveSchedule}>Save & apply</Button>
  </div>
</div>