#!/bin/bash

# Board Discovery Script
# Automatically discovers Arduino boards and extracts port/FQBN information
#
# Usage: ./scripts/init.sh [board_name]
#   board_name: String to match in board name (default: "Arduino Nano Every")
#
# This script:
# - Scans for connected Arduino boards
# - Looks for boards matching the specified name
# - Extracts the port address and FQBN
# - Provides clear error messages if board not found
# - Outputs the results in a format suitable for other scripts

set -e  # Exit on any error

# Get board name from parameter or use default
BOARD_NAME="${1:-Arduino Nano Every}"

echo "🔍 Discovering Arduino Board"
echo "============================"
echo "Looking for: $BOARD_NAME"
echo ""

# Check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
    echo "❌ arduino-cli is not installed. Please install it first."
    exit 1
fi

# Check if jq is available for JSON parsing
if ! command -v jq &> /dev/null; then
    echo "❌ jq is not installed. Please install it for JSON parsing:"
    echo "  macOS: brew install jq"
    echo "  Linux: sudo apt-get install jq"
    echo "  Windows: https://stedolan.github.io/jq/download/"
    exit 1
fi

echo "✅ arduino-cli and jq found"

# Get board list and look for Nano Every
echo ""
echo "🔍 Scanning for Arduino boards..."

# Check if any boards are detected
BOARD_COUNT=$(arduino-cli board list --format json | jq '.detected_ports | length')

if [ "$BOARD_COUNT" -eq 0 ]; then
    echo "❌ No Arduino boards detected."
    echo ""
    echo "Troubleshooting:"
    echo "  - Make sure your Arduino Nano Every is connected via USB"
    echo "  - Try a different USB cable"
    echo "  - Check if the board is powered on"
    echo "  - Try unplugging and reconnecting the board"
    exit 1
fi

echo "✅ Found $BOARD_COUNT connected device(s)"

# Look for specified board
BOARD_COUNT=$(arduino-cli board list --format json | jq ".detected_ports[] | select(.matching_boards) | .matching_boards[] | select(.name | contains(\"$BOARD_NAME\")) | .name" | wc -l)

if [ "$BOARD_COUNT" -eq 0 ]; then
    echo "❌ No board matching '$BOARD_NAME' found."
    echo ""
    echo "Available boards:"
    arduino-cli board list
    echo ""
    echo "Try specifying a different board name:"
    echo "  ./scripts/init.sh 'Arduino Uno'"
    echo "  ./scripts/init.sh 'Arduino Nano'"
    exit 1
fi

echo "✅ Found board matching '$BOARD_NAME'"

# Extract port and FQBN
PORT=$(arduino-cli board list --format json | jq -r ".detected_ports[] | select(.matching_boards) | select(.matching_boards[].name | contains(\"$BOARD_NAME\")) | .port.address" | head -1)
FQBN=$(arduino-cli board list --format json | jq -r ".detected_ports[] | select(.matching_boards) | .matching_boards[] | select(.name | contains(\"$BOARD_NAME\")) | .fqbn" | head -1)

if [ -z "$PORT" ] || [ "$PORT" = "null" ]; then
    echo "❌ Could not extract port information"
    exit 1
fi

if [ -z "$FQBN" ] || [ "$FQBN" = "null" ]; then
    echo "❌ Could not extract FQBN information"
    exit 1
fi

echo ""
echo "✅ Board Information:"
echo "   Port: $PORT"
echo "   FQBN: $FQBN"
echo "   Board: $BOARD_NAME"

# Output for use by other scripts
echo "$PORT" > .board_port
echo "$FQBN" > .board_fqbn

echo ""
echo "🎉 Board discovery completed successfully!"
