<script>
  import { onMount } from 'svelte';
  import legacyBody from './legacy-body.html?raw';
  import { initLegacyApp } from './legacy-app.js';

  let container;

  onMount(() => {
    if (container) container.innerHTML = legacyBody;
    const cleanup = initLegacyApp();
    const meta = document.querySelector('meta[name="curling-timer-version"]');
    const ver = meta?.content || '{{version}}';
    document.querySelectorAll('.header-version, .version').forEach((el) => {
      if (el && (el.textContent.trim() === '{{version}}' || el.textContent.trim() === '')) el.textContent = ver;
    });
    return () => { try { cleanup && cleanup(); } catch {} };
  });
</script>

<div bind:this={container} id="legacy-root"></div>
