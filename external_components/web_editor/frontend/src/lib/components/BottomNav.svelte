<script>
  import { state as appState, tabs, navLabels, navIcons } from '../stores.svelte.js';
  import { selectTab } from '../stores.svelte.js';

  const mobileTabs = tabs.slice(0, 5);
</script>

<nav class="bottom-nav" aria-label="Mobile navigation">
  {#each mobileTabs as [id, , ]}
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