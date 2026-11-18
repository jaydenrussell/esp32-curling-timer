<script>
  import { state as appState, tabs, navLabels, navIcons, selectTab } from '../stores.svelte.js';
</script>

<nav class="bottom-nav" aria-label="Mobile navigation">
  {#each tabs as [id, , ]}
    <button
      class:active={appState.tab === id}
      onclick={() => selectTab(id)}
      aria-label={navLabels[id]}
      aria-current={appState.tab === id ? 'page' : undefined}
    >
      <span class="icon">{navIcons[id]}</span>
      <span class="label sr-only">{navLabels[id]}</span>
    </button>
  {/each}
</nav>