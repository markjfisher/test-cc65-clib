#!/bin/bash

DISK_IMAGE="build/test-c-comprehensive/test.ssd"

# Parse command line arguments
while getopts "d:h" opt; do
  case $opt in
    d)
      DISK_IMAGE="$OPTARG"
      ;;
    h)
      echo "Usage: $0 [OPTIONS]"
      echo ""
      echo "Options:"
      echo "  -d <disk_image>    Disk image file (default: build/test-c-comprehensive/test.ssd)"
      echo "  -h                 Show this help message"
      echo ""
      echo "Environment variables:"
      echo "  BEEBEM_DATA_DIR    Data directory for BeebEm (default: D:\\bbc\\roms)"
      exit 0
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      echo "Use -h for usage information"
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument" >&2
      exit 1
      ;;
  esac
done

shift $((OPTIND-1))

b2-debug \
  -0 "$DISK_IMAGE"
