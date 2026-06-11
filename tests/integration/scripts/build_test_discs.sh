#!/bin/bash
# Build DFS disc images for all integration test programs.
# Output goes to build/integration-testing/discs/
#
# Environment:
#   CC65_DIR       path to cc65 checkout (default: ../cc65)
#   BEEBIUM_ROM_DIR  path to ROM files (default: ../beebium/build-release/roms)
#
# Run from project root: bash tests/integration/scripts/build_test_discs.sh

set -euo pipefail

SRC_DIR="tests/integration/discs"
OUT_DIR="build/integration-testing/discs"
CC65="${CC65_DIR:-$(cd .. && pwd)/cc65}/bin/cl65"
DFSTOOL="$(command -v dfstool)"

mkdir -p "$OUT_DIR"

SHORT_NAMES="break:TBREAK clock:TCLOCK console:TCONSL fileio:TFDISK kbhit:TKBHIT screen:TSCREN"

for entry in $SHORT_NAMES; do
  test_name="${entry%%:*}"
  short="${entry##*:}"
  c_file="$SRC_DIR/test_${test_name}.c"
  binary="$OUT_DIR/$short"
  ssd="$OUT_DIR/$short.ssd"
  json="$OUT_DIR/$short.json"

  echo "  $test_name -> $short"

  compile_args=("-t" "bbc" "-o" "$binary" "$c_file")
  if [ "$test_name" = "break" ]; then
    compile_args+=("$SRC_DIR/brk_helper.s")
  fi

  "$CC65" "${compile_args[@]}" 2>/dev/null

  real_binary=$(realpath "$binary")

  cat > "$json" <<JSONEOF
{
  "version": 1,
  "discTitle": "$short",
  "discSize": 800,
  "bootOption": "none",
  "cycleNumber": 0,
  "files": [
    {
      "fileName": "$short",
      "directory": "\$",
      "locked": false,
      "loadAddress": "&000E00",
      "executionAddress": "&000E00",
      "contentPath": "$real_binary",
      "type": "other"
    }
  ]
}
JSONEOF

  "$DFSTOOL" make -o "$ssd" -f "$json"
done

echo "  Done. Output in $OUT_DIR"