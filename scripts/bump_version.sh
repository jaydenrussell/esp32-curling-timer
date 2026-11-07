#!/usr/bin/env bash
set -euo pipefail

VERSION=$(git describe --tags --always 2>/dev/null || echo "dev")
sed -i "s/^  firmware_version:.*/  firmware_version: \"$VERSION\"/" esphome/curling-timer.ha.yaml
echo "$VERSION" > esphome/component_ref.txt 2>/dev/null || true
echo "Bumped to $VERSION"
