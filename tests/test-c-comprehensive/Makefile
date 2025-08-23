# Makefile for comprehensive C library testing
# Tests wide range of functions to validate ROM/local function splitting

CC65_ROOT = ../../../cc65
CLIB_ROOT = ../../../cc65-clib
ROM_PATH = ../../roms
BUILD_DIR = ../../build
BUILD_ROM_DIR = ../../build-rom
TEST_BUILD_DIR = $(BUILD_DIR)/test-c-comprehensive
CC_TARGET = bbc-clib
CC_ARGS = -Osir
CA_ARGS = -Osir

# Default target - build for testing  
all: test-disk

production: production-disk

# Ensure build directory exists
$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

# Build the C test executable
$(TEST_BUILD_DIR)/test: test.c $(TEST_BUILD_DIR) build-rom
	@echo "Building comprehensive C test..."
	$(CC65_ROOT)/bin/cl65 $(CC_ARGS) -t $(CC_TARGET) -Ln $(TEST_BUILD_DIR)/test.lbl --mapfile $(TEST_BUILD_DIR)/test.map --start-addr 0x1900 -o $(TEST_BUILD_DIR)/test test.c

# Create test disk image
test-disk: $(TEST_BUILD_DIR)/test $(ROM_PATH)/clib.rom
	@echo "Creating C test disk..."
	dfstool make --output $(TEST_BUILD_DIR)/test.ssd --overwrite test.json
	@echo "Comprehensive C test build complete!"
	@echo "Files ready:"
	@echo "  ROM: $(ROM_PATH)/clib.rom"
	@echo "  Disk: $(TEST_BUILD_DIR)/test.ssd"

# Production build (same as regular build for C test)
production-disk: test-disk

# Clean targets
clean:
	rm -rf $(TEST_BUILD_DIR)
	rm -f *.o *.s
	@echo "Cleaned local build artifacts and test build directory"

build-rom:
	cd $(BUILD_ROM_DIR) && $(MAKE) rebuild-clib

.PHONY: all test-disk production-disk clean production
