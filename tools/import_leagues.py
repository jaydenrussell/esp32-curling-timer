#!/usr/bin/env python3
"""Server-side league importer for the curling timer.

Fetches a Curling Club Manager league page URL (server-side, so no CORS),
parses out league names using the same selector logic as the web UI
(h1-h4, strong, b), and prints them one per line or as JSON.

Usage:
    python tools/import_leagues.py [URL]
    python tools/import_leagues.py --json [URL]

Default URL: https://simcoecurlingclub.ca/curling/leagues
"""
import sys
import json
import re
from html.parser import HTMLParser

DEFAULT_URL = "https://simcoecurlingclub.ca/curling/leagues"

try:
    import requests  # type: ignore
    HAVE_REQUESTS = True
except Exception:  # pragma: no cover - fallback to stdlib
    HAVE_REQUESTS = False
    import urllib.request


class LeagueExtractor(HTMLParser):
    """Mirror of the web UI DOMParser logic: h1-h4, strong, b text."""

    TAGS = {"h1", "h2", "h3", "h4", "strong", "b"}
    SKIP_RE = re.compile(r"^(leagues|home|menu|login|copyright|simcoe)$", re.I)
    WS_RE = re.compile(r"\s+")

    def __init__(self):
        super().__init__()
        self._stack = []  # list of (tag, active)
        self.names = []
        self.seen = set()
        self._buf = []

    def handle_starttag(self, tag, attrs):
        if tag in self.TAGS:
            self._stack.append(tag)
            self._buf.append([])

    def handle_endtag(self, tag):
        if tag in self.TAGS and self._stack and self._stack[-1] == tag:
            self._stack.pop()
            text = self.WS_RE.sub(" ", "".join(self._buf.pop())).strip()
            self._emit(text)

    def handle_data(self, data):
        if self._stack:
            self._buf[-1].append(data)

    def _emit(self, text):
        if not text or len(text) < 3 or len(text) > 80:
            return
        if self.SKIP_RE.match(text):
            return
        if text in self.seen:
            return
        self.seen.add(text)
        self.names.append(text)


def fetch_html(url):
    if HAVE_REQUESTS:
        r = requests.get(url, headers={"User-Agent": "Mozilla/5.0"}, timeout=20)
        r.raise_for_status()
        return r.text
    req = urllib.request.Request(url, headers={"User-Agent": "Mozilla/5.0"})
    with urllib.request.urlopen(req, timeout=20) as resp:
        return resp.read().decode(resp.headers.get_content_charset() or "utf-8", errors="replace")


def extract_leagues(html):
    p = LeagueExtractor()
    p.feed(html)
    return p.names


def main(argv):
    url = DEFAULT_URL
    as_json = False
    args = [a for a in argv[1:]]
    if "--json" in args:
        as_json = True
        args.remove("--json")
    if args:
        url = args[0]

    try:
        html = fetch_html(url)
    except Exception as exc:  # noqa: BLE001 - report and exit cleanly
        print(f"ERROR fetching {url}: {exc}", file=sys.stderr)
        return 1

    names = extract_leagues(html)
    if as_json:
        print(json.dumps(names, ensure_ascii=False, indent=2))
    else:
        for n in names:
            print(n)
    print(f"# {len(names)} league(s) found", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
