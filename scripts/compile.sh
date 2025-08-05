#!/bin/bash

# Compile Script
# Compiles the ProjectorController Arduino sketch for the discovered board
#
# This script:
# - Reads board information from discovery script
# - Compiles the Arduino sketch for the specific board
# - Provides detailed error messages if compilation fails
# - Ensures all dependencies are available

set -e  # Exit on any error

echo "🔧 Compiling ProjectorController"
echo "================================"

# Check if we're in the right directory
if [ ! -f "src/ProjectorController/ProjectorController.ino" ]; then
    echo "❌ ProjectorController.ino not found."
    echo "   Make sure you're in the project root directory."
    exit 1
fi

echo "✅ Found ProjectorController.ino"

# Check if board information is available
if [ ! -f ".board_fqbn" ]; then
    echo "❌ Board information not found."
    echo "   Please run the board discovery script first:"
    echo "   ./scripts/init.sh [board name]"
    exit 1
fi

FQBN=$(cat .board_fqbn)
echo "✅ Using FQBN: $FQBN"

# Check if dependencies are installed
if ! arduino-cli lib list | grep -q "IRremote"; then
    echo "❌ IRRemote library not found."
    echo "   Please install dependencies first:"
    echo "   ./scripts/install-deps.sh"
    exit 1
fi

echo "✅ Dependencies verified"

# Compile the project
echo ""
echo "🔧 Compiling for $FQBN..."
echo "   This may take a moment..."

if arduino-cli compile --fqbn "$FQBN" src/ProjectorController/; then
    echo ""
    echo "✅ Compilation successful!"
    echo "   The sketch is ready for upload"
else
    echo ""
    echo "❌ Compilation failed!"
    echo ""
    echo "Troubleshooting:"
    echo "  - Check for syntax errors in the Arduino sketch"
    echo "  - Verify all required libraries are installed"
    echo "  - Make sure the FQBN is correct for your board"
    echo "  - Try running: ./scripts/install-deps.sh"
    exit 1
fi 
