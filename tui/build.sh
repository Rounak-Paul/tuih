#!/bin/bash
# build.sh - Build script for TUI library examples (Linux/macOS)
#
# This script compiles all example programs using gcc or clang.
# Usage: ./build.sh

set -e

# Determine the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Create output directory
mkdir -p bin

# Compiler selection: prefer clang on macOS, gcc on Linux
if command -v clang &> /dev/null && [[ "$(uname)" == "Darwin" ]]; then
    CC=clang
elif command -v gcc &> /dev/null; then
    CC=gcc
elif command -v clang &> /dev/null; then
    CC=clang
else
    echo "Error: No C compiler found. Please install gcc or clang."
    exit 1
fi

# Compiler flags
CFLAGS="-std=c11 -Wall -Wextra -Wpedantic -O2"

# Platform-specific flags
if [[ "$(uname)" == "Darwin" ]]; then
    # macOS specific flags
    CFLAGS="$CFLAGS -Wno-unused-parameter"
else
    # Linux specific flags
    CFLAGS="$CFLAGS -D_POSIX_C_SOURCE=200809L"
fi

echo "==================================="
echo "TUI Library Build Script"
echo "==================================="
echo "Compiler: $CC"
echo "Flags: $CFLAGS"
echo ""

# Build each example
build_example() {
    local src="$1"
    local name=$(basename "$src" .c)
    local out="bin/$name"
    
    echo -n "Building $name... "
    $CC $CFLAGS -o "$out" "$src"
    echo "OK"
}

echo "Building examples..."
echo "-----------------------------------"

build_example "examples/hello.c"
build_example "examples/buttons.c"
build_example "examples/input.c"
build_example "examples/widgets_demo.c"

echo "-----------------------------------"
echo ""
echo "Build complete! Binaries are in bin/"
echo ""
echo "Run examples:"
echo "  ./bin/hello         - Label rendering demo"
echo "  ./bin/buttons       - Interactive buttons demo"
echo "  ./bin/input         - Keyboard handling demo"
echo "  ./bin/widgets_demo  - Hierarchical widgets demo"
echo ""
