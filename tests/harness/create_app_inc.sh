#!/bin/bash
# Convert a cc65 .lbl (VICE label file) into a ca65-compatible .inc file
# that the harness can include to reference symbols from the test binary
# or cc65 library.
#
# Usage: ./create_app_inc.sh <label_file> [output_file]
#
# Default output: ./app.inc (in current directory for harness.yaml)

set -euo pipefail

LABEL_FILE="${1:-${WS_ROOT}/build/hello/test.lbl}"
OUTPUT_FILE="${2:-app.inc}"

if [[ ! -f "$LABEL_FILE" ]]; then
  echo "Label file not found: $LABEL_FILE"
  echo "Build the test binary first with the bbc target."
  exit 1
fi

# Remove cc65 auto-generated symbols (__MAIN_START__, etc.)
# Convert from: "al 00ABCD .symbol_name"
# To:          "symbol_name := $ABCD"
awk '!/\.__.*__/ {
    printf "%s := $%X\n", substr($3, 2), strtonum("0x" $2)
}' "$LABEL_FILE" > "$OUTPUT_FILE"

echo "Created $OUTPUT_FILE from $LABEL_FILE"