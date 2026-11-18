<script>
  import { state, closeHelp } from '../stores.svelte.js';
  import Button from './Button.svelte';

  function close() {
    closeHelp();
  }
</script>

<div class="modal" class:show={state.helpOpen} onkeydown={(e) => e.key === 'Escape' && close()}>
  <div class="modal-backdrop" onclick={close}></div>
  <div class="modal-card card">
    <h2>Help</h2>
    <p>ESP32-P4 Curling Timer - countdown, schedule, and relay control for curling games.</p>
    <ul>
      <li><strong>Status:</strong> live game countdown and bell state.</li>
      <li><strong>Quick Start:</strong> start a one-off game immediately.</li>
      <li><strong>Schedule:</strong> weekly game schedule with date exceptions.</li>
      <li><strong>Settings:</strong> relay durations, buzzer hold, web UI options.</li>
      <li><strong>Diagnostics:</strong> device info, network, logs.</li>
    </ul>
    <Button class="secondary" onclick={close}>Close</Button>
  </div>
</div>

<style>
.modal {
  position: fixed;
  inset: 0;
  display: none;
  align-items: center;
  justify-content: center;
  z-index: 300;
  padding: 1rem;
  background: var(--ice-900);
}
.modal.show { display: flex; }
.modal-backdrop { position: absolute; inset: 0; }
.modal-card { position: relative; border-radius: var(--radius-lg); padding: 1.5rem; max-width: 92vw; max-height: 90vh; overflow-y: auto; background: var(--surface); border: 1px solid var(--border); box-shadow: var(--shadow-lg); }
.modal-card h2 { color: var(--text-strong); font-size: var(--step-2); text-transform: none; letter-spacing: -0.01em; }
.modal-card p { color: var(--text-dim); }
.modal-card ul { margin: 0.5rem 0 1rem; padding-left: 1.25rem; color: var(--text-dim); }
.modal-card button { margin-top: 1rem; }
</style>