#!/usr/bin/env python3
"""Derive esphome/curling-timer.ha.yaml (paste-ready for HA Device Builder)
from esphome/curling-timer.yaml (repo/CI relative source).

Only two regions differ: the file header note and the external_components
block. CI runs --check so the paste variant cannot drift.
"""
import argparse
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent / "esphome" / "curling-timer.yaml"
HA = Path(__file__).resolve().parent.parent / "esphome" / "curling-timer.ha.yaml"

HEADER_REPO = """\
# Schedule engine + web editor + LT8912B HDMI display + LVGL countdown
# UI are enabled below (external_components).
# =====================================================================
"""

HEADER_HA = """\
# Schedule engine + web editor + LT8912B HDMI display + LVGL countdown
# UI are enabled below (external_components).
#
# THIS IS THE PASTE-READY VARIANT (git source) for the Home Assistant
# ESPHome Device Builder add-on. Repo/CI builds use curling-timer.yaml.
# =====================================================================
"""

BLOCK_REPO = """\
# ---------------------------------------------------------------------
# Schedule engine + web editor (external_components).
# Source below is relative to this file (repo checkout / CI). To build
# from the Home Assistant "ESPHome Device Builder" add-on — or any
# checkout outside this repo — replace `source:` with the GitHub form:
#   source:
#     type: git
#     url: https://github.com/jaydenrussell/esp32-curling-timer
#     ref: main
#     path: external_components   # required: ESPHome only auto-scans
#                                 # components/ and esphome/components/
# ---------------------------------------------------------------------
external_components:
  - source: ../external_components
    components: [curling_engine, web_editor, lt8912b]
"""

BLOCK_HA = """\
# ---------------------------------------------------------------------
# Schedule engine + web editor (external_components).
# PASTE-READY for Home Assistant "ESPHome Device Builder": the source
# is fetched from GitHub (anonymous, repo is public) — no repo checkout
# or path mapping needed. Copy this entire file into
# /config/esphome/curling-timer.yaml and click Install.
# (Repo/CI builds use curling-timer.yaml with the relative source.)
# ---------------------------------------------------------------------
external_components:
  - source:
      type: git
      url: https://github.com/jaydenrussell/esp32-curling-timer
      ref: main
      path: external_components   # required: ESPHome only auto-scans
                                  # components/ and esphome/components/
    components: [curling_engine, web_editor, lt8912b]
    # Re-fetch main on every install so push fixes land immediately
    # (ESPHome default is 1 day).
    #
    # If the build fails with a missing symbol or member, the add-on
    # served a cached (older) clone: update the add-on, or delete its
    # external_components cache, and install again. The "Web Editor"
    # diagnostic entity reports the component build stamp the device
    # actually runs (web_editor <version>).
    refresh: 0s
"""


def build() -> str:
    text = REPO.read_text(encoding="utf-8")
    for old, new, what in (
        (HEADER_REPO, HEADER_HA, "header"),
        (BLOCK_REPO, BLOCK_HA, "external_components block"),
    ):
        if text.count(old) != 1:
            sys.exit(
                f"error: expected exactly one {what} in {REPO.name}, "
                f"found {text.count(old)} — update tools/make_ha_yaml.py "
                "to match the repo yaml"
            )
        text = text.replace(old, new)
    return text


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true", help="verify instead of write")
    args = ap.parse_args()

    expected = build()
    if args.check:
        actual = HA.read_text(encoding="utf-8") if HA.exists() else ""
        if actual != expected:
            sys.exit(
                f"error: {HA.name} is out of sync with {REPO.name} — "
                "run: python tools/make_ha_yaml.py"
            )
        print(f"{HA.name} is in sync")
    else:
        HA.write_text(expected, encoding="utf-8")
        print(f"wrote {HA}")


if __name__ == "__main__":
    main()
