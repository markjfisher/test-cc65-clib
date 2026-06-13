#!/bin/bash
#
# build.sh - convenience wrapper for building the cc65 CLIB ROM + libraries.
#
# The old interactive per-test build flow (which built tests/test-* programs and
# hand-rolled dfstool JSON) has been retired: those legacy tests are gone, the
# integration disc images are now built by tests/integration/scripts/
# build_test_discs.sh (via scripts/create_ssd.py), and the full validation flow
# lives in ./run_tests.sh.
#
# This script now just drives build-rom/Makefile, whose paths are env-overridable
# (CC65_ROOT, CLIB_ROOT, TARGET_ROM_COPY).
#
# Usage:
#   ./build.sh            # build ROM + cc65 libs if needed (delegates to build-rom)
#   ./build.sh -r         # force a clean rebuild of the CLIB ROM and cc65 libs
#   ./build.sh -h         # help

set -euo pipefail
cd "$(dirname "$0")"

usage() {
  cat <<'EOF'
Usage: build.sh [-r|-h]
  (no args)  Build the CLIB ROM and cc65 bbc/bbc-clib libraries (make -C build-rom all).
  -r         Force a clean rebuild (make -C build-rom clean all).
  -h         Show this help.

To run the test suites use ./run_tests.sh (unit + dual-mode integration).
EOF
}

case "${1:-}" in
  -h|--help) usage; exit 0 ;;
  -r) echo "Forcing clean rebuild of CLIB ROM and cc65 libraries..."
      make -C build-rom clean all ;;
  "") echo "Building CLIB ROM and cc65 libraries..."
      make -C build-rom all ;;
  *)  echo "unknown arg: $1" >&2; usage; exit 2 ;;
esac

echo
echo "ROM files in roms/:"
ls -la roms/ 2>/dev/null || echo "  (none)"
