# Makefile for comprehensive C library testing
# Tests wide range of functions to validate ROM/local function splitting

CC65_ROOT = ../../../cc65
CLIB_ROOT = ../../../cc65-clib
ROM_PATH = ../../roms
BUILD_DIR = ../../build
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

# Build the ROM if it doesn't exist
$(ROM_PATH)/clib.rom:
	@echo "Building cc65-clib ROM..."
	cd $(CLIB_ROOT)/src && $(MAKE)
	@echo "Copying ROM and VICE symbol files to test project roms directory..."
	mkdir -p $(ROM_PATH)
	cp $(CLIB_ROOT)/build/clib.rom $(ROM_PATH)/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(ROM_PATH)/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(ROM_PATH)/clib-mos.lbl
	@echo "Copying ROM and VICE symbol files to emulator roms directory..."
	mkdir -p $(HOME)/dev/bbc/roms
	cp $(CLIB_ROOT)/build/clib.rom $(HOME)/dev/bbc/roms/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(HOME)/dev/bbc/roms/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(HOME)/dev/bbc/roms/clib-mos.lbl

# Build the production cc65-clib.lib if needed  
$(CC65_ROOT)/lib/$(CC_TARGET).lib:
	@echo "Building cc65 $(CC_TARGET) library..."
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET)

# Build the C test executable
$(TEST_BUILD_DIR)/test: test.c $(TEST_BUILD_DIR) $(CC65_ROOT)/lib/$(CC_TARGET).lib
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

# Rebuild tasks for development workflow
rebuild-cc65-lib:
	@echo "Cleaning and rebuilding cc65 bbc-clib library..."
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET) clean
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET)

rebuild-clib-rom:
	@echo "Cleaning and rebuilding cc65-clib ROM..."
	cd $(CLIB_ROOT)/src && $(MAKE) clean
	cd $(CLIB_ROOT)/src && $(MAKE)
	@echo "Copying ROM and VICE symbol files to test project roms directory..."
	mkdir -p $(ROM_PATH)
	cp $(CLIB_ROOT)/build/clib.rom $(ROM_PATH)/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(ROM_PATH)/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(ROM_PATH)/clib-mos.lbl
	@echo "Copying ROM and VICE symbol files to emulator roms directory..."
	mkdir -p $(HOME)/dev/bbc/roms
	cp $(CLIB_ROOT)/build/clib.rom $(HOME)/dev/bbc/roms/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(HOME)/dev/bbc/roms/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(HOME)/dev/bbc/roms/clib-mos.lbl

# Combined rebuild tasks
rebuild-clib: rebuild-cc65-lib rebuild-clib-rom

# Production build (same as regular build for C test)
production-disk: test-disk

# Build simple C debug test
simple-debug: clean-debug $(TEST_BUILD_DIR)
	@echo "Building simple C debug test with fresh debug library..."
	cd ../test-rom-simple && $(MAKE) ../../build/test-rom-simple/debug-clib.lib
	$(CC65_ROOT)/bin/cc65 -t $(CC_TARGET) $(CC_ARGS) simple.c
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) --listing $(TEST_BUILD_DIR)/simple.lst simple.s
	$(CC65_ROOT)/bin/ld65 -t $(CC_TARGET) -Ln $(TEST_BUILD_DIR)/simple-debug.lbl --mapfile $(TEST_BUILD_DIR)/simple-debug.map --start-addr 0x1900 -o $(TEST_BUILD_DIR)/simple-debug simple.o ../test-rom-simple/../../build/test-rom-simple/debug-clib.lib
	dfstool make --output $(TEST_BUILD_DIR)/simple-debug.ssd --overwrite simple-debug.json
	@echo "Simple C debug test complete! File: $(TEST_BUILD_DIR)/simple-debug.ssd"

# Build comprehensive C debug test
test-debug: clean-debug $(TEST_BUILD_DIR)
	@echo "Building comprehensive C debug test with fresh debug library..."
	cd ../test-rom-simple && $(MAKE) ../../build/test-rom-simple/debug-clib.lib
	$(CC65_ROOT)/bin/cc65 -t $(CC_TARGET) $(CC_ARGS) test.c
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) --listing $(TEST_BUILD_DIR)/test.lst test.s
	$(CC65_ROOT)/bin/ld65 -t $(CC_TARGET) -Ln $(TEST_BUILD_DIR)/test-debug.lbl --mapfile $(TEST_BUILD_DIR)/test-debug.map --start-addr 0x1900 -o $(TEST_BUILD_DIR)/test-debug test.o ../test-rom-simple/../../build/test-rom-simple/debug-clib.lib
	dfstool make --output $(TEST_BUILD_DIR)/test-debug.ssd --overwrite test-debug.json
	@echo "Comprehensive C debug test complete! File: $(TEST_BUILD_DIR)/test-debug.ssd"

# Clean targets
clean:
	rm -rf $(TEST_BUILD_DIR)
	rm -f *.o *.s
	@echo "Cleaned local build artifacts and test build directory"

clean-debug: 
	cd ../test-rom-simple && rm -f ../../build/test-rom-simple/debug-clib.lib
	rm -rf $(TEST_BUILD_DIR)
	rm -f *.o *.s
	@echo "Cleaned debug library and local artifacts"

clean-all: clean
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET) clean
	cd $(CLIB_ROOT)/src && $(MAKE) clean
	rm -f $(ROM_PATH)/clib.rom
	rm -f $(ROM_PATH)/clib.lbl
	rm -f $(ROM_PATH)/clib-mos.lbl
	rm -f $(HOME)/dev/bbc/roms/clib.rom
	rm -f $(HOME)/dev/bbc/roms/clib.lbl
	rm -f $(HOME)/dev/bbc/roms/clib-mos.lbl

.PHONY: all test-disk production-disk simple-debug test-debug clean clean-debug clean-all production rebuild-cc65-lib rebuild-clib-rom rebuild-clib
