<script>
  import { state as appState, tabs, navLabels, navIcons } from '../stores.svelte.js';
  import { selectTab } from '../stores.svelte.js';
</script>

<nav class="tab-bar" aria-label="Primary navigation">
  {#each tabs as [id, , ]}
    <button
      class:active={appState.tab === id}
      onclick={() => selectTab(id)}
      title={navLabels[id]}
      aria-current={appState.tab === id ? 'page' : undefined}
      aria-label={navLabels[id]}
    >
      <span class="icon">{navIcons[id]}</span>
      <span class="label">{navLabels[id]}</span>
    </button>
  {/each}
</nav>