<script>
  import { onMount, onDestroy } from 'svelte';
  import { state as appState, loadSettings, loadConfig, loadStatus, api, startPolling, stopPolling, auth, clearMessage } from './lib/stores.svelte.js';
  import Header from './lib/components/Header.svelte';
  import TabBar from './lib/components/TabBar.svelte';
  import BottomNav from './lib/components/BottomNav.svelte';
  import SaveBar from './lib/components/SaveBar.svelte';
  import LoginModal from './lib/components/LoginModal.svelte';
  import HelpModal from './lib/components/HelpModal.svelte';
  import StatusView from './lib/components/StatusView.svelte';
  import QuickStartView from './lib/components/QuickStartView.svelte';
  import ScheduleView from './lib/components/ScheduleView.svelte';
  import LeaguesView from './lib/components/LeaguesView.svelte';
  import ConfigView from './lib/components/ConfigView.svelte';
  import DiagnosticsView from './lib/components/DiagnosticsView.svelte';
  import HistoryView from './lib/components/HistoryView.svelte';

  let statusInterval = $state(null);

  async function init() {
    clearMessage();
    const ok = await auth.check();
    if (!ok) return; // LoginModal shown by auth.check
    await Promise.all([loadSettings(), loadConfig(), loadStatus()]);
    startPolling();
    statusInterval = setInterval(loadStatus, 5000);
  }

  onMount(() => { init(); });
  onDestroy(() => { stopPolling(); if (statusInterval) clearInterval(statusInterval); });
</script>

<div class="app">
  <Header />
  {#if appState.authenticated}
    <main class="main-content">
      <TabBar />
      <div class="tab-content">
        {#if appState.tab === 'status'}
          <StatusView />
        {:else if appState.tab === 'quickstart'}
          <QuickStartView />
        {:else if appState.tab === 'schedule'}
          <ScheduleView />
        {:else if appState.tab === 'leagues'}
          <LeaguesView />
        {:else if appState.tab === 'config'}
          <ConfigView />
        {:else if appState.tab === 'history'}
          <HistoryView />
        {:else if appState.tab === 'diagnostics'}
          <DiagnosticsView />
        {/if}
      </div>
      <SaveBar />
    </main>
    <BottomNav />
  {/if}
  <LoginModal />
  <HelpModal />
</div>

<style>
  .app {
    min-height: 100vh;
    display: flex;
    flex-direction: column;
  }
  .main-content {
    flex: 1;
    display: flex;
    flex-direction: column;
  }
  .tab-content {
    flex: 1;
    padding: 1rem;
  }
</style>