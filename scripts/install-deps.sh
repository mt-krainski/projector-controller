#!/bin/bash

# Install Dependencies Script
# Installs all required Arduino libraries for the ProjectorController project
#
# This script:
# - Checks if arduino-cli is installed
# - Installs the arduino:megaavr platform needed for compilation
# - Installs the IRRemote library needed for IR signal transmission/reception
# - Provides clear error messages if anything fails

set -e  # Exit on any error

echo "📚 Installing ProjectorController Dependencies"
echo "=============================================="

# Check if arduino-cli is installed
if ! command -v arduino-cli &> /dev/null; then
    echo "❌ arduino-cli is not installed."
    echo ""
    echo "Please install it first:"
    echo "  macOS: brew install arduino-cli"
    echo "  Linux: https://arduino.github.io/arduino-cli/latest/installation/"
    echo "  Windows: https://arduino.github.io/arduino-cli/latest/installation/"
    exit 1
fi

echo "✅ arduino-cli found"

# Install arduino:megaavr platform
echo ""
echo "📦 Installing arduino:megaavr platform..."
if arduino-cli core install "arduino:megaavr"; then
    echo "✅ arduino:megaavr platform installed successfully"
else
    echo "❌ Failed to install arduino:megaavr platform"
    echo ""
    echo "Troubleshooting:"
    echo "  - Check your internet connection"
    echo "  - Try running: arduino-cli core update-index"
    echo "  - Verify the platform name is correct"
    exit 1
fi

# Install IRRemote library
echo ""
echo "📦 Installing IRRemote library..."
if arduino-cli lib install "IRRemote"; then
    echo "✅ IRRemote library installed successfully"
else
    echo "❌ Failed to install IRRemote library"
    echo ""
    echo "Troubleshooting:"
    echo "  - Check your internet connection"
    echo "  - Try running: arduino-cli lib update"
    echo "  - Verify the library name is correct"
    exit 1
fi

echo ""
echo "🎉 All dependencies installed successfully!"
echo "   Ready to compile and upload the ProjectorController" 