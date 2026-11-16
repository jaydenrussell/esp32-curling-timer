<script>
  let { variant = 'primary', size = 'md', disabled = false, type = 'button', href = '', onclick, loading = false, icon, fullWidth = false, children, ...rest } = $props();
</script>

<button
  class="btn {variant} {size} {fullWidth ? 'w-full' : ''}"
  type={type}
  disabled={disabled || loading}
  onclick={onclick}
  {...rest}
>
  {#if loading}
    <svg class="animate-spin h-5 w-5" viewBox="0 0 24 24"><circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="3" fill="none"/><path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"/></svg>
  {:else if icon}
    <span class="icon">{@render icon()}</span>
  {/if}
  {@render children?.()}
</button>

<style>
  .btn {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    gap: 0.5rem;
    font-family: inherit;
    font-weight: 600;
    border-radius: var(--radius-sm);
    border: 1px solid var(--border);
    background: rgba(255,255,255,0.04);
    color: var(--text);
    min-height: 48px;
    padding: 0 1.25rem;
    cursor: pointer;
    transition: background 150ms ease, border-color 150ms ease, color 150ms ease, box-shadow 150ms ease;
  }
  .btn:focus-visible { outline: none; box-shadow: 0 0 0 2px var(--ring); }
  .btn:disabled { opacity: 0.4; cursor: not-allowed; }
  .btn .icon { display: inline-flex; width: 1em; height: 1em; flex-shrink: 0; }
  .btn svg:not(.icon) { width: 1em; height: 1em; }
  .primary { background: var(--accent); border-color: var(--accent); color: #040e1a; }
  .primary:hover:not(:disabled) { background: #d4a51a; border-color: #d4a51a; }
  .secondary { background: rgba(255,255,255,0.06); border-color: var(--border); }
  .secondary:hover:not(:disabled) { background: rgba(255,255,255,0.12); }
  .ghost { background: transparent; border-color: transparent; color: var(--text-muted); }
  .ghost:hover:not(:disabled) { background: rgba(255,255,255,0.06); color: var(--text); }
  .danger { background: var(--danger); border-color: var(--danger); color: #fff; }
  .danger:hover:not(:disabled) { background: #c0392b; border-color: #c0392b; }
  .success { background: var(--success); border-color: var(--success); color: #040e1a; }
  .success:hover:not(:disabled) { background: #1db54a; border-color: #1db54a; }
  .sm { min-height: 40px; padding: 0 1rem; font-size: var(--step--2); }
  .lg { min-height: 56px; padding: 0 1.5rem; font-size: var(--step0); }
  .w-full { width: 100%; }
</style>