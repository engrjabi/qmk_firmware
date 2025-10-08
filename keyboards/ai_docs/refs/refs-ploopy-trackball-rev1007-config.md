# Conversation Reference: Ploopy Trackball Rev1_007 QMK Configuration

## Session Overview
This session focused on configuring and enhancing QMK firmware for a Ploopy trackball rev1_007. The work involved fixing drag scroll direction issues, implementing tapdance functionality, and creating a customized button layout. The conversation progressed from basic scroll direction fixes to implementing comprehensive tapdance features for enhanced productivity.

## Main Intents
- **Fix drag scroll direction**: Correct inverted scroll behavior when using VIA-assigned drag scroll
- **Implement tapdance functionality**: Add multi-function capabilities to trackball buttons
- **Optimize button layout**: Create a productive 5-button configuration with copy/paste, shortcuts, and drag scroll
- **Simplify code structure**: Move from complex tapdance implementations to simple ACTION_TAP_DANCE_DOUBLE

## Key Details & Decisions

### Hardware Configuration
- **Device**: Ploopy Classic Trackball rev1_007
- **Processor**: RP2040 (Raspberry Pi Pico microcontroller)
- **Sensor**: PMW3360 optical sensor
- **Features**: 5 buttons + 1 encoder wheel + RGB LED
- **Working Directory**: `/app/home/joshua/extrafiles/projects/qmk_ploopy/keyboards/ploopyco/trackball/rev1_007/`

### Drag Scroll Direction Fix
- **Problem**: Drag scroll was inverted (upward trackball movement scrolled down)
- **Initial Approach**: Tried modifying `POINTING_DEVICE_INVERT_Y` - caused issues with normal mouse movement
- **Solution**: Added `PLOOPY_DRAGSCROLL_INVERT` configuration
- **Additional Settings Added**:
  - `PLOOPY_DRAGSCROLL_MOMENTARY` (later removed for toggle behavior)
  - `PLOOPY_DRAGSCROLL_FIXED`
  - `PLOOPY_DRAGSCROLL_DPI 100`

### Tapdance Implementation
- **Enabled**: Added `TAP_DANCE_ENABLE = yes` to new `rules.mk` file
- **Evolution**: Started with complex `ACTION_TAP_DANCE_FN_ADVANCED` functions, simplified to `ACTION_TAP_DANCE_DOUBLE`
- **Compilation Issues**: Fixed DRAG_SCROLL keycode errors by using simpler tapdance approach

### Final Button Configuration
1. **First button (left)**: `TD(TD_DRAG_META)`
   - Single tap: Toggle drag scroll mode
   - Double tap: Meta+D (show desktop)

2. **Second button (middle)**: `SCROLL_SPEED`
   - Cycles through scroll wheel multipliers: 1x, 2x, 4x, 8x

3. **Third button (right)**: `KC_BTN1`
   - Standard left click (no tapdance - double click handled by OS)

4. **Fourth button (back)**: `TD(TD_BTN2_ESC)`
   - Single tap: Right click
   - Double tap: Esc key

5. **Fifth button (forward)**: `TD(TD_PASTE_ALTV)`
   - Single tap: Paste (Ctrl+V)
   - Double tap: Alt+V

### Files Modified/Created
- **`/keyboards/ploopyco/trackball/config.h`**: Added drag scroll configuration
- **`/keyboards/ploopyco/trackball/rev1_007/rules.mk`**: Created with tapdance enable
- **`/keyboards/ploopyco/trackball/keymaps/default/keymap.c`**: Completely rewritten with tapdance
- **User Credentials**: Joshua Ignacio <engr.jabi@gmail.com>

### Technical Learnings
- `DRAG_SCROLL` keycode cannot be used directly with `tap_code()` function
- `ACTION_TAP_DANCE_DOUBLE(kc1, kc2)` is much simpler than complex state management
- QMK's built-in tapdance functions are preferred over custom implementations
- Code went from ~150 lines to ~40 lines with simplification

### Configuration Settings Applied
```c
// Drag scroll settings in config.h
#define PLOOPY_DRAGSCROLL_INVERT
#define PLOOPY_DRAGSCROLL_FIXED  
#define PLOOPY_DRAGSCROLL_DPI 100

// Tapdance definitions in keymap.c
[TD_DRAG_META] = ACTION_TAP_DANCE_DOUBLE(DRAG_SCROLL, G(KC_D)),
[TD_BTN2_ESC] = ACTION_TAP_DANCE_DOUBLE(KC_BTN2, KC_ESC),
[TD_PASTE_ALTV] = ACTION_TAP_DANCE_DOUBLE(C(KC_V), A(KC_V))
```

## Suggested Usage
This reference provides complete context for:
- **Future Ploopy trackball configurations**: Understanding the drag scroll fix and tapdance setup
- **QMK tapdance implementations**: Example of progression from complex to simple approaches
- **Trackball button optimization**: Proven layout for productivity features
- **Compilation troubleshooting**: Solutions for DRAG_SCROLL keycode issues
- **Hardware-specific configurations**: Rev1_007 RP2040-based setup details

The final configuration creates a highly functional trackball with drag scroll, desktop shortcuts, and integrated copy/paste functionality while maintaining clean, maintainable code.