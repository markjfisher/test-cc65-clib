#!/bin/bash
# Build DFS disc images for all integration test programs.
# Output goes to build/integration-testing/discs/
#
# Each program is compiled with cc65, staged into a per-disc directory with a
# DFS .inf sidecar (giving its DFS name + load/exec addresses), then turned into
# an SSD by scripts/create_ssd.py (the same SSD builder used elsewhere, which
# wraps dfstool). This avoids hand-crafting dfstool JSON manifests here.
#
# Environment:
#   CC65_DIR         path to cc65 checkout (default: ../cc65)
#
# Run from project root: bash tests/integration/scripts/build_test_discs.sh

set -euo pipefail

SRC_DIR="tests/integration/discs"
OUT_DIR="build/integration-testing/discs"
STAGE_ROOT="$OUT_DIR/stage"
CC65="${CC65_DIR:-$(cd .. && pwd)/cc65}/bin/cl65"
CREATE_SSD="scripts/create_ssd.py"

mkdir -p "$OUT_DIR"

# Link at $1900 (PAGE when DFS is present). The cc65 bbc target defaults to
# STARTADDRESS $0E00, which collides with Acorn DFS private workspace
# ($0E00-$18FF). A program loaded+run there from a DFS disc corrupts (and is
# corrupted by) DFS, so its OSWRCH output never reaches the screen. Linking at
# $1900 (the DFS PAGE) avoids the conflict; the disc catalogue load/exec
# addresses must match.
LOAD_EXEC="001900 001900"

# stage_inf <stage_dir> <dfs_name> <load_exec>
# Write a DFS .inf sidecar so create_ssd.py uses an exact name + load/exec.
stage_inf() {
  printf '$.%s %s\n' "$2" "$3" > "$1/$2.inf"
}

# build_program <short> <c_file> [extra_src...]
# Compile a single-program disc (DFS file named <short>, disc titled <short>).
build_program() {
  local short="$1" c_file="$2"
  shift 2
  local stage="$STAGE_ROOT/$short"
  echo "  ${c_file##*/} -> $short"
  rm -rf "$stage"
  mkdir -p "$stage"
  "$CC65" -t bbc --start-addr 0x1900 -o "$stage/$short" "$c_file" "$@" 2>/dev/null
  stage_inf "$stage" "$short" "$LOAD_EXEC"
  python3 "$CREATE_SSD" -i "$stage" -o "$OUT_DIR/$short.ssd" -t "$short" >/dev/null
}

build_program TBREAK  "$SRC_DIR/test_break.c"   "$SRC_DIR/brk_helper.s"
build_program TCLOCK  "$SRC_DIR/test_clock.c"
build_program TCONSL  "$SRC_DIR/test_console.c"
build_program TFDISK  "$SRC_DIR/test_fileio.c"
build_program TKBHIT  "$SRC_DIR/test_kbhit.c"
build_program TSCREN  "$SRC_DIR/test_screen.c"
build_program TOSFIL  "$SRC_DIR/test_osfile.c"

# ---------------------------------------------------------------------------
# Directory-listing test (TDIR): needs extra catalogue entries to list.
# ---------------------------------------------------------------------------
echo "  dir -> TDIR"
stage="$STAGE_ROOT/TDIR"
rm -rf "$stage"
mkdir -p "$stage"
"$CC65" -t bbc --start-addr 0x1900 -o "$stage/TDIR" "$SRC_DIR/test_dir.c" 2>/dev/null
stage_inf "$stage" "TDIR" "$LOAD_EXEC"
printf 'alpha-data' > "$stage/ALPHA"
printf 'beta-data'  > "$stage/BETA"
stage_inf "$stage" "ALPHA" "000000 000000"
stage_inf "$stage" "BETA"  "000000 000000"
python3 "$CREATE_SSD" -i "$stage" -o "$OUT_DIR/TDIR.ssd" -t TDIR >/dev/null

echo "  Done. Output in $OUT_DIR"
