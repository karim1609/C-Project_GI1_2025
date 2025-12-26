#!/bin/bash
# Build and run path test

echo "Building path test..."

# Get project directory
cd "$(dirname "$0")"

# Compile test
gcc -o /tmp/test_paths.exe test_paths.c src/utils.c src/file_manager.c -Iinclude -lm -Wall

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running test from project directory..."
    echo "Working directory: $(pwd)"
    echo ""
    /tmp/test_paths.exe
else
    echo "Build failed!"
    exit 1
fi
