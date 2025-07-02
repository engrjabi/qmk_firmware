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
#include QMK_KEYBOARD_H

// Custom keycodes
enum custom_keycodes {
    DRAG_OR_META = SAFE_RANGE  // This will handle both drag scroll and Meta+D
};

// Tapdance declarations (for other buttons)
enum {
    TD_BTN2_ESC,
    TD_PASTE_ALTV
};

// Tapdance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_BTN2_ESC] = ACTION_TAP_DANCE_DOUBLE(KC_BTN2, KC_ESC),
    [TD_PASTE_ALTV] = ACTION_TAP_DANCE_DOUBLE(C(KC_V), A(KC_V))
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        DRAG_OR_META, KC_BTN3, KC_BTN1,
          TD(TD_BTN2_ESC), TD(TD_PASTE_ALTV)
    ),
};

// Double tap detection variables
static uint16_t drag_timer = 0;
static bool drag_tap_registered = false;
#define DOUBLE_TAP_TIMEOUT 300

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_OR_META:
            if (record->event.pressed) {
                if (drag_tap_registered && timer_elapsed(drag_timer) < DOUBLE_TAP_TIMEOUT) {
                    // Double tap detected - send Meta+D instead of drag scroll
                    drag_tap_registered = false;
                    tap_code16(G(KC_D));
                } else {
                    // First tap - set timer and wait
                    drag_tap_registered = true;
                    drag_timer = timer_read();
                }
            } else {
                // Key release - check if it's a single tap
                if (drag_tap_registered) {
                    if (timer_elapsed(drag_timer) < DOUBLE_TAP_TIMEOUT) {
                        // Still within double tap window, wait for potential second tap
                        // Don't do anything yet
                    } else {
                        // Too long for double tap, must be single tap
                        drag_tap_registered = false;
                        // Directly call process_record_kb with DRAG_SCROLL
                        keyrecord_t drag_record = {};
                        drag_record.event.pressed = true;
                        process_record_kb(DRAG_SCROLL, &drag_record);
                    }
                }
            }
            return false;
    }
    return true;
}

// Matrix scan to handle single tap after timeout
void matrix_scan_user(void) {
    if (drag_tap_registered && timer_elapsed(drag_timer) >= DOUBLE_TAP_TIMEOUT) {
        // Timeout reached without second tap - execute single tap action
        drag_tap_registered = false;
        keyrecord_t drag_record = {};
        drag_record.event.pressed = true;
        process_record_kb(DRAG_SCROLL, &drag_record);
    }
}

// No longer needed - removed scroll speed multiplier functionality