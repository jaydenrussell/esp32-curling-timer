<script>
  import { api, showMessage } from '../stores.svelte.js';
  import { onMount } from 'svelte';
  import Card from './Card.svelte';
  import Button from './Button.svelte';

  let backups = $state([]);
  let loading = $state(false);
  let viewContent = $state('');

  async function load() {
    loading = true;
    try {
      const result = await api('/api/backups');
      if (!result.ok) return showMessage(result.data.error);
      backups = result.data.backups || [];
    } finally {
      loading = false;
    }
  }

  onMount(() => { load(); });

  async function viewBackup(name) {
    if (viewContent) { viewContent = ''; return; }
    const result = await api(`/api/backups/${encodeURIComponent(name)}`);
    if (!result.ok) return showMessage(result.data.error);
    viewContent = result.data.text;
  }

  async function restoreBackup(name) {
    const confirmed = confirm(`Restore settings from this backup? Your current file is backed up before restoring.`);
    if (!confirmed) return;
    const raw = await api('/api/raw');
    if (!raw.ok) return showMessage(raw.data.error);
    const restored = await api('/api/restore', { name, version: raw.data.version || '' });
    if (!restored.ok) return showMessage(`Not restored: ${restored.data.error}`);
    showMessage('Restored; the display updates within a second', 'ok');
    backups = backups.filter((b) => b.name !== name);
    viewContent = '';
  }

  function fmtTime(mtime) {
    if (!mtime) return '—';
    return new Date(mtime * 1000).toLocaleString();
  }
</script>

<div id="tab-history">
  <Card>
    <div class="section-title">History & backups</div>
    <p class="muted small" style="margin: 0;">A copy is kept automatically before every change (latest 30). Restore creates a backup of the current file first.</p>
  </Card>

  <Card>
    <div class="section-title">Previous versions</div>
    {#if loading}
      <p class="muted">Loading…</p>
    {:else if backups.length === 0}
      <p class="muted" style="margin: 0;">No backups yet.</p>
    {:else}
      {#each backups as backup}
        <div style="display:flex; align-items:center; justify-content:space-between; gap:0.5rem; border-bottom:1px solid var(--border); padding:0.5rem 0;">
          <span style="color:var(--text-dim);">{fmtTime(backup.mtime)}</span>
          <span class="muted small" style="color:var(--text-dim);">{backup.size || '—'} bytes</span>
          <div style="display:flex; gap:0.375rem;">
            <Button variant="secondary" onclick={() => viewBackup(backup.name)} style="padding:0.25rem 0.625rem; font-size:var(--step--3);">View</Button>
            <Button variant="primary" onclick={() => restoreBackup(backup.name)} style="padding:0.25rem 0.625rem; font-size:var(--step--3);">Restore</Button>
          </div>
        </div>
      {/each}
    {/if}
  </Card>

  {#if viewContent}
    <Card>
      <div class="section-title">Backup content</div>
      <textarea spellcheck="false" style="width:100%; min-height:200px; border-radius:var(--radius-sm); border:1px solid var(--border); background:rgba(4,14,26,0.5); color:var(--text); padding:0.75rem; font:var(--step--2)/1.5 'JetBrains Mono', monospace;" readonly>{viewContent}</textarea>
      <Button variant="secondary" onclick={() => { viewContent = ''; }} style="margin-top:0.5rem;">Close</Button>
    </Card>
  {/if}
</div>
