#!/usr/bin/env bash
# Host test runner (CI/Linux): g++, tests against fs_std.
# Usage: tests/host/run.sh [doctest args...]
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p build
ENGINE=../../external_components/curling_engine
WEB=../../external_components/web_editor
g++ -std=c++20 -O1 -Wall -Wextra -Wno-unused-parameter -I. \
  "$ENGINE/settings.cpp" "$ENGINE/model.cpp" "$ENGINE/quickstart.cpp" \
  "$ENGINE/sha256.cpp" "$ENGINE/store.cpp" "$ENGINE/fs_std.cpp" \
  "$ENGINE/config_edit.cpp" "$ENGINE/render.cpp" \
  "$WEB/auth.cpp" \
  test_*.cpp -o build/host_tests
exec ./build/host_tests "$@"
