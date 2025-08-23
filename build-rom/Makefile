#
# Build ROM from cc65-clib project and copy to local roms/ directory
#

CC65_ROOT = ../../cc65
CLIB_ROOT = ../../cc65-clib
CC_TARGET = bbc-clib

ROM_PATH = ../roms
BUILD_DIR = ../build
TARGET_ROM_COPY = $(HOME)/dev/bbc/roms

.PHONY: all clean rebuild-clib rebuild-clib-rom rebuild-cc65-lib clean-all

all: rebuild-clib

rebuild-clib: rebuild-clib-rom rebuild-cc65-lib

rebuild-clib-rom:
	@echo "Cleaning and rebuilding cc65-clib ROM..."
	cd $(CLIB_ROOT)/src && $(MAKE) clean
	cd $(CLIB_ROOT)/src && $(MAKE)
	@echo "ROM build complete - manifest files generated, and copied to cc65"
	@echo "Copying ROM and VICE symbol files to test project roms directory..."
	mkdir -p $(ROM_PATH)
	cp $(CLIB_ROOT)/build/clib.rom $(ROM_PATH)/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(ROM_PATH)/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(ROM_PATH)/clib-mos.lbl
	@echo "Copying ROM and VICE symbol files to emulator roms directory..."
	mkdir -p $(TARGET_ROM_COPY)
	cp $(CLIB_ROOT)/build/clib.rom $(TARGET_ROM_COPY)/clib.rom
	cp $(CLIB_ROOT)/build/clib.lbl $(TARGET_ROM_COPY)/clib.lbl
	cp $(CLIB_ROOT)/build/clib-mos.lbl $(TARGET_ROM_COPY)/clib-mos.lbl

# Build the production cc65-clib.lib if needed  
rebuild-cc65-lib:
	@echo "Building cc65 $(CC_TARGET) library..."
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET) clean
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET)

clean-all: clean
	cd $(CC65_ROOT) && $(MAKE) -C libsrc TARGETS=$(CC_TARGET) clean
	cd $(CLIB_ROOT)/src && $(MAKE) clean
	-rm -f $(ROM_PATH)/clib.rom
	-rm -f $(ROM_PATH)/clib.lbl
	-rm -f $(ROM_PATH)/clib-mos.lbl

clean: clean-all
