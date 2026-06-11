#!/bin/bash
# Build DFS disc images for all integration test programs.
# Output goes to build/integration-testing/discs/
# Run from project root: bash tests/integration/scripts/build_test_discs.sh

set -euo pipefail

SRC_DIR="tests/integration/discs"
OUT_DIR="build/integration-testing/discs"
CC65="../cc65/bin/cl65"
DFSTOOL="$HOME/.cargo/bin/dfstool"

mkdir -p "$OUT_DIR"

# Map test name -> 7-char DFS filenames
declare -A SHORT
SHORT[break]=TBREAK
SHORT[clock]=TCLOCK
SHORT[console]=TCONSL
SHORT[fileio]=TFDISK
SHORT[kbhit]=TKBHIT
SHORT[screen]=TSCREN

for c_file in "$SRC_DIR"/test_*.c; do
    base=$(basename "$c_file" .c)
    test_name="${base#test_}"
    short="${SHORT[$test_name]}"
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
    real_json=$(realpath "$json")
    abs_out_dir=$(realpath "$OUT_DIR")

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