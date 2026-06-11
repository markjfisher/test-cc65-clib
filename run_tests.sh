#!/bin/bash
# One-command test matrix for test-cc65-clib.
#
# Usage:
#   ./run_tests.sh              # full matrix: builds + unit tests + integration (if available)
#   ./run_tests.sh --no-beebium # builds + unit tests only

set -euo pipefail
cd "$(dirname "$0")"

RUN_BEEBIUM=1
for arg in "$@"; do
  case "$arg" in
    --no-beebium) RUN_BEEBIUM=0 ;;
    *) echo "unknown arg: $arg" >&2; exit 2 ;;
  esac
done

echo "############################################################"
echo "# 1/3  Build cc65 bbc library"
echo "############################################################"
make -C build-rom rebuild-cc65-lib

echo
echo "############################################################"
echo "# 2/3  Unit tests (soft65c02)"
echo "############################################################"
./run_unit_tests.sh

if [ "$RUN_BEEBIUM" = "1" ]; then
  echo
  echo "############################################################"
  echo "# 3/3  Beebium integration tests"
  echo "############################################################"
  ./run_integration_tests.sh
else
  echo
  echo "(skipping beebium: --no-beebium)"
fi

echo
echo "==> test matrix complete"