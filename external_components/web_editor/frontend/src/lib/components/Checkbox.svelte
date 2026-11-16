<script>
  export let label = '';
  export let checked = false;
  export let disabled = false;
  export let required = false;
  export let id = `checkbox-${Math.random().toString(36).slice(2)}`;
  export let name = '';
  export let value = '';
  export let onchange;
</script>

<label class="check" for={id}>
  <input
    type="checkbox"
    id={id}
    name={name}
    value={value}
    checked={checked}
    disabled={disabled}
    required={required}
    onchange={onchange}
    {...$$restProps}
  />
  <span>{label}</span>
</label>

<style>
  .check {
    display: inline-flex;
    align-items: center;
    gap: 0.625rem;
    font-weight: 500;
    cursor: pointer;
    min-height: 44px;
    color: var(--text);
    font-size: var(--step-0);
    user-select: none;
  }
  .check input[type="checkbox"] {
    width: 22px;
    height: 22px;
    min-height: 0;
    margin: 0;
    accent-color: var(--house-red);
    flex-shrink: 0;
    cursor: pointer;
  }
  .check:has(input:disabled) { opacity: 0.5; cursor: not-allowed; }
  .check:has(input:focus-visible) { outline: 2px solid var(--focus); outline-offset: 2px; border-radius: 4px; }
</style>