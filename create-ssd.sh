#!/bin/bash

# Create SSD disk image from folder contents
# Usage: create-ssd.sh -t "DiscTitle" -i input_folder -o output.ssd

set -e  # Exit on any error

# Default values
DISC_TITLE="beeb"
INPUT_FOLDER=""
OUTPUT_SSD="out.ssd"
DEFAULT_LOAD_ADDR="0x1900"
DEFAULT_DISC_SIZE="800"

usage() {
    echo "Usage: $(basename $0) -t <disc_title> -i <input_folder> -o <output.ssd> [-a <load_addr>]"
    echo "    -t     Disc title (required)"
    echo "    -i     Input folder containing files (required)"
    echo "    -o     Output SSD filename (required)"
    echo "    -a     Load address in hex (default: 0x1900)"
    echo "    -h     Show this help"
    echo
    echo "Example:"
    echo "    $(basename $0) -t \"MyPrograms\" -i ./myfiles -o programs.ssd"
    exit 1
}

# Parse command line arguments
while getopts "t:i:o:a:h" opt; do
    case $opt in
        t)
            DISC_TITLE="$OPTARG"
            ;;
        i)
            INPUT_FOLDER="$OPTARG"
            ;;
        o)
            OUTPUT_SSD="$OPTARG"
            ;;
        a)
            DEFAULT_LOAD_ADDR="$OPTARG"
            ;;
        h)
            usage
            ;;
        *)
            usage
            ;;
    esac
done

# Validate required parameters
if [ -z "$INPUT_FOLDER" ]; then
    echo "Error: Missing required parameters"
    echo
    usage
fi

# Check if input folder exists
if [ ! -d "$INPUT_FOLDER" ]; then
    echo "Error: Input folder '$INPUT_FOLDER' does not exist"
    exit 1
fi

# Check if dfstool is available
if ! command -v dfstool &> /dev/null; then
    echo "Error: dfstool not found in PATH"
    echo "Please install dfstool or add it to your PATH"
    exit 1
fi

# Convert hex address to BBC format (0x1900 -> &001900)
BBC_ADDR="&$(printf "%06X" $((DEFAULT_LOAD_ADDR)))"

echo "Creating SSD disk image..."
echo "  Disc Title: $DISC_TITLE"
echo "  Input Folder: $INPUT_FOLDER"
echo "  Output SSD: $OUTPUT_SSD"
echo "  Load Address: $BBC_ADDR"

# Create temporary JSON file
TEMP_JSON=$(mktemp)
echo "  JSON Manifest: $TEMP_JSON"

# Start building JSON manifest
cat > "$TEMP_JSON" << EOF
{
  "version": 1,
  "discTitle": "$DISC_TITLE",
  "discSize": $DEFAULT_DISC_SIZE,
  "bootOption": "none",
  "cycleNumber": 0,
  "files": [
EOF

# Process each file in the input folder
file_count=0
for file_path in "$INPUT_FOLDER"/*; do
    if [ -f "$file_path" ]; then
        filename=$(basename "$file_path")
        
        # Create short filename (max 7 chars for BBC Micro)
        short_name="${filename%.*}"  # Remove extension
        if [ ${#short_name} -gt 7 ]; then
            short_name="${short_name:0:7}"
        fi
        
        # Convert to absolute path
        abs_file_path=$(realpath "$file_path")
        
        # Add comma if not first file
        if [ $file_count -gt 0 ]; then
            echo "," >> "$TEMP_JSON"
        fi
        
        # Add file entry to JSON
        cat >> "$TEMP_JSON" << EOF
    {
      "fileName": "$short_name",
      "directory": "\$",
      "locked": false,
      "loadAddress": "$BBC_ADDR",
      "executionAddress": "$BBC_ADDR",
      "contentPath": "$abs_file_path",
      "type": "other"
    }
EOF
        file_count=$((file_count + 1))
        echo "    Added: $filename -> $short_name"
    fi
done

# Close JSON manifest
cat >> "$TEMP_JSON" << EOF
  ]
}
EOF

cat $TEMP_JSON

echo "  Total files: $file_count"

# Create SSD using dfstool
echo "Creating SSD disk image..."
if dfstool make --output "$OUTPUT_SSD" --overwrite "$TEMP_JSON"; then
    echo "✅ SSD disk image created successfully: $OUTPUT_SSD"
else
    echo "❌ Error creating SSD disk image"
    rm -f "$TEMP_JSON"
    exit 1
fi

# Clean up temporary JSON file
rm -f "$TEMP_JSON"

echo "Done!"
