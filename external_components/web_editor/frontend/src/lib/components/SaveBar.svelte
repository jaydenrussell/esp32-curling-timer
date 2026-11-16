<script>
  import { state as appState } from '../stores.svelte.js';
  import Button from './Button.svelte';
  import { loadConfig, loadSettings } from '../stores.svelte.js';

  $: isDirty = appState.dirty || appState.configDirty;

  function handleDiscard() {
    if (confirm('Discard your unsaved changes?')) {
      if (appState.configDirty) loadConfig();
      if (appState.dirty) loadSettings();
    }
  }
</script>

{#if isDirty}
  <div class="save-bar show" role="status" aria-live="polite">
    <span>Unsaved changes</span>
    <Button variant="ghost" onclick={handleDiscard}>Discard</Button>
    <Button variant="primary" onclick={() => { /* handled by parent */ }}>Save & apply</Button>
  </div>
{/if}