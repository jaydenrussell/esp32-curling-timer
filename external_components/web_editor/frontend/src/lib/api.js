let onUnauthorized = null;
export function setUnauthorizedHandler(fn) { onUnauthorized = fn; }

export async function api(path, body, opts = {}) {
  const options = body === undefined ? {} : {
    method: 'POST',
    headers: { 'Content-Type': 'application/json', ...(opts.headers || {}) },
    body: JSON.stringify(body),
  };
  const fetchOpts = { credentials: 'same-origin', ...options, ...(opts.fetchOptions || {}) };
  const controller = new AbortController();
  const signals = [];
  if (fetchOpts.signal) signals.push(fetchOpts.signal);
  if (opts.signal) signals.push(opts.signal);
  const onAbort = () => controller.abort();
  for (const s of signals) {
    if (!s) continue;
    if (s.aborted) controller.abort(s.reason);
    else s.addEventListener('abort', onAbort, { once: true });
  }
  let timeoutId;
  const timeoutMs = opts.timeoutMs;
  if (typeof timeoutMs === 'number' && timeoutMs > 0) {
    timeoutId = setTimeout(() => controller.abort(new DOMException('Timeout', 'TimeoutError')), timeoutMs);
  }
  fetchOpts.signal = controller.signal;
  let response;
  try {
    response = await fetch(path, fetchOpts);
  } catch (err) {
    if (timeoutId) clearTimeout(timeoutId);
    for (const s of signals) s?.removeEventListener?.('abort', onAbort);
    return { ok: false, status: 0, data: { error: 'cannot reach the timer, check the network' }, aborted: controller.signal.aborted };
  }
  if (timeoutId) clearTimeout(timeoutId);
  for (const s of signals) s?.removeEventListener?.('abort', onAbort);
  const data = await response.json().catch(() => ({ error: 'unexpected response (' + response.status + ')' }));
  if (response.status === 401 && path !== '/api/login') {
    try { onUnauthorized && onUnauthorized(); } catch {}
  }
  return { ok: response.ok, status: response.status, data };
}
