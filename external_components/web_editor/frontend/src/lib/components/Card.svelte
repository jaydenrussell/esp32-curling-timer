<script>
  let { title = '', subtitle = '', hint = '', action, extraClass = '', padding = 'md', children } = $props();
</script>

<article class="card {extraClass} {padding}">
  {#if title || subtitle || hint || action}
    <header class="card-header">
      <div class="card-title-group">
        {#if title}
          <h3>{title}</h3>
        {/if}
        {#if subtitle}
          <p class="card-subtitle">{subtitle}</p>
        {/if}
      </div>
      {#if hint}
        <p class="card-hint">{hint}</p>
      {/if}
      {#if action}
        <div class="card-action">{@render action()}</div>
      {/if}
    </header>
  {/if}
  <div class="card-content">
    {@render children?.()}
  </div>
</article>

<style>
  .card {
    background: var(--card-bg);
    border: 1px solid var(--border);
    border-radius: var(--radius-lg);
    overflow: hidden;
  }
  .card-header {
    padding: 1rem 1.25rem;
    border-bottom: 1px solid var(--border);
    display: flex;
    align-items: flex-start;
    justify-content: space-between;
    gap: 1rem;
    flex-wrap: wrap;
  }
  .card-title-group { display: flex; flex-direction: column; gap: 0.125rem; }
  .card-header h3 { margin: 0; font-size: var(--step1); font-weight: 600; color: var(--text); }
  .card-subtitle { margin: 0; font-size: var(--step--1); color: var(--text-muted); }
  .card-hint { margin: 0.25rem 0 0; font-size: var(--step--2); color: var(--text-muted); }
  .card-action { flex-shrink: 0; }
  .card-content { padding: 1rem 1.25rem; }
  .sm .card-header { padding: 0.75rem 1rem; }
  .sm .card-content { padding: 0.75rem 1rem; }
  .lg .card-header { padding: 1.25rem 1.5rem; }
  .lg .card-content { padding: 1.25rem 1.5rem; }
</style>