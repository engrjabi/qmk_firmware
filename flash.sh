#!/bin/zsh

# Navigate to the QMK project root directory
cd "$(dirname "$0")"

# conda activate qmk_ploopy

# Flash the firmware
echo "Flashing Ploopy Trackball firmware..."
echo "Make sure your device is in bootloader mode!"
echo "Press and hold the reset button on your trackball..."
read -p "Press Enter when ready to flash..."

qmk flash -kb ploopyco/trackball/rev1_007 -km default

# Check if flashing was successful
if [ $? -eq 0 ]; then
    echo "✅ Flashing successful!"
    echo "Your trackball should now be running the new firmware"
else
    echo "❌ Flashing failed!"
    echo "Make sure the device is in bootloader mode and try again"
    exit 1
fi
