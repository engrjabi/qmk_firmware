/* Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 * Copyright 2020 Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* USB polling rate: 10ms = 100Hz (simulates BLE latency for wireless testing) */
#define USB_POLLING_INTERVAL_MS 10

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

/* PMW3360 settings */
#define ROTATIONAL_TRANSFORM_ANGLE 20
#define POINTING_DEVICE_INVERT_Y

/* Drag scroll settings */
#define PLOOPY_DRAGSCROLL_INVERT
#define PLOOPY_DRAGSCROLL_FIXED
#define PLOOPY_DRAGSCROLL_DPI 50

// Adjust these divisors to control base drag scroll sensitivity
// Lower values = faster scrolling, higher values = slower scrolling
// You can use any positive value (e.g., 4.0, 8.0, 16.0, 32.0, etc.)
#define PLOOPY_DRAGSCROLL_DIVISOR_H 50.0  // Default horizontal divisor
#define PLOOPY_DRAGSCROLL_DIVISOR_V 50.0  // Default vertical divisor

#define ENCODER_BUTTON_COL 1
#define ENCODER_BUTTON_ROW 0
/* Custom encoder needs to specify just how many encoders we have */
#define NUM_ENCODERS 1
