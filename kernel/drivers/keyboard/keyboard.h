/**
 * @file keyboard.h
 * @brief PS/2 keyboard driver interface
 * 
 * Provides functions to read keyboard input using PS/2 protocol.
 * The keyboard controller is accessed via I/O ports 0x60 and 0x64.
 * 
 * PS/2 KEYBOARD OVERVIEW:
 *   Port 0x60 - Data port (read scancodes, write commands to keyboard)
 *   Port 0x64 - Status/Command port
 *   
 *   Status register bits (read from 0x64):
 *     Bit 0: Output buffer full (data ready to read from 0x60)
 *     Bit 1: Input buffer full (don't write to 0x60/0x64 yet)
 *   
 * SCANCODE SETS:
 *   We use Scancode Set 1 which is the default on most systems.
 *   Each key has a "make" code (press) and "break" code (release).
 *   Break codes are make codes with bit 7 set (OR 0x80).
 */

#ifndef _DRIVERS_KEYBOARD_H
#define _DRIVERS_KEYBOARD_H

#include <squirel/types.h>

/* ============================================================================
 * Special Key Codes
 * ============================================================================ */

/** @brief No key pressed */
#define KEY_NONE        0

/** @brief Special keys (returned by keyboard_getchar) */
#define KEY_ESCAPE      0x1B
#define KEY_BACKSPACE   0x08
#define KEY_TAB         0x09
#define KEY_ENTER       0x0A

/** @brief Keys that don't produce printable characters (high byte set) */
#define KEY_SPECIAL     0x100

#define KEY_F1          (KEY_SPECIAL | 0x3B)
#define KEY_F2          (KEY_SPECIAL | 0x3C)
#define KEY_F3          (KEY_SPECIAL | 0x3D)
#define KEY_F4          (KEY_SPECIAL | 0x3E)
#define KEY_F5          (KEY_SPECIAL | 0x3F)
#define KEY_F6          (KEY_SPECIAL | 0x40)
#define KEY_F7          (KEY_SPECIAL | 0x41)
#define KEY_F8          (KEY_SPECIAL | 0x42)
#define KEY_F9          (KEY_SPECIAL | 0x43)
#define KEY_F10         (KEY_SPECIAL | 0x44)
#define KEY_F11         (KEY_SPECIAL | 0x57)
#define KEY_F12         (KEY_SPECIAL | 0x58)

#define KEY_UP          (KEY_SPECIAL | 0x48)
#define KEY_DOWN        (KEY_SPECIAL | 0x50)
#define KEY_LEFT        (KEY_SPECIAL | 0x4B)
#define KEY_RIGHT       (KEY_SPECIAL | 0x4D)

#define KEY_HOME        (KEY_SPECIAL | 0x47)
#define KEY_END         (KEY_SPECIAL | 0x4F)
#define KEY_PAGEUP      (KEY_SPECIAL | 0x49)
#define KEY_PAGEDOWN    (KEY_SPECIAL | 0x51)
#define KEY_INSERT      (KEY_SPECIAL | 0x52)
#define KEY_DELETE      (KEY_SPECIAL | 0x53)

/* ============================================================================
 * Keyboard Functions
 * ============================================================================ */

/**
 * @brief Initialize the keyboard driver
 * 
 * Sets up the keyboard controller and enables keyboard.
 * Must be called before any other keyboard functions.
 */
void keyboard_init(void);

/**
 * @brief Check if a key is available
 * 
 * @return true if a key is waiting to be read, false otherwise
 */
bool keyboard_has_key(void);

/**
 * @brief Read a key (blocking)
 * 
 * Waits for a key press and returns the ASCII character.
 * For non-printable keys, returns a KEY_* constant.
 * 
 * @return ASCII character or KEY_* constant
 */
int keyboard_getchar(void);

/**
 * @brief Read a key (non-blocking)
 * 
 * @return ASCII character, KEY_* constant, or KEY_NONE if no key
 */
int keyboard_getchar_nonblock(void);

/**
 * @brief Read a raw scancode (non-blocking)
 * 
 * @return Raw scancode, or 0 if no key available
 */
uint8_t keyboard_read_scancode(void);

/**
 * @brief Check if Shift is currently pressed
 */
bool keyboard_shift_pressed(void);

/**
 * @brief Check if Ctrl is currently pressed
 */
bool keyboard_ctrl_pressed(void);

/**
 * @brief Check if Alt is currently pressed
 */
bool keyboard_alt_pressed(void);

#endif /* _DRIVERS_KEYBOARD_H */
