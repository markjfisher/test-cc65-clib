#
# Build ROM from cc65-clib project and copy to local roms/ directory
#

CLIB_PROJECT = ../../cc65-clib
ROMS_DIR = ../roms

.PHONY: all clean build-rom copy-rom

all: build-rom copy-rom

build-rom:
	@echo "Building cc65 CLIB ROM..."
	@$(MAKE) -C $(CLIB_PROJECT)/src clean
	@$(MAKE) -C $(CLIB_PROJECT)/src all

copy-rom:
	@echo "Copying ROM and library files..."
	@mkdir -p $(ROMS_DIR)
	@cp $(CLIB_PROJECT)/build/clib.rom $(ROMS_DIR)/
	# @cp $(CLIB_PROJECT)/build/clib.lib $(ROMS_DIR)/
	# @cp $(CLIB_PROJECT)/build/clib.map $(ROMS_DIR)/
	@echo "ROM files copied to $(ROMS_DIR)/"
	@ls -la $(ROMS_DIR)/

clean:
	@echo "Cleaning ROM build..."
	@$(MAKE) -C $(CLIB_PROJECT)/src clean
	@rm -f $(ROMS_DIR)/*
	@echo "ROM build cleaned."
