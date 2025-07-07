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
    SMART_LEFT_CLICK = SAFE_RANGE  // Left click that disables drag scroll if active
};

// Scroll mode state
enum scroll_modes {
    SCROLL_NORMAL
};

static uint8_t current_scroll_mode = SCROLL_NORMAL;

// Tapdance declarations
enum {
    TD_META_RIGHTCLICK,
    TD_PASTE_ALTV
};

// Tapdance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_META_RIGHTCLICK] = ACTION_TAP_DANCE_DOUBLE(G(KC_D), KC_BTN2),
    [TD_PASTE_ALTV] = ACTION_TAP_DANCE_DOUBLE(C(KC_V), A(KC_V))
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        TD(TD_META_RIGHTCLICK), DRAG_SCROLL, SMART_LEFT_CLICK,
          C(S(KC_TAB)), C(KC_TAB)
    ),
};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL:
            {
                extern bool is_drag_scroll;
                if (record->event.pressed) {
                    if (is_drag_scroll) {
                        // When turning off drag scroll, reset everything to normal
                        toggle_drag_scroll();
                        current_scroll_mode = SCROLL_NORMAL;
                    } else {
                        // When turning on drag scroll, just toggle it
                        toggle_drag_scroll();
                    }
                }
            }
            return false;
        case SMART_LEFT_CLICK:
            {
                extern bool is_drag_scroll;
                if (record->event.pressed) {
                    // Reset drag scroll mode with one click
                    if (is_drag_scroll) {
                        toggle_drag_scroll();
                        current_scroll_mode = SCROLL_NORMAL;
                        return false; // Don't perform left click when resetting modes
                    }
                }
                // For normal left click behavior (both press and release), 
                // register/unregister the button to allow drag operations
                if (record->event.pressed) {
                    register_code(KC_BTN1);
                } else {
                    unregister_code(KC_BTN1);
                }
            }
            return false;
    }
    return true;
}


// Handle physical scroll wheel events
bool encoder_update_user(uint8_t index, bool clockwise) {
    return true; // Allow default scroll behavior
}

// Normal scroll processing
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return mouse_report;
}

