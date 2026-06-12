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

SHORT_NAMES="break:TBREAK clock:TCLOCK console:TCONSL fileio:TFDISK kbhit:TKBHIT screen:TSCREN osfile:TOSFIL"

for entry in $SHORT_NAMES; do
  test_name="${entry%%:*}"
  short="${entry##*:}"
  c_file="$SRC_DIR/test_${test_name}.c"
  binary="$OUT_DIR/$short"
  ssd="$OUT_DIR/$short.ssd"
  json="$OUT_DIR/$short.json"

  echo "  $test_name -> $short"

  # Link at $1900 (PAGE when DFS is present). The cc65 bbc target defaults to
  # STARTADDRESS $0E00, which collides with Acorn DFS private workspace
  # ($0E00-$18FF). A program loaded+run there from a DFS disc corrupts (and is
  # corrupted by) DFS, so its OSWRCH output never reaches the screen. Linking at
  # $1900 (the DFS PAGE) avoids the conflict.
  compile_args=("-t" "bbc" "--start-addr" "0x1900" "-o" "$binary" "$c_file")
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
      "loadAddress": "&001900",
      "executionAddress": "&001900",
      "contentPath": "$real_binary",
      "type": "other"
    }
  ]
}
JSONEOF

  "$DFSTOOL" make -o "$ssd" -f "$json"
done

# ---------------------------------------------------------------------------
# Directory-listing test (TDIR): needs extra catalogue entries to list.
# ---------------------------------------------------------------------------
echo "  dir -> TDIR"
dir_bin="$OUT_DIR/TDIR"
"$CC65" -t bbc --start-addr 0x1900 -o "$dir_bin" "$SRC_DIR/test_dir.c" 2>/dev/null
real_dir_bin=$(realpath "$dir_bin")
printf 'alpha-data' > "$OUT_DIR/ALPHA.dat"
printf 'beta-data'  > "$OUT_DIR/BETA.dat"
alpha_path=$(realpath "$OUT_DIR/ALPHA.dat")
beta_path=$(realpath "$OUT_DIR/BETA.dat")
cat > "$OUT_DIR/TDIR.json" <<JSONEOF
{
  "version": 1,
  "discTitle": "TDIR",
  "discSize": 800,
  "bootOption": "none",
  "cycleNumber": 0,
  "files": [
    { "fileName": "TDIR", "directory": "\$", "locked": false,
      "loadAddress": "&001900", "executionAddress": "&001900",
      "contentPath": "$real_dir_bin", "type": "other" },
    { "fileName": "ALPHA", "directory": "\$", "locked": false,
      "loadAddress": "&000000", "executionAddress": "&000000",
      "contentPath": "$alpha_path", "type": "other" },
    { "fileName": "BETA", "directory": "\$", "locked": false,
      "loadAddress": "&000000", "executionAddress": "&000000",
      "contentPath": "$beta_path", "type": "other" }
  ]
}
JSONEOF
"$DFSTOOL" make -o "$OUT_DIR/TDIR.ssd" -f "$OUT_DIR/TDIR.json"

echo "  Done. Output in $OUT_DIR"