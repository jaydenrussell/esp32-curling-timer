<script>
  import { state as appState } from '../stores.svelte.js';
  import { shortTime, secondsNow, durationText, hms, toggleEnabled, cancelQuick } from '../stores.svelte.js';
  import { isAddonEnabled } from '../stores.svelte.js';
  import Card from './Card.svelte';
  import Button from './Button.svelte';

  let mirrorScale = 100;

  function getMirrorScale() {
    const sm = appState.status || {};
    const cv = appState.configValues || {};
    return ((sm.mirror_scale != null ? Number(sm.mirror_scale)
      : (cv['config.mirror_scale'] != null ? Number(cv['config.mirror_scale']) : 100)) / 100);
  }

  function formatTime(seconds) {
    if (seconds <= 0) return '00:00';
    const h = Math.floor(seconds / 3600), m = Math.floor(seconds % 3600 / 60), s = seconds % 60;
    return h ? `${h}:${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}` : `${String(m).padStart(2, '0')}:${String(s).padStart(2, '0')}`;
  }

  $: mirrorScaleVal = getMirrorScale();
</script>

<div id="tab-status">
  {#if appState.status && appState.status.valid}
    <div class="status-grid">
      <section aria-labelledby="status-heading">
        <h2 id="status-heading" class="sr-only">Timer Status</h2>

        {#if !appState.status.enabled}
          <Card>
            <div style="text-align: center; padding: 1rem;">
              <div class="state" style="color: var(--house-red); font-size: 1.5rem;">TIMER DISABLED</div>
              <p class="muted small">The display shows the disabled message and no buzzers will ring.</p>
              <div class="row-actions" style="justify-content: center; margin-top: 1rem;">
                <Button variant="primary" onclick={() => toggleEnabled(true)}>Enable timer</Button>
              </div>
            </div>
          </Card>
        {:else if !appState.status.clock_synced}
          <Card>
            <div class="banner warn">Waiting for the clock to sync (SNTP). The countdown, today's games and quick start appear once the time is known.</div>
          </Card>
        {:else if appState.status.current}
          <Card>
            {#if !isAddonEnabled('relays', appState.configValues, appState.addonDetection)}
              <div class="banner warn" style="margin-bottom: 0.5rem;">Relay add-on is disabled — bells/buzzer will not ring. Enable it in Settings → Add-ons.</div>
            {/if}
            <div style="display: flex; align-items: flex-start; justify-content: space-between; gap: 1rem; margin-bottom: 0.5rem; flex-wrap: wrap;">
              <div>
                <div class="section-title" style="margin-bottom: 0.125rem;">{appState.status.current.quick ? 'Now on ice (Quick Start)' : 'Now on ice'}</div>
                <div style="font-size: var(--step-2); font-weight: 700; line-height: 1.2;">{appState.status.current.name || (appState.status.current.quick ? 'Quick Start' : 'Current game')}</div>
              </div>
              <span class="pill ok">Timer enabled</span>
            </div>
            <div style="display: flex; flex-wrap: wrap; gap: 1rem; align-items: center; color: var(--text-dim); font-size: var(--step--1);">
              <span><strong style="color: var(--text); font-weight: 600;">Start: </strong>{shortTime(appState.status.current.start)}</span>
              <span><strong style="color: var(--text); font-weight: 600;">Game expiration: </strong>{shortTime(appState.status.current.buzzer)}</span>
            </div>
            <div class="row-actions" style="margin-top: 0.5rem;">
              {#if appState.status.current.quick}
                <Button variant="ghost" size="sm" onclick={() => cancelQuick(appState.status.current)}>Cancel quick start</Button>
              {/if}
              <Button variant="danger" size="sm" onclick={() => toggleEnabled(false)}>Disable timer</Button>
            </div>
          </Card>
        {:else}
          <Card>
            {#if !isAddonEnabled('relays', appState.configValues, appState.addonDetection)}
              <div class="banner warn" style="margin-bottom: 0.5rem;">Relay add-on is disabled — bells/buzzer will not ring. Enable it in Settings → Add-ons.</div>
            {/if}
            <div style="display: flex; align-items: flex-start; justify-content: space-between; gap: 1rem; margin-bottom: 0.5rem; flex-wrap: wrap;">
              <div>
                <div class="section-title" style="margin-bottom: 0.125rem;">Status</div>
                <div style="font-size: var(--step-2); font-weight: 700; line-height: 1.2;">Idle</div>
              </div>
              <span class="pill ok">Timer enabled</span>
            </div>
            {#if appState.status.next}
              <div style="display: flex; flex-wrap: wrap; gap: 1rem; align-items: center; color: var(--text-dim); font-size: var(--step--1);">
                <span><strong style="color: var(--text); font-weight: 600;">Next: </strong>{appState.status.next.name || 'Game'}</span>
                <span><strong style="color: var(--text); font-weight: 600;">Start: </strong>{shortTime(appState.status.next.start)}</span>
                <span><strong style="color: var(--text); font-weight: 600;">Game expiration: </strong>{shortTime(appState.status.next.buzzer)}</span>
              </div>
            {:else}
              <p class="muted small">No more games today</p>
            {/if}
            <div class="row-actions" style="margin-top: 0.5rem;">
              <Button variant="danger" size="sm" onclick={() => toggleEnabled(false)}>Disable timer</Button>
            </div>
          </Card>
        {/if}
      </section>

      <section aria-labelledby="mirror-heading">
        <Card>
          <div class="page-head" style="margin-bottom: 0.625rem;">
            <div>
              <div class="section-title">Live display</div>
              <h3>On-display mirror</h3>
            </div>
            <span class="pill">Scale {Math.round(mirrorScaleVal * 100)}%</span>
          </div>
          <div class="display-mirror" id="display-mirror" style="background: #000; color: #fff; border-radius: 12px; position: relative; overflow: hidden; width: 100%; max-width: 100%; aspect-ratio: 16/9; margin: 0 auto;"></div>
        </Card>
      </section>
    </div>

    {#if appState.status.clock_synced}
      <section aria-labelledby="today-heading">
        <Card>
          <div class="page-head">
            <div>
              <div class="section-title">Today</div>
              <h3>{appState.status.weekday}</h3>
            </div>
            {#if appState.status.today.source === 'exception'}
              <span class="pill warn" style="max-width: 14rem; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;" title={appState.status.today.note || ''}>Exception{appState.status.today.note ? ': ' + appState.status.today.note : ''}</span>
            {:else}
              <span class="pill">Weekly schedule</span>
            {/if}
          </div>
          {#if !appState.status.today.entries.length}
            <p class="muted">No buzzers today.</p>
          {:else}
            <ul class="today-list">
              <li class="head"><span>Start</span><span>Bell/Buzzer</span><span class="name">Entry Name</span></li>
              {#each [...appState.status.today.entries].sort((a, b) => a.start - b.start) as entry}
                <li class:active={appState.status.current && appState.status.current.start === entry.start} class:past={entry.buzzer + (entry.header_hold != null ? entry.header_hold : (appState.status.header_hold || 0)) < secondsNow()}>
                  <span>{shortTime(entry.start)}</span>
                  <span>{shortTime(entry.buzzer)}</span>
                  <span class="name">
                    {entry.name || ''}
                    {#if entry.quick}
                      <span class="pill quick">quick</span>
                    {/if}
                  </span>
                </li>
              {/each}
            </ul>
          {/if}
        </Card>
      </section>
    {/if}
  {:else if appState.status && !appState.status.valid}
    <Card>
      <div class="section-title">System status</div>
      <h3>Settings file has an error</h3>
      <div class="banner bad">{appState.status.error}</div>
      <p class="muted">The display shows this error until the file is fixed. Use Settings → Raw settings or Report to recover.</p>
    </Card>
  {/if}
</div>