#!/bin/bash

# Navigate to the QMK project root directory
cd "$(dirname "$0")"

# conda activate qmk_ploopy

# Compile the firmware
echo "Compiling Ploopy Trackball firmware..."
qmk compile -kb ploopyco/trackball/rev1_007 -km default

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "Firmware files generated in .build directory"
else
    echo "❌ Compilation failed!"
    exit 1
fi