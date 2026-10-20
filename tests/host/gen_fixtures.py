"""Generate C++ test fixtures from the pitimer Python ground truth.

Regenerates tests/host/generated_fixtures.h by running the same dict
transformations test_settings.py performs (deepcopy + safe_dump), so the
C++ suite validates against byte-identical PyYAML output.

Usage:
  python tests/host/gen_fixtures.py <path-to-rpi-buzzer-dev>
"""
import copy
import sys
import textwrap
from pathlib import Path

import yaml


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit(__doc__)
    project = Path(sys.argv[1]).resolve()
    sys.path.insert(0, str(project))
    from tests.fixtures import BASE_SETTINGS, HEADER, CONFIG, SECTIONS, SETTINGS_TEXT  # noqa: E402

    example = (project / "pitimer_settings.example.yml").read_bytes().decode("utf-8")

    def dump(raw) -> str:
        return yaml.safe_dump(raw, sort_keys=False)

    def modified(section_path, key, value=None, delete=False):
        raw = copy.deepcopy(BASE_SETTINGS)
        section = raw
        for part in section_path:
            section = section[part]
        if delete:
            del section[key]
        else:
            section[key] = value
        return raw

    cases: dict[str, str] = {}

    # --- test_settings.TestParseSettingsFile ---------------------------------
    cases["BASE"] = dump(BASE_SETTINGS)
    for label, path, key in [
        ("MISSING_CONFIG", [], "config"),
        ("MISSING_SCHEDULE", [], "schedule"),
        ("MISSING_EXCEPTIONS", [], "exceptions"),
        ("MISSING_ENABLED", ["config"], "enabled"),
        ("MISSING_TEXT_WHEN_IDLE", ["config", "header", "text"], "when_idle"),
        ("MISSING_COUNTDOWN_COLOR", ["config", "countdown"], "color"),
        ("MISSING_PACE_ENABLED", ["config", "pace_of_play"], "enabled"),
        ("MISSING_PACE_UNIT_DURATION", ["config", "pace_of_play"], "unit_duration"),
        ("MISSING_FOOTER_ENTRY_NAME", ["config", "footer"], "entry_name"),
        ("MISSING_BUZZER_HEADER_HOLD", ["config", "buzzer"], "header_hold"),
        ("MISSING_SUNDAY", ["schedule"], "Sunday"),
    ]:
        cases[label] = dump(modified(path, key, delete=True))
    for label, path, key, value in [
        ("INVALID_ENABLED_YES", ["config"], "enabled", "yes"),
        ("INVALID_FONT_SIZE_BOOL", ["config", "footer"], "font_size", True),
        ("INVALID_FONT_SIZE_STR", ["config", "header"], "font_size", "208"),
        ("INVALID_PACE_UNIT_ZERO", ["config", "pace_of_play"], "unit_duration", 0),
        ("INVALID_RELAY5", ["config", "buzzer"], "relays", ["RELAY5"]),
        ("INVALID_RELAYS_STR", ["config", "buzzer"], "relays", "RELAY1"),
        ("INVALID_SUNDAY_NULL", ["schedule"], "Sunday", None),
    ]:
        cases[label] = dump(modified(path, key, value))

    # --- test_settings.TestParseSchedules ------------------------------------
    def sunday(entries):
        return dump(modified(["schedule"], "Sunday", entries))

    cases["SUN_OPTIONAL"] = sunday([
        {"start": 36000, "buzzer": 39600, "name": "X", "during_buzzer": "DONE",
         "header_hold": 30, "pace_of_play": False, "pace_unit_duration": 720}])
    cases["SUN_DEFAULT_HOLD"] = sunday([{"start": 36000, "buzzer": 39600}])
    cases["SUN_HOLD_ZERO"] = sunday([{"start": 36000, "buzzer": 39600, "header_hold": 0}])
    for label, entry in [
        ("E_MISSING_BUZZER", {"start": 36000}),
        ("E_BUZZER_BEFORE", {"start": 39600, "buzzer": 36000}),
        ("E_OVER_3H", {"start": 36000, "buzzer": 36000 + 3 * 3600}),
        ("E_STRING_TIME", {"start": "09:00:00", "buzzer": 39600}),
        ("E_PACE_ZERO", {"start": 36000, "buzzer": 39600, "pace_unit_duration": 0}),
        ("E_HOLD_NEG", {"start": 36000, "buzzer": 39600, "header_hold": -1}),
        ("E_HOLD_FRAC", {"start": 36000, "buzzer": 39600, "header_hold": 1.5}),
    ]:
        cases[label] = sunday([entry])
    first = {"start": 36000, "buzzer": 39600}
    for label, second in [
        ("OV_STARTS_DURING", {"start": 38000, "buzzer": 42000}),
        ("OV_STARTS_HOLD", {"start": 39600 + 300, "buzzer": 42000}),
        ("OV_ENDS_DURING", {"start": 30000, "buzzer": 37000}),
        ("OV_ENCLOSES", {"start": 30000, "buzzer": 42000}),
        ("OV_INSIDE", {"start": 37000, "buzzer": 38000}),
    ]:
        cases[label] = sunday([first, second])
    cases["OK_BACK_TO_BACK"] = sunday([first, {"start": 39600 + 300 + 1, "buzzer": 43200}])
    cases["OK_SHORTER_HOLD"] = sunday(
        [{"start": 36000, "buzzer": 39600, "header_hold": 10}, {"start": 39611, "buzzer": 43200}])
    cases["ERR_SHORTER_HOLD"] = sunday(
        [{"start": 36000, "buzzer": 39600, "header_hold": 10}, {"start": 39610, "buzzer": 43200}])
    cases["ERR_LONGER_HOLD"] = sunday(
        [{"start": 36000, "buzzer": 39600, "header_hold": 360}, {"start": 39600 + 300 + 1, "buzzer": 43200}])
    cases["OK_LONGER_HOLD"] = sunday(
        [{"start": 36000, "buzzer": 39600, "header_hold": 360}, {"start": 39600 + 300 + 61, "buzzer": 43200}])

    # --- test_settings.TestFindCurrentGame -----------------------------------
    cases["SHORT_HOLD_MONDAY"] = dump(modified(
        ["schedule"], "Monday",
        [{"start": 36000, "buzzer": 39600, "name": "SHORT HOLD", "header_hold": 5}]))

    # --- raw texts ------------------------------------------------------------
    texts = {
        "HEADER": HEADER,
        "CONFIG": CONFIG,
        "SETTINGS_TEXT": SETTINGS_TEXT,
        "EXAMPLE_TEXT": example.replace("\r\n", "\n"),
    }

    out = [
        "// AUTO-GENERATED by tests/host/gen_fixtures.py — do not edit by hand.",
        "// Ground truth: pitimer tests/fixtures.py + PyYAML safe_dump output.",
        "#pragma once",
        "",
    ]
    for name, body in sorted(cases.items()):
        out.append(f'static const char CASE_{name}[] = R"FIX({body.replace(chr(13), "")})FIX";')
        out.append("")
    for name, body in texts.items():
        out.append(f'static const char TEXT_{name}[] = R"FIX({body.replace(chr(13), "")})FIX";')
        out.append("")

    dest = Path(__file__).resolve().parent / "generated_fixtures.h"
    dest.write_text("\n".join(out) + "\n", encoding="utf-8", newline="\n")
    print(f"wrote {dest} ({dest.stat().st_size} bytes, {len(cases)} cases)")


if __name__ == "__main__":
    main()
