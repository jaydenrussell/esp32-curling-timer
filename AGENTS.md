# AGENTS.md

## How to run

- **Build**: `pip install esphome` → `cp esphome/secrets.yaml.example esphome/secrets.yaml` → fill secrets → `esphome compile esphome/curling-timer.yaml`
- **Lint**: `esphome lint esphome/curling-timer.yaml`
- **Host tests** (no hardware): `tests/host/run.sh` (Linux/g++) or `tests/host/run.ps1` (Windows/MSVC). Runs doctest cases over settings, model, quickstart, sha256, store, fs_std, config_edit, render, bell, and web auth.
- **No typecheck step** — ESPHome uses C++/Python; verification is via build + host tests only.
- **Typecheck**: ESPHome uses C++/Python; no formal typecheck beyond ESP-IDF toolchain

## Entrypoints

- Main build config: `esphome/curling-timer.yaml` — the repository/CI build target
- Wi-Fi C6 failover package: `esphome/wifi-c6-failover.yaml` — include only after C6 is wired and flashed (`packages: { wifi_addon: !include wifi-c6-failover.yaml }`)
- Secrets: `esphome/secrets.yaml` (gitignored; copy from `secrets.yaml.example`)
- External components live in `external_components/` — `lt8912b` (HDMI), `curling_engine` (schedule + bells), `web_editor` (on-device UI), `calendar_import`

## Command order

- `lint` before `compile` to catch YAML errors early
- Flash C6 first (`firmware/network_adapter_esp32c6.bin` over SuperMini USB-C), then enable Wi-Fi in `curling-timer.yaml`
- When bumping component code: update `esphome/component_ref.txt`, regenerate the paste variant, commit everything together, then tag. CI verifies the paste variant matches the generator.

## Architecture

- Single ESPHome package: `esphome/curling-timer.yaml` is the build entrypoint
- Hardware: M5Stack Unit PoE-P4 (U213) ESP32-P4NRW32 (16MB flash, 32MB PSRAM) + IP101 Ethernet PHY (PoE) + optional ESP32-C6 SuperMini on SDIO header for Wi-Fi 6 failover
- Ethernet-only is the default build; Wi-Fi requires flashing C6 + enabling `wifi-c6-failover.yaml`
- GPIO pin map: `docs/wiring-U213.md` — verify all pins against this before adding hardware
- C6 first flash: `docs/flash-c6.md`

## Testing

- Host tests: `tests/host/run.sh` or `tests/host/run.ps1` — 87 doctest cases, no hardware needed
- Hardware-in-the-loop requires physical U213 + relays + LT8912B display
- No separate test suite beyond ESPHome's built-in test framework + host doctests

## Common pitfalls

- **Secrets**: Never commit `esphome/secrets.yaml` — use `secrets.yaml.example` and fill securely. Missing secrets fail the build.
- **GPIO pin mapping**: Always verify against `docs/wiring-U213.md` before adding hardware. RMII TXD0/TX1 are boot-strapping pins (G34/G35); the SDIO-Bus header pin 9 is 5V — never feed the C6 from there.
- **C6 version match**: `network_adapter_esp32c6.bin` must match the ESPHome `esp_hosted` ref — mismatch traps the P4 in a boot loop.
- **Task priorities**: ESP-IDF tasks need explicit priorities; avoid blocking `delay()` in lambdas.
- **engineering_sample**: U213 is pre-rev3 silicon — `engineering_sample: true` is required in the YAML.
- **Web editor password**: must be ≥ 6 characters or the editor fails to start.
- **Web UI**: repo builds use `curling-timer.yaml` for both standalone and HA Device Builder.


## Subagent Delegation Guidelines
- `@planner`: Default agent for breaking down features and system architecture.
- `@reviewer`: Run before committing any custom C++ components or complex YAML changes.
- `@release`: Call once changes are verified to build, commit, tag, and push.

<!-- gitnexus:start -->
# GitNexus — Code Intelligence

This project is indexed by GitNexus as **esp32-curling-timer** (1916 symbols, 4755 relationships, 122 execution flows).

> Index stale? Run `node .gitnexus/run.cjs analyze --index-only` from the project root — it auto-selects an available runner. No `.gitnexus/run.cjs` yet? Bootstrap with `npx`, `bunx`, or `pnpm dlx` — e.g. `bunx gitnexus@latest analyze` (npm 11 npx crash; #1939).

## Always Do

- **MUST run impact before editing.** Use `impact({target: "symbolName", direction: "upstream"})` or `node .gitnexus/run.cjs impact "symbolName" --direction upstream --repo .`; report callers, processes, and risk. Never substitute grep for graph analysis.
- **MUST analyze graph changes before committing.** Use `detect_changes({scope: "all"})` (MCP) or `node .gitnexus/run.cjs detect-changes --scope all --repo .` (CLI fallback). `partial: true` or `truncated: true` is not a clean check — a zero means unseen, not unaffected; re-run it. For regression review: `detect_changes({scope: "compare", base_ref: "main"})` or `node .gitnexus/run.cjs detect-changes --scope compare --base-ref "main" --repo .`.
- MUST warn on HIGH/CRITICAL `risk` pre-edit; never use `riskSharedAxes` to waive a HIGH/CRITICAL `risk` warning. Compare File/symbol: MCP File omits axes; Graph-RAG expands File.
- **MUST treat `risk: UNKNOWN` as unresolved, not as low.** An empty caller set is not evidence the symbol is unused — it can also mean the callers are not resolvable by the index (plain-object property access, dynamic dispatch, cross-language calls). `impact` pairs `UNKNOWN` with a `riskNote` saying so. Confirm with a text search before treating the symbol as safe to change or delete; do not proceed on the strength of a zero.
- **MUST use `query({search_query: "concept"})` for concepts/flows, `context({name: "symbolName"})` for a named symbol, or `impact` for blast radius, on read-only callers, dependencies, imports, or execution flow.** Graph first; text search only for empty/`UNKNOWN`/literals.
- For security review, `explain({target: "fileOrSymbol"})` lists taint findings (source→sink flows; needs `analyze --pdg`).

## Never Do

- NEVER edit a function, class, or method before MCP/CLI impact analysis.
- NEVER ignore HIGH or CRITICAL risk warnings from impact analysis, and never read `UNKNOWN` as an all-clear — it means the walk could not answer, which is the one verdict that requires confirming by other means.
- NEVER rename symbols with find-and-replace — use `rename` which understands the call graph.
- NEVER commit before MCP/CLI graph change analysis.

## Resources

| Resource | Use for |
| --- | --- |
| `gitnexus://repo/esp32-curling-timer/context` | Codebase overview, check index freshness |
| `gitnexus://repo/esp32-curling-timer/clusters` | All functional areas |
| `gitnexus://repo/esp32-curling-timer/processes` | All execution flows |
| `gitnexus://repo/esp32-curling-timer/process/{name}` | Step-by-step execution trace |

## CLI

| Task | Read this skill file |
| --- | --- |
| Understand architecture / "How does X work?" | `.claude/skills/gitnexus-exploring/SKILL.md` |
| Blast radius / "What breaks if I change X?" | `.claude/skills/gitnexus-impact-analysis/SKILL.md` |
| Trace bugs / "Why is X failing?" | `.claude/skills/gitnexus-debugging/SKILL.md` |
| Rename / extract / split / refactor | `.claude/skills/gitnexus-refactoring/SKILL.md` |
| Tools, resources, schema reference | `.claude/skills/gitnexus-guide/SKILL.md` |
| Index, status, clean, wiki CLI commands | `.claude/skills/gitnexus-cli/SKILL.md` |

<!-- gitnexus:end -->
