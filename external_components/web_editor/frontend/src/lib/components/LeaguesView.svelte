<script>
  import { state as appState, shortTime, parseTime, setDirty, loadSettings, api, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Input from './Input.svelte';
  import Checkbox from './Checkbox.svelte';
  import Button from './Button.svelte';
  import Card from './Card.svelte';

  let leagues = $state([]);
  let editingIdx = $state(null);

  async function loadLeagues() {
    const result = await api('/api/leagues');
    if (!result.ok) return;
    leagues = result.data.leagues || [];
    appState.leagues = leagues;
  }

  onMount(() => {
    loadLeagues();
  });

  function addLeague() {
    leagues = [...leagues, { name: '', club_website_url: '', league_page_url: '', season: '', division: '', team_name: '', schedule: {} }];
    editingIdx = leagues.length - 1;
  }

  function removeLeague(idx) {
    if (!confirm('Remove this league?')) return;
    leagues = leagues.filter((_, i) => i !== idx);
    if (editingIdx === idx) editingIdx = null;
    else if (editingIdx > idx) editingIdx--;
  }

  async function saveLeague(idx) {
    const league = leagues[idx];
    const result = await api('/api/leagues', { action: league.id ? 'update' : 'create', league });
    if (!result.ok) return showMessage(`Not saved: ${result.data.error}`);
    showMessage('League saved', 'ok');
    await loadLeagues();
    editingIdx = null;
  }

  async function importLeague(idx) {
    const league = leagues[idx];
    if (!league.club_website_url) return showMessage('Club website URL required');
    const result = await api('/api/leagues/import', { league_id: league.id });
    if (!result.ok) return showMessage(`Import failed: ${result.data.error}`);
    showMessage('League imported', 'ok');
    await loadLeagues();
  }
</script>

<div id="tab-leagues">
  <Card title="Leagues" subtitle="Curling Club Manager league import" hint="Configure league details and import schedules from Curling Club Manager.">
    <div class="row-actions" style="margin-bottom: 0.5rem;">
      <Button variant="secondary" size="sm" onclick={addLeague}>+ Add league</Button>
    </div>
    {#if leagues.length === 0}
      <p class="muted">No leagues configured. Click "Add league" to start.</p>
    {:else}
      <div class="league-list">
        {#each leagues as league, idx}
          <div class="league-row league-standard-form">
            <Input bind:value={league.name} placeholder="League name" style="min-width: 12rem;" />
            <Input bind:value={league.club_website_url} placeholder="Club website URL" type="url" style="flex: 1;" />
            <Input bind:value={league.league_page_url} placeholder="League page URL (optional)" type="url" style="flex: 1;" />
            <div style="display: flex; gap: 0.375rem;">
              {#if editingIdx === idx}
                <Button variant="primary" size="sm" onclick={() => saveLeague(idx)}>Save</Button>
                <Button variant="ghost" size="sm" onclick={() => editingIdx = null}>Cancel</Button>
              {:else}
                <Button variant="ghost" size="sm" onclick={() => editingIdx = idx}>Edit</Button>
                {#if league.id}
                  <Button variant="secondary" size="sm" onclick={() => importLeague(idx)}>Import</Button>
                {/if}
              {/if}
            </div>
          </div>
          {#if editingIdx === idx}
            <div class="game-subcard" style="margin-top: 0.5rem;">
              <div class="grid2">
                <Input label="Season" bind:value={league.season} placeholder="2024-2025" />
                <Input label="Division" bind:value={league.division} placeholder="A Division" />
                <Input label="Team name" bind:value={league.team_name} placeholder="Our Team" />
              </div>
            </div>
          {/if}
        {/each}
      </div>
    {/if}
  </Card>
</div>