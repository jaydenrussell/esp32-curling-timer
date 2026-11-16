<script>
  export let label = '';
  export let value = '';
  export let options = [];
  export let placeholder = '';
  export let disabled = false;
  export let required = false;
  export let error = '';
  export let hint = '';
  export let id = `select-${Math.random().toString(36).slice(2)}`;
  export let name = '';
  export let onchange;
</script>

<div class="field">
  {#if label}
    <label for={id}>{label}</label>
  {/if}
  <select
    id={id}
    name={name}
    value={value}
    disabled={disabled}
    required={required}
    aria-invalid={!!error}
    aria-describedby={error ? `${id}-error` : hint ? `${id}-hint` : undefined}
    onchange={onchange}
    {...$$restProps}
  >
    {#if placeholder}
      <option value="" disabled selected>{placeholder}</option>
    {/if}
    {#each options as opt}
      <option value={opt.value}>{opt.label}</option>
    {/each}
  </select>
  {#if hint && !error}
    <span id={id}-hint class="muted small" style="margin-top: 0.25rem; display: block;">{hint}</span>
  {/if}
  {#if error}
    <span id={id}-error class="banner bad small" style="margin-top: 0.25rem; display: block;">{error}</span>
  {/if}
</div>

<style>
  .field { margin: 0 0 0.75rem; }
  .field label { display: block; font-size: var(--step--2); color: var(--text-dim); margin: 0 0 0.375rem; font-weight: 600; letter-spacing: 0.08em; text-transform: uppercase; }
  select {
    font: inherit; color: var(--text); background: var(--ice-900);
    border: 1px solid var(--border); border-radius: var(--radius-sm); padding: 0.625rem 0.875rem;
    min-height: 48px; min-width: 0; font-size: var(--step-0);
    transition: border-color var(--transition-fast), background var(--transition-fast), box-shadow var(--transition-fast);
    width: 100%;
    appearance: none;
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 24 24' fill='none' stroke='%238b9cb3' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'%3E%3Cpolyline points='6 9 12 15 18 9'%3E%3C/polyline%3E%3C/svg%3E");
    background-repeat: no-repeat;
    background-position: right 0.75rem center;
    padding-right: 2.5rem;
  }
  @media (pointer: coarse) { select { font-size: 1rem; } }
  select:focus { outline: none; border-color: var(--focus); box-shadow: 0 0 0 2px rgba(214, 40, 40, 0.2); background: var(--ice-700); }
  select[aria-invalid="true"] { border-color: var(--house-red); }
  select:disabled { opacity: 0.5; cursor: not-allowed; }
</style>