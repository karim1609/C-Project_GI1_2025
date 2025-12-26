#!/bin/bash
# Build script for Student Management System

echo "Building Student Management System..."

# Get GTK flags
GTK_CFLAGS=$(pkg-config --cflags gtk+-3.0)
GTK_LIBS=$(pkg-config --libs gtk+-3.0)

# Remove old executable if exists
rm -f /tmp/student_mgmt.exe

# Compile to /tmp directory
gcc -o /tmp/student_mgmt.exe main.c src/*.c -Iinclude \
    $GTK_CFLAGS $GTK_LIBS \
    -lm -Wall 2>&1

if [ $? -eq 0 ]; then
    chmod +x /tmp/student_mgmt.exe
    echo "Build successful! Executable: /tmp/student_mgmt.exe"
    echo "Running application from project directory..."
    # IMPORTANT: Change to the directory containing this script
    # This ensures data/ paths work correctly even though executable is in /tmp
    cd "$(dirname "$0")"
    echo "Working directory: $(pwd)"
    /tmp/student_mgmt.exe
else
    echo "Build failed! Check errors above."
    exit 1
fi
