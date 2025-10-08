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

// Define tapping term if not already defined
#ifndef TAPPING_TERM
#define TAPPING_TERM 200
#endif

// Custom keycodes
enum custom_keycodes {
    SMART_LEFT_CLICK = SAFE_RANGE,  // Left click that disables drag scroll if active
    CLICK_PREV_TAB,                 // Left click + Ctrl+Shift+Tab
    CLICK_NEXT_TAB                  // Left click + Ctrl+Tab
};

// Scroll mode state
enum scroll_modes {
    SCROLL_NORMAL
};

static uint8_t current_scroll_mode = SCROLL_NORMAL;

// Tab navigation hold states
static bool click_prev_tab_held = false;
static bool click_next_tab_held = false;
static uint16_t tab_hold_timer = 0;
static bool initial_click_sent = false;
static uint16_t last_tab_send = 0;

// Tab session tracking
static bool tab_session_active = false;
static uint16_t last_tab_activity = 0;

#define TAB_INITIAL_DELAY 50   // Initial delay after click (ms)
#define TAB_REPEAT_DELAY 300   // Delay between repeated tabs (ms)
#define TAB_SESSION_TIMEOUT 1000  // Reset session after 1 second of inactivity

// Double tap detection for first button
static uint16_t first_button_timer = 0;
static uint8_t first_button_tap_count = 0;
#define DOUBLE_TAP_TERM 300   // Max time between taps for double tap (ms)

// Tapdance declarations
enum {
    TD_PASTE_ALTV
};

// Tapdance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_PASTE_ALTV] = ACTION_TAP_DANCE_DOUBLE(C(KC_V), A(KC_V))
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( /* Base */
        LT(0, KC_NO), DRAG_SCROLL, SMART_LEFT_CLICK,
          CLICK_PREV_TAB, CLICK_NEXT_TAB
    ),
};


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT(0, KC_NO):
            if (record->event.pressed) {
                if (record->tap.count > 0) {
                    // Handle taps
                    if (timer_elapsed(first_button_timer) < DOUBLE_TAP_TERM && first_button_tap_count == 1) {
                        // Double tap detected - send Alt+Tab
                        tap_code16(A(KC_TAB));
                        first_button_tap_count = 0;  // Reset after double tap
                    } else {
                        // First tap or tap after timeout
                        first_button_tap_count = 1;
                        first_button_timer = timer_read();
                        // Don't send Meta+D yet, wait to see if it's a double tap
                    }
                } else {
                    // Hold: send right click press
                    register_code(KC_BTN2);
                    first_button_tap_count = 0;  // Reset tap count on hold
                }
            } else {
                if (!record->tap.count) {
                    // Release hold: release right click
                    unregister_code(KC_BTN2);
                }
                // Single tap is handled in matrix_scan_user
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
                        // When turning on drag scroll, release any held mouse buttons to prevent interference
                        unregister_code(KC_BTN1);
                        unregister_code(KC_BTN2);
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
        case CLICK_PREV_TAB:
            if (record->event.pressed) {
                // Check if this is the first tap in a session
                if (!tab_session_active) {
                    tab_session_active = true;
                }
                // Set up for hold
                click_prev_tab_held = true;
                tab_hold_timer = timer_read();
                initial_click_sent = true;
                last_tab_send = 0;
                last_tab_activity = timer_read();
            } else {
                // Release: stop holding
                click_prev_tab_held = false;
                initial_click_sent = false;
                last_tab_activity = timer_read();
            }
            return false;
        case CLICK_NEXT_TAB:
            if (record->event.pressed) {
                // Check if this is the first tap in a session
                if (!tab_session_active) {
                    tab_session_active = true;
                }
                // Set up for hold
                click_next_tab_held = true;
                tab_hold_timer = timer_read();
                initial_click_sent = true;
                last_tab_send = 0;
                last_tab_activity = timer_read();
            } else {
                // Release: stop holding
                click_next_tab_held = false;
                initial_click_sent = false;
                last_tab_activity = timer_read();
            }
            return false;
    }
    return true;
}


// Handle physical scroll wheel events
bool encoder_update_user(uint8_t index, bool clockwise) {
    return true; // Allow default scroll behavior
}

// Matrix scan for continuous tab sending and single tap timeout
void matrix_scan_user(void) {
    // Check for single tap timeout on first button
    if (first_button_tap_count == 1 && timer_elapsed(first_button_timer) >= DOUBLE_TAP_TERM) {
        // Single tap timeout - send Meta+D
        tap_code16(G(KC_D));
        first_button_tap_count = 0;
    }
    
    // Check for tab session timeout
    if (tab_session_active && timer_elapsed(last_tab_activity) >= TAB_SESSION_TIMEOUT) {
        tab_session_active = false;
    }
    
    // Handle tab key repeating
    if (click_prev_tab_held || click_next_tab_held) {
        uint16_t elapsed = timer_elapsed(tab_hold_timer);
        
        // First tab after initial delay
        if (initial_click_sent && elapsed >= TAB_INITIAL_DELAY && last_tab_send == 0) {
            if (click_prev_tab_held) {
                tap_code16(G(S(KC_TAB)));
            } else if (click_next_tab_held) {
                tap_code16(G(KC_TAB));
            }
            last_tab_send = elapsed;
        }
        // Subsequent tabs at repeat rate
        else if (last_tab_send > 0 && (elapsed - last_tab_send) >= TAB_REPEAT_DELAY) {
            if (click_prev_tab_held) {
                tap_code16(G(S(KC_TAB)));
            } else if (click_next_tab_held) {
                tap_code16(G(KC_TAB));
            }
            last_tab_send = elapsed;
        }
    }
}

// Pass through scroll processing
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return mouse_report;
}

