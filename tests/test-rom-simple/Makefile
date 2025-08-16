# Minimal Assembly ROM Test
# Tests ROM functions without C library dependencies

CC_TARGET = bbc-clib
CC65_ROOT = ../../../cc65
CLIB_ROOT = ../../../cc65-clib
ROM_PATH = ../../roms
BUILD_DIR = ../../build
TEST_BUILD_DIR = $(BUILD_DIR)/test-rom-simple

# Main build target (debug version)
all: debug-disk
	@echo "Debug build complete! Files ready:"
	@echo "  ROM: $(ROM_PATH)/clib.rom"
	@echo "  Disk: $(TEST_BUILD_DIR)/simple.ssd"

# Production build target (full crt0.s - will need missing functions added)
production: production-disk
	@echo "Production build complete! Files ready:"
	@echo "  ROM: $(ROM_PATH)/clib.rom"
	@echo "  Disk: $(TEST_BUILD_DIR)/simple.ssd"

# Build DEBUG library for testing (uses local debug_crt0.s)
$(TEST_BUILD_DIR)/debug-clib.lib: debug_crt0.s $(CC65_ROOT)/libsrc/$(CC_TARGET)/*.s | $(TEST_BUILD_DIR)
	@echo "Building DEBUG $(CC_TARGET) library with local debug_crt0.s..."
	@mkdir -p $(TEST_BUILD_DIR)/libwrk
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/libwrk/debug_crt0.o debug_crt0.s
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/libwrk/rom_detect.o $(CC65_ROOT)/libsrc/$(CC_TARGET)/rom_detect.s  
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/libwrk/rom_error.o $(CC65_ROOT)/libsrc/$(CC_TARGET)/rom_error.s
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/libwrk/rom_stubs.o $(CC65_ROOT)/libsrc/$(CC_TARGET)/rom_stubs.s
	rm -f $(TEST_BUILD_DIR)/debug-clib.lib
	$(CC65_ROOT)/bin/ar65 a $(TEST_BUILD_DIR)/debug-clib.lib $(TEST_BUILD_DIR)/libwrk/debug_crt0.o $(TEST_BUILD_DIR)/libwrk/rom_detect.o $(TEST_BUILD_DIR)/libwrk/rom_error.o $(TEST_BUILD_DIR)/libwrk/rom_stubs.o

# Build cc65 bbc-clib library if needed (PRODUCTION version)
$(CC65_ROOT)/lib/$(CC_TARGET).lib: $(CC65_ROOT)/libsrc/$(CC_TARGET)/*.s
	@echo "Building PRODUCTION cc65 $(CC_TARGET) library with ALL functions..."
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGET=$(CC_TARGET) clean
	cd $(CC65_ROOT) && mkdir -p libwrk/$(CC_TARGET)
	@echo "  Compiling core runtime files..."
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/crt0.o libsrc/$(CC_TARGET)/crt0.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/zerobss.o libsrc/common/zerobss.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/rom_detect.o libsrc/$(CC_TARGET)/rom_detect.s  
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/rom_error.o libsrc/$(CC_TARGET)/rom_error.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/rom_stubs.o libsrc/$(CC_TARGET)/rom_stubs.s
	@echo "  Compiling missing C runtime functions..."
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/callmain.o libsrc/runtime/callmain.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/condes.o libsrc/runtime/condes.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/raise.o libsrc/common/raise.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/signal.o libsrc/common/signal.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/sigtable.o libsrc/common/sigtable.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/seterrno.o libsrc/common/_seterrno.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/errno.o libsrc/common/errno.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/jmpvec.o libsrc/runtime/jmpvec.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/disable_cursor_edit.o libsrc/bbc/disable_cursor_edit.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/brkvec.o libsrc/bbc/brkvec.s
	cd $(CC65_ROOT) && bin/ca65 -t $(CC_TARGET) -o libwrk/$(CC_TARGET)/brkret.o libsrc/bbc/brkret.s
	cd $(CC65_ROOT) && rm -f lib/$(CC_TARGET).lib
	@echo "  Creating library archive..."
	cd $(CC65_ROOT) && bin/ar65 a lib/$(CC_TARGET).lib libwrk/$(CC_TARGET)/crt0.o libwrk/$(CC_TARGET)/zerobss.o libwrk/$(CC_TARGET)/rom_detect.o libwrk/$(CC_TARGET)/rom_error.o libwrk/$(CC_TARGET)/rom_stubs.o libwrk/$(CC_TARGET)/callmain.o libwrk/$(CC_TARGET)/condes.o libwrk/$(CC_TARGET)/raise.o libwrk/$(CC_TARGET)/signal.o libwrk/$(CC_TARGET)/sigtable.o libwrk/$(CC_TARGET)/seterrno.o libwrk/$(CC_TARGET)/errno.o libwrk/$(CC_TARGET)/jmpvec.o libwrk/$(CC_TARGET)/disable_cursor_edit.o libwrk/$(CC_TARGET)/brkvec.o libwrk/$(CC_TARGET)/brkret.o

# Build cc65-clib ROM if needed  
$(ROM_PATH)/clib.rom: $(CLIB_ROOT)/src/libsrc/bbc/*.s $(CLIB_ROOT)/src/libsrc/common/*.s
	@echo "Building cc65-clib ROM..."
	cd $(CLIB_ROOT)/src && $(MAKE) all
	@mkdir -p $(ROM_PATH)
	cp $(CLIB_ROOT)/build/clib.rom $(ROM_PATH)/clib.rom

# Build minimal assembly test (using DEBUG library with debug output)
$(TEST_BUILD_DIR)/simple: simple.s $(TEST_BUILD_DIR)/debug-clib.lib $(ROM_PATH)/clib.rom | $(TEST_BUILD_DIR)
	@echo "Building minimal ROM test with debug startup..."
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/simple.o simple.s
	$(CC65_ROOT)/bin/ld65 -t $(CC_TARGET) -Ln $(TEST_BUILD_DIR)/simple.lbl --mapfile $(TEST_BUILD_DIR)/simple.map --start-addr 0x1900 -o $(TEST_BUILD_DIR)/simple $(TEST_BUILD_DIR)/simple.o $(TEST_BUILD_DIR)/debug-clib.lib

# Create build directory
$(TEST_BUILD_DIR):
	@mkdir -p $(TEST_BUILD_DIR)

# Build test disk (DEBUG version)
debug-disk: $(TEST_BUILD_DIR)/simple
	@echo "Creating debug test disk..."
	dfstool make --output $(TEST_BUILD_DIR)/simple.ssd --overwrite simple.json

# Build production version of simple test  
$(TEST_BUILD_DIR)/simple-production: simple.s $(CC65_ROOT)/lib/$(CC_TARGET).lib $(ROM_PATH)/clib.rom | $(TEST_BUILD_DIR)
	@echo "Building production ROM test with full crt0.s..."
	$(CC65_ROOT)/bin/ca65 -t $(CC_TARGET) -o $(TEST_BUILD_DIR)/simple-production.o simple.s
	$(CC65_ROOT)/bin/ld65 -t $(CC_TARGET) -Ln $(TEST_BUILD_DIR)/simple-production.lbl --mapfile $(TEST_BUILD_DIR)/simple-production.map --start-addr 0x1900 -o $(TEST_BUILD_DIR)/simple-production $(TEST_BUILD_DIR)/simple-production.o $(CC65_ROOT)/lib/$(CC_TARGET).lib

# Build test disk (PRODUCTION version)
production-disk: $(TEST_BUILD_DIR)/simple-production
	@echo "Creating production test disk..."
	dfstool make --output $(TEST_BUILD_DIR)/simple.ssd --overwrite simple.json

# Clean build artifacts
clean:
	rm -rf $(TEST_BUILD_DIR)
	
# Clean everything including dependencies
clean-all: clean
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGET=$(CC_TARGET) clean
	rm -f $(CC65_ROOT)/lib/$(CC_TARGET).lib
	cd $(CLIB_ROOT)/src && $(MAKE) clean
	rm -f $(ROM_PATH)/clib.rom

# Force rebuild of everything
rebuild: clean-all all

.PHONY: all production clean clean-all rebuild debug-disk production-disk
