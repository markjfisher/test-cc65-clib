#!/bin/bash
# Run the beebium integration tests.
#
# Usage: ./run_integration_tests.sh [pytest_args...]
#
# Requires: built beebium-server, MOS/BASIC ROMs, DFS ROM.
# Uses the .venv in the project root (created by ./run_tests.sh or manually).
# Skips gracefully if dependencies are not met.

set -euo pipefail
cd "$(dirname "$0")"

# Path to the project venv
VENV_DIR=".venv"

# Find the beebium-server
BEEBIUM_SERVER="/home/markf/dev/bbc/beebium/build-release/src/server/beebium-model-b"
if [ ! -x "$BEEBIUM_SERVER" ]; then
  echo "    beebium-server not found at $BEEBIUM_SERVER -- skipping integration tests."
  exit 0
fi

# Activate the venv
if [ -f "$VENV_DIR/bin/activate" ]; then
  . "$VENV_DIR/bin/activate"
elif command -v uv >/dev/null 2>&1; then
  # uv virtualenv support
  :
else
  echo "    no virtualenv found -- skipping integration tests."
  exit 0
fi

export BEEBIUM_SERVER
exec python3 -m pytest tests/integration/scripted/ -q "$@"