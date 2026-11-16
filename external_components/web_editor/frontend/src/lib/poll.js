export function startPoller(fn, options = {}) {
  const intervalMs = options.intervalMs ?? 1000;
  const timeoutMs = options.timeoutMs ?? 2500;
  const runImmediately = options.immediate ?? true;
  const onError = options.onError;
  let stopped = false, timerId = null, inFlight = false, tick = 0, currentController = null;
  const clearTimer = () => { if (timerId != null) { clearTimeout(timerId); timerId = null; } };
  const schedule = (delay = intervalMs) => { if (stopped) return; clearTimer(); timerId = setTimeout(run, delay); };
  const run = async () => {
    timerId = null;
    if (stopped) return;
    if (typeof document !== 'undefined' && document.hidden) return;
    if (inFlight) { schedule(intervalMs); return; }
    inFlight = true; tick += 1;
    const controller = new AbortController(); currentController = controller;
    let t2 = null;
    if (typeof timeoutMs === 'number' && timeoutMs > 0) t2 = setTimeout(() => controller.abort(new DOMException('Poll timeout', 'TimeoutError')), timeoutMs);
    try { await fn({ signal: controller.signal, tick }); }
    catch (err) { if (!controller.signal.aborted && typeof onError === 'function') onError(err); }
    finally {
      if (t2) clearTimeout(t2);
      if (currentController === controller) currentController = null;
      inFlight = false;
      if (!stopped && !(typeof document !== 'undefined' && document.hidden)) schedule(intervalMs);
    }
  };
  const onVisibilityChange = () => {
    if (stopped) return;
    if (document.hidden) { clearTimer(); if (currentController) currentController.abort(new DOMException('Page hidden', 'AbortError')); return; }
    if (!inFlight && timerId == null) schedule(0);
  };
  if (typeof document !== 'undefined') document.addEventListener('visibilitychange', onVisibilityChange);
  if (runImmediately) { if (!(typeof document !== 'undefined' && document.hidden)) schedule(0); }
  else { if (!(typeof document !== 'undefined' && document.hidden)) schedule(intervalMs); }
  return function stop() {
    if (stopped) return; stopped = true; clearTimer();
    if (currentController) currentController.abort(new DOMException('Poll stopped', 'AbortError'));
    if (typeof document !== 'undefined') document.removeEventListener('visibilitychange', onVisibilityChange);
  };
}
