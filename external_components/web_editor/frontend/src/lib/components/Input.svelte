<script>
  export let label = '';
  export let type = 'text';
  export let value = '';
  export let placeholder = '';
  export let disabled = false;
  export let required = false;
  export let readonly = false;
  export let error = '';
  export let hint = '';
  export let inputmode = '';
  export let min;
  export let max;
  export let step;
  export let maxlength;
  export let ariaLabel = '';
  export let oninput;
  export let onchange;
  export let onblur;
  export let id = `input-${Math.random().toString(36).slice(2)}`;
  export let name = '';
  export let tabular = false;
  export let textAlign = '';
  export let width;
</script>

<div class="field">
  {#if label}
    <label for={id}>{label}</label>
  {/if}
  <input
    id={id}
    name={name}
    type={type}
    value={value}
    placeholder={placeholder}
    disabled={disabled}
    required={required}
    readonly={readonly}
    aria-invalid={!!error}
    aria-describedby={error ? `${id}-error` : hint ? `${id}-hint` : undefined}
    aria-label={ariaLabel}
    inputmode={inputmode}
    min={min}
    max={max}
    step={step}
    maxlength={maxlength}
    class:tabular={tabular}
    style="text-align: {textAlign}; width: {width};"
    oninput={oninput}
    onchange={onchange}
    onblur={onblur}
    {...$$restProps}
  />
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
  input {
    font: inherit; color: var(--text); background: var(--ice-900);
    border: 1px solid var(--border); border-radius: var(--radius-sm); padding: 0.625rem 0.875rem;
    min-height: 48px; min-width: 0; font-size: var(--step-0);
    transition: border-color var(--transition-fast), background var(--transition-fast), box-shadow var(--transition-fast);
    width: 100%;
  }
  @media (pointer: coarse) { input { font-size: 1rem; } }
  input[type="number"]::-webkit-outer-spin-button, input[type="number"]::-webkit-inner-spin-button { -webkit-appearance: none; margin: 0; }
  input[type="number"] { -moz-appearance: textfield; }
  input.tabular { font-variant-numeric: tabular-nums; font-family: 'JetBrains Mono', monospace; }
  input:focus { outline: none; border-color: var(--focus); box-shadow: 0 0 0 2px rgba(214, 40, 40, 0.2); background: var(--ice-700); }
  input[aria-invalid="true"] { border-color: var(--house-red); }
  input[aria-invalid="true"]:focus { box-shadow: 0 0 0 2px rgba(214, 40, 40, 0.3); }
  input:disabled { opacity: 0.5; cursor: not-allowed; }
</style>