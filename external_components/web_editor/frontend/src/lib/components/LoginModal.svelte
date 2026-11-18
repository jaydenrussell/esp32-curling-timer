<script>
  import { state, auth, handleLogin, handleLogout, handleHelp, closeHelp, clearMessage } from '../stores.svelte.js';
  import Button from './Button.svelte';

  let password = '';
  let error = '';

  function login(e) {
    e.preventDefault();
    clearMessage();
    handleLogin(password);
  }

  function escape(event) {
    if (event.key === 'Escape') {
      closeHelp();
    }
  }
</script>

<div class="modal" class:show={state.authenticated === false} onkeydown={escape}>
  <div class="modal-card card">
    <h2 class="modal-header">Curling Timer</h2>
    <p>ESP32-P4 curling timer — live countdown, schedule, and relay control for your club.</p>
    <label for="password">Password</label>
    <input id="password" type="password" autocomplete="current-password" required bind:value={password} />
    <div class="banner {state.headerMsg.kind} hidden" id="login-error">{state.headerMsg.text}</div>
    <Button class="primary" type="submit" onclick={login}>Log in</Button>
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
.modal-card {
  position: relative;
  border-radius: var(--radius-lg);
  padding: 1.5rem;
  max-width: 92vw;
  max-height: 90vh;
  overflow-y: auto;
  background: var(--surface);
  border: 1px solid var(--border);
  box-shadow: var(--shadow-lg);
}
.modal-card h2 {
  font-size: var(--step-2);
  text-transform: none;
  letter-spacing: -0.01em;
  color: var(--text-strong);
}
.modal-card p {
  color: var(--text-dim);
}
.modal-card label {
  display: block;
  margin: 0.75rem 0 0;
}
.modal-card input {
  width: 100%;
  margin: 0 0 0.75rem;
}
.modal-card button {
  width: 100%;
}
</style>