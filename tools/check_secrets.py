"""Verify the secrets contract is consistent across build environments.

ESPHome YAML only names secret keys via `!secret <key>`. The actual values come
from whichever environment builds the firmware:

  * Home Assistant ESPHome Device Builder  -> local esphome/secrets.yaml
  * GitHub Actions release                 -> repository secrets named <KEY>.upper()

This script fails if any `!secret` key used in the YAMLs is missing from
secrets.yaml.example (so the HA builder can't fill it) or if the example file
has keys no YAML uses. It prints the GitHub Actions secret names that must be
set so the two environments can never drift apart.
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
YAML_GLOB = ["esphome/curling-timer.yaml", "esphome/wifi-c6-failover.yaml",
             "esphome/test/hdmi-test.yaml"]
EXAMPLE = ROOT / "esphome/secrets.yaml.example"

SECRET_RE = re.compile(r"!secret\s+([A-Za-z0-9_]+)")


def yaml_secret_keys() -> set[str]:
    keys = set()
    for rel in YAML_GLOB:
        p = ROOT / rel
        if p.exists():
            keys.update(SECRET_RE.findall(p.read_text(encoding="utf-8")))
    return keys


def example_keys() -> set[str]:
    keys = set()
    for line in EXAMPLE.read_text(encoding="utf-8").splitlines():
        m = re.match(r"^([A-Za-z0-9_]+):\s", line)
        if m:
            keys.add(m.group(1))
    return keys


def main() -> int:
    used = yaml_secret_keys()
    defined = example_keys()
    missing_in_example = sorted(used - defined)
    unused_in_example = sorted(defined - used)
    if missing_in_example:
        print("ERROR: !secret keys used in YAML but missing from secrets.yaml.example:")
        for k in missing_in_example:
            print(f"  - {k}")
    if unused_in_example:
        print("WARNING: keys in secrets.yaml.example not used by any YAML:")
        for k in unused_in_example:
            print(f"  - {k}")
    print("\nGitHub Actions repository secrets to set (one per YAML key):")
    for k in sorted(used):
        print(f"  - {k.upper()}   (fills {k})")
    if missing_in_example:
        return 1
    print("\nsecrets contract OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
