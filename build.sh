#!/bin/bash

# Build script for BBC Micro CLIB ROM testing framework
# Builds ROM and multiple test programs using cc65 bbc-clib target

set -e  # Exit on any error

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

test_dirs=(
  "tests/test-c-comprehensive"
  "tests/test-maths"
  "tests/test-strings"
  "tests/test-break-handler"
  "tests/test-files"
)

# Configuration
START_ADDR="0x1900"

# Function to build a single test
build_test() {
  local test_dir=$1
  local test_name=$(basename $test_dir)

  echo "Building test: $test_name"

  if [ ! -d "$test_dir" ]; then
    echo "Error: Test directory $test_dir not found"
    return 1
  fi

  cd "$test_dir"
  make clean
  make all
  cd - > /dev/null
}

# Function to build ROM if needed
ensure_rom() {
  if [ ! -f "roms/clib.rom" ] || [ ! -f "roms/clib.lib" ]; then
    echo "ROM not found, building..."
    make -C build-rom all
  else
    echo "ROM already available"
  fi
}

force_rom() {
  echo "Forcing ROM/cc65 build"
  make -C build-rom clean all
}

# Function to build all tests
build_all_tests() {
  for test_dir in "${test_dirs[@]}"; do
    if [ -d "$test_dir" ]; then
      create_json $START_ADDR $test_dir
      build_test "$test_dir"
    else
      echo "Warning: Test directory $test_dir not found, skipping"
    fi
  done
}

# Function to create disk images with JSON manifests
create_json() {
  echo
  local start_addr_hex=$1
  local test_dir=$2
  echo "Creating json file for $test_dir"
  # Convert hex address to BBC format (0x1900 -> &001900)
  local bbc_addr="&$(printf "%06X" $((start_addr_hex)))"
  echo "Using BBC load/exec address: $bbc_addr"

  local json_name="test.json"
  local build_path=${SCRIPT_DIR}/build/$(basename $test_dir)
  local exe_path=${build_path}/test

  # Create JSON manifest
  cat > "$test_dir/$json_name" << EOF
{
  "version": 1,
  "discTitle": "ctest",
  "discSize": 800,
  "bootOption": "none", 
  "cycleNumber": 0,
  "files": [
    {
      "fileName": "TEST",
      "directory": "\$",
      "locked": false,
      "loadAddress": "$bbc_addr",
      "executionAddress": "$bbc_addr", 
      "contentPath": "$exe_path",
      "type": "other"
    }
  ]
}
EOF
}

# Function to show results
show_results() {
  echo "ROM files:"
  ls -la roms/ 2>/dev/null || echo "No ROM files found"

  echo
  echo "Test executables:"
  find tests -name "test" -type f -exec ls -la {} \; 2>/dev/null || echo "No test executables found"

  echo
  echo "Disk images:"
  find build -name \*.ssd -type f -exec ls -la {} \; 2>/dev/null || echo "No disk images found"
}

usage() {
  echo "Usage: $(basename $0) [-r|-t|-c|-a|-h]"
  echo "    -r     force clib ROM and cc65-clib re-build"
  echo "    -t     run all Tests"
  echo "    -c     Clean tests"
  echo "    -a     All: build tests, create disks"
  echo "    -h     Show Help"
  exit 0
}

while getopts "rtcah" opt; do
  case $opt in
    r)
      force_rom
      exit 0
      ;;
    t)
      build_all_tests
      exit 0
      ;;
    c)
      echo "Cleaning all tests..."
      # make -C build-rom clean
      for test_dir in tests/*/; do
        if [ -f "$test_dir/Makefile" ]; then
          make -C "$test_dir" clean
        fi
      done
      echo "Clean completed"
      exit 0
      ;;
    a)
      echo "=== BBC CLIB ROM Test Framework ==="
      ensure_rom
      build_all_tests
      show_results
      echo
      echo "✅ Build completed successfully!"
      echo "To test: Load ROM (roms/clib.rom) into BBC emulator and run test executables"
      exit 0
      ;;
    h|*)
      usage
      exit 0
  esac
done

usage