#!/bin/bash
# Run the beebium integration tests.
#
# Usage: ./run_integration_tests.sh [pytest_args...]
#
# Requires: beebium-server on PATH, MOS ROMs in expected locations.
# Skips gracefully if dependencies are not met.

set -euo pipefail
cd "$(dirname "$0")/tests/integration"

if ! command -v beebium >/dev/null 2>&1 && ! uv run which beebium >/dev/null 2>&1; then
  echo "    beebium not found -- skipping integration tests."
  echo "    (install from https://github.com/...)  "
  exit 0
fi

exec uv run pytest scripted/ -q "$@"