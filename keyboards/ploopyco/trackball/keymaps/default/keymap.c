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
    ALT_TAB_OR_META = SAFE_RANGE,  // This will handle alt+tab and Meta+D
    SMART_LEFT_CLICK,              // Left click that disables drag scroll if active
    CYCLE_SCROLL_MODE,             // Cycle through scroll modes
    VOLUME_SCROLL_MODE,            // Toggle volume scroll mode
    TAB_SCROLL_MODE                // Toggle tab scroll mode
};

// Scroll mode state
enum scroll_modes {
    SCROLL_NORMAL,
    SCROLL_VOLUME,
    SCROLL_TAB
};

static uint8_t current_scroll_mode = SCROLL_NORMAL;

// Scroll speed control variables
static uint16_t last_scroll_time = 0;
#define TAB_SCROLL_DEBOUNCE 150  // Slower scrolling for tab mode (vs default 5ms)
#define VOLUME_SCROLL_DEBOUNCE 50  // Medium speed for volume
#define NORMAL_SCROLL_DEBOUNCE 5   // Fast scrolling for normal mode

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
        ALT_TAB_OR_META, DRAG_SCROLL, SMART_LEFT_CLICK,
          TD(TD_BTN2_ESC), CYCLE_SCROLL_MODE
    ),
};


// Scroll mode functions
void cycle_scroll_mode(void) {
    current_scroll_mode = (current_scroll_mode + 1) % 3;
}

void set_scroll_mode(uint8_t mode) {
    current_scroll_mode = mode;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CYCLE_SCROLL_MODE:
            if (record->event.pressed) {
                cycle_scroll_mode();
            }
            return false;
        case VOLUME_SCROLL_MODE:
            if (record->event.pressed) {
                set_scroll_mode(SCROLL_VOLUME);
            }
            return false;
        case TAB_SCROLL_MODE:
            if (record->event.pressed) {
                set_scroll_mode(SCROLL_TAB);
            }
            return false;
        case ALT_TAB_OR_META:
            if (record->event.pressed) {
                tap_code16(G(KC_D));
            }
            return false;
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
                    // Reset both drag scroll and custom scroll modes with one click
                    if (is_drag_scroll || current_scroll_mode != SCROLL_NORMAL) {
                        if (is_drag_scroll) {
                            toggle_drag_scroll();
                        }
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


// Handle physical scroll wheel events with speed control
bool encoder_update_user(uint8_t index, bool clockwise) {
    uint16_t debounce_time;
    
    switch (current_scroll_mode) {
        case SCROLL_VOLUME:
            debounce_time = VOLUME_SCROLL_DEBOUNCE;
            if (timer_elapsed(last_scroll_time) < debounce_time) {
                return false; // Too soon, ignore this scroll event
            }
            last_scroll_time = timer_read();
            if (clockwise) {
                tap_code(KC_VOLU);
            } else {
                tap_code(KC_VOLD);
            }
            return false; // Prevent default scroll behavior
            
        case SCROLL_TAB:
            debounce_time = TAB_SCROLL_DEBOUNCE;
            if (timer_elapsed(last_scroll_time) < debounce_time) {
                return false; // Too soon, ignore this scroll event
            }
            last_scroll_time = timer_read();
            if (clockwise) {
                tap_code16(C(KC_TAB));
            } else {
                tap_code16(C(S(KC_TAB)));
            }
            return false; // Prevent default scroll behavior
            
        case SCROLL_NORMAL:
        default:
            return true; // Allow default scroll behavior
    }
}

// Enhanced scroll mode processing for drag scroll
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Only process if we have scroll data and we're not in normal mode
    if ((mouse_report.v != 0 || mouse_report.h != 0) && current_scroll_mode != SCROLL_NORMAL) {
        
        // Handle vertical scroll
        if (mouse_report.v != 0) {
            switch (current_scroll_mode) {
                case SCROLL_VOLUME:
                    if (mouse_report.v > 0) {
                        tap_code(KC_VOLU);
                    } else {
                        tap_code(KC_VOLD);
                    }
                    break;
                case SCROLL_TAB:
                    if (mouse_report.v > 0) {
                        tap_code16(C(KC_TAB));
                    } else {
                        tap_code16(C(S(KC_TAB)));
                    }
                    break;
            }
            // Clear scroll values after processing
            mouse_report.v = 0;
        }
        
        // Handle horizontal scroll (optional - could be used for additional functions)
        if (mouse_report.h != 0) {
            switch (current_scroll_mode) {
                case SCROLL_VOLUME:
                    // Horizontal scroll also controls volume
                    if (mouse_report.h > 0) {
                        tap_code(KC_VOLU);
                    } else {
                        tap_code(KC_VOLD);
                    }
                    break;
                case SCROLL_TAB:
                    // Horizontal scroll also controls tab switching
                    if (mouse_report.h > 0) {
                        tap_code16(C(KC_TAB));
                    } else {
                        tap_code16(C(S(KC_TAB)));
                    }
                    break;
            }
            // Clear scroll values after processing
            mouse_report.h = 0;
        }
    }
    
    return mouse_report;
}

// No longer needed - removed scroll speed multiplier functionality