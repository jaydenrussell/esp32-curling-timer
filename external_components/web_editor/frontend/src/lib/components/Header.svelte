<script>
  import { state, navLabels, navIcons } from '../stores.svelte.js';
  import Button from './Button.svelte';

  export let version = '{{version}}';

  function handleLogout() {
    import('../stores.svelte.js').then(m => m.handleLogout());
  }

  function handleHelp() {
    import('../stores.svelte.js').then(m => m.handleHelp());
  }
</script>

<header>
  <div class="header-brand">
    <h1>⏱️ Curling Timer</h1>
    <span class="header-version">{version}</span>
  </div>
  {#if state.headerMsg.visible}
    <span id="header-msg" class="banner {state.headerMsg.kind}" style="flex: 1;">{state.headerMsg.text}</span>
  {:else}
    <span id="header-msg" class="banner hidden" style="flex: 1;"></span>
  {/if}
  <div class="header-actions">
    <Button variant="ghost" size="sm" onclick={handleHelp} aria-label="Help">Help</Button>
    <Button variant="ghost" size="sm" onclick={handleLogout} aria-label="Log out">Log out</Button>
  </div>
</header>