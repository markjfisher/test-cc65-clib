#!/bin/bash

# Build script for BBC Micro CLIB ROM testing framework
# Builds ROM and multiple test programs using cc65 bbc-clib target

set -e  # Exit on any error

# Configuration
START_ADDR="0x1900"

echo "=== BBC CLIB ROM Test Framework ==="

# Function to build a single test
build_test() {
    local test_dir=$1
    local test_name=$(basename $test_dir)
    
    echo
    echo "Building test: $test_name"
    echo "================================"
    
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
    echo "Step 1: Ensuring ROM is available..."
    if [ ! -f "roms/clib.rom" ] || [ ! -f "roms/clib.lib" ]; then
        echo "ROM not found, building..."
        make -C build-rom all
    else
        echo "ROM already available"
    fi
}

# Function to build all tests
build_all_tests() {
    echo
    echo "Step 2: Building all test programs..."
    
    local test_dirs=(
        "tests/test-basic"
        "tests/test-strings" 
        "tests/test-math"
        "tests/test-rom-detection"
    )
    
    for test_dir in "${test_dirs[@]}"; do
        if [ -d "$test_dir" ]; then
            build_test "$test_dir"
        else
            echo "Warning: Test directory $test_dir not found, skipping"
        fi
    done
}

# Function to create disk images with JSON manifests
create_disks() {
    echo
    echo "Step 3: Creating disk images..."
    
    if ! command -v dfstool &> /dev/null; then
        echo "dfstool not available, skipping disk creation"
        return
    fi
    
    # Convert hex address to BBC format (0x1900 -> &001900)
    local bbc_addr="&$(printf "%06X" $((START_ADDR)))"
    echo "Using BBC load/exec address: $bbc_addr"
    
    # Create individual test disks
    local test_dirs=(
        "tests/test-basic"
        "tests/test-strings" 
        "tests/test-math"
        "tests/test-rom-detection"
    )
    
    for test_dir in "${test_dirs[@]}"; do
        if [ -f "$test_dir/test" ]; then
            local test_name=$(basename $test_dir)
            local disk_name="${test_name}.ssd"
            local json_name="${test_name}.json"
            
            echo "Creating disk for $test_name..."
            
            # Create JSON manifest
            cat > "$json_name" << EOF
{
  "version": 1,
  "discTitle": "$test_name",
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
      "contentPath": "$test_dir/test",
      "type": "other"
    }
  ]
}
EOF
            
            # Create SSD disk image
            dfstool create-ssd "$disk_name" "$json_name"
            echo "Created: $disk_name with $json_name"
        else
            echo "Skipping $test_dir - no test executable found"
        fi
    done
    
    # Create a combined disk with ROM + one test
    if [ -f "roms/clib.rom" ] && [ -f "tests/test-basic/test" ]; then
        echo "Creating combined ROM + test disk..."
        
        cat > "rom_test.json" << EOF
{
  "version": 1,
  "discTitle": "ROM+TEST",
  "discSize": 800,
  "bootOption": "none",
  "cycleNumber": 0, 
  "files": [
    {
      "fileName": "CLIB",
      "directory": "\$",
      "locked": false,
      "loadAddress": "&008000",
      "executionAddress": "&008000",
      "contentPath": "roms/clib.rom",
      "type": "other"
    },
    {
      "fileName": "TEST",
      "directory": "\$", 
      "locked": false,
      "loadAddress": "$bbc_addr",
      "executionAddress": "$bbc_addr",
      "contentPath": "tests/test-basic/test",
      "type": "other"
    }
  ]
}
EOF
        
        dfstool create-ssd "rom_test.ssd" "rom_test.json"
        echo "Created: rom_test.ssd with ROM and test program"
    fi
}

# Function to show results
show_results() {
    echo
    echo "Step 4: Build Results"
    echo "===================="
    echo "ROM files:"
    ls -la roms/ 2>/dev/null || echo "No ROM files found"
    
    echo
    echo "Test executables:"
    find tests -name "test" -type f -exec ls -la {} \; 2>/dev/null || echo "No test executables found"
    
    echo
    echo "Disk images:"
    ls -la *.ssd *.json 2>/dev/null || echo "No disk images found"
}

# Main execution
case "${1:-all}" in
    "rom")
        ensure_rom
        ;;
    "tests")
        build_all_tests
        ;;
    "clean")
        echo "Cleaning all builds..."
        make -C build-rom clean
        for test_dir in tests/*/; do
            if [ -f "$test_dir/Makefile" ]; then
                make -C "$test_dir" clean
            fi
        done
        echo "Clean completed"
        ;;
    "all"|*)
        ensure_rom
        build_all_tests
        create_disks
        show_results
        echo
        echo "✅ Build completed successfully!"
        echo "To test: Load ROM (roms/clib.rom) into BBC emulator and run test executables"
        ;;
esac