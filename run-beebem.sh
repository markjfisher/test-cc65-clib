#!/bin/bash

# requires WINE to be setup with D: set to parent of bbc folder ($HOME/dev in my case)

# Default values
BEEBEM_DATA_DIR=${BEEBEM_DATA_DIR:-"D:\\bbc\\roms"}
ROMS_CONFIG="clib-roms.cfg"
PREFS_CONFIG="clib-prefs.cfg"
DISK_IMAGE="build/test-c-comprehensive/test.ssd"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
  case $1 in
    -d)
      DISK_IMAGE="$2"
      shift 2
      ;;
    -r)
      ROMS_CONFIG="$2"
      shift 2
      ;;
    -p)
      PREFS_CONFIG="$2"
      shift 2
      ;;
    -h|--help)
      echo "Usage: $0 [OPTIONS]"
      echo ""
      echo "Options:"
      echo "  -d <disk_image>    Disk image file (default: build/test-c-comprehensive/test.ssd)"
      echo "  -r <roms_config>   ROMs config file (default: clib-roms.cfg)"
      echo "  -p <prefs_config>  Preferences config file (default: clib-prefs.cfg)"
      echo "  -h, --help         Show this help message"
      echo ""
      echo "Environment variables:"
      echo "  BEEBEM_DATA_DIR    Data directory for BeebEm (default: D:\\bbc\\roms)"
      exit 0
      ;;
    *)
      echo "Unknown option: $1"
      echo "Use -h or --help for usage information"
      exit 1
      ;;
  esac
done

$HOME/external/BeebEm/BeebEm.exe \
  -NoAutoBoot \
  -Data "$BEEBEM_DATA_DIR" \
  -Roms "$BEEBEM_DATA_DIR\\$ROMS_CONFIG" \
  -Prefs "$BEEBEM_DATA_DIR\\$PREFS_CONFIG" \
  "$DISK_IMAGE"
