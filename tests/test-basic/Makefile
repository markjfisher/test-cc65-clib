#
# Makefile for test-basic (BBC CLIB ROM target test)
#

TARGET = test
SOURCE = test.c
ROM_PATH = ../../roms
START_ADDR = 0x1900

# BBC CLIB target requires ROM stubs
CC_TARGET = bbc-clib

.PHONY: all clean build run

all: build

build: $(TARGET)

$(TARGET): $(SOURCE) $(ROM_PATH)/clib.rom
	@echo "Building $(TARGET) with $(CC_TARGET) target..."
	cl65 -t $(CC_TARGET) -Ln $(TARGET).lbl --start-addr $(START_ADDR) -o $(TARGET) $(SOURCE)
	@echo "Build complete: $(TARGET)"

$(ROM_PATH)/clib.rom:
	@echo "Building ROM first..."
	@$(MAKE) -C ../../build-rom all

clean:
	@rm -f $(TARGET) $(TARGET).lbl

run: $(TARGET)
	@echo "To run this test, load $(TARGET) into a BBC emulator"
	@echo "For ROM testing, also install: $(ROM_PATH)/clib.rom"
