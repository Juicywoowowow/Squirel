/**
 * @file keyboard.c
 * @brief PS/2 keyboard driver implementation
 * 
 * Implements keyboard input using polling (not interrupts for now).
 * Uses Scancode Set 1 which is the default on most systems.
 * 
 * SCANCODE SET 1 (partial):
 *   Key        Make  Break
 *   A          0x1E  0x9E
 *   ...
 *   Enter      0x1C  0x9C
 *   Space      0x39  0xB9
 *   Backspace  0x0E  0x8E
 *   L-Shift    0x2A  0xAA
 *   R-Shift    0x36  0xB6
 *   L-Ctrl     0x1D  0x9D
 *   L-Alt      0x38  0xB8
 */

#include "keyboard.h"
#include <squirel/config.h>
#include <arch/x86_64/io/port.h>

/* ============================================================================
 * Scancode to ASCII Translation Table
 * ============================================================================ */

/**
 * @brief Scancode Set 1 to ASCII mapping (lowercase)
 * 
 * Index is the scancode, value is the ASCII character.
 * 0 means no printable character.
 */
static const char scancode_to_ascii_lower[128] = {
    0,    0x1B, '1',  '2',  '3',  '4',  '5',  '6',   /* 0x00-0x07 */
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',  /* 0x08-0x0F */
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',   /* 0x10-0x17 */
    'o',  'p',  '[',  ']',  '\n', 0,    'a',  's',   /* 0x18-0x1F */
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',   /* 0x20-0x27 */
    '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',   /* 0x28-0x2F */
    'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',   /* 0x30-0x37 */
    0,    ' ',  0,    0,    0,    0,    0,    0,     /* 0x38-0x3F */
    0,    0,    0,    0,    0,    0,    0,    '7',   /* 0x40-0x47 */
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',   /* 0x48-0x4F */
    '2',  '3',  '0',  '.',  0,    0,    0,    0,     /* 0x50-0x57 */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x58-0x5F */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x60-0x67 */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x68-0x6F */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x70-0x77 */
    0,    0,    0,    0,    0,    0,    0,    0      /* 0x78-0x7F */
};

/**
 * @brief Scancode Set 1 to ASCII mapping (uppercase/shifted)
 */
static const char scancode_to_ascii_upper[128] = {
    0,    0x1B, '!',  '@',  '#',  '$',  '%',  '^',   /* 0x00-0x07 */
    '&',  '*',  '(',  ')',  '_',  '+',  '\b', '\t',  /* 0x08-0x0F */
    'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',   /* 0x10-0x17 */
    'O',  'P',  '{',  '}',  '\n', 0,    'A',  'S',   /* 0x18-0x1F */
    'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',   /* 0x20-0x27 */
    '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',   /* 0x28-0x2F */
    'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',   /* 0x30-0x37 */
    0,    ' ',  0,    0,    0,    0,    0,    0,     /* 0x38-0x3F */
    0,    0,    0,    0,    0,    0,    0,    '7',   /* 0x40-0x47 */
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',   /* 0x48-0x4F */
    '2',  '3',  '0',  '.',  0,    0,    0,    0,     /* 0x50-0x57 */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x58-0x5F */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x60-0x67 */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x68-0x6F */
    0,    0,    0,    0,    0,    0,    0,    0,     /* 0x70-0x77 */
    0,    0,    0,    0,    0,    0,    0,    0      /* 0x78-0x7F */
};

/* ============================================================================
 * Private State
 * ============================================================================ */

/** @brief Current modifier key states */
static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;

/* ============================================================================
 * Private Helper Functions
 * ============================================================================ */

/**
 * @brief Wait for keyboard controller input buffer to be empty
 */
static void keyboard_wait_input(void) {
    while (inb(KEYBOARD_STATUS_PORT) & 0x02) {
        /* Busy wait */
    }
}

/**
 * @brief Wait for keyboard controller output buffer to have data
 */
static void keyboard_wait_output(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & 0x01)) {
        /* Busy wait */
    }
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

void keyboard_init(void) {
    /* Wait for any pending data */
    while (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);  /* Discard */
    }
    
    /* Enable keyboard */
    keyboard_wait_input();
    outb(KEYBOARD_STATUS_PORT, 0xAE);  /* Enable first PS/2 port */
    
    /* Enable keyboard scanning */
    keyboard_wait_input();
    outb(KEYBOARD_DATA_PORT, 0xF4);    /* Enable scanning */
    
    /* Wait for ACK */
    keyboard_wait_output();
    inb(KEYBOARD_DATA_PORT);  /* Should be 0xFA (ACK) */
}

bool keyboard_has_key(void) {
    return (inb(KEYBOARD_STATUS_PORT) & 0x01) != 0;
}

uint8_t keyboard_read_scancode(void) {
    if (!keyboard_has_key()) {
        return 0;
    }
    return inb(KEYBOARD_DATA_PORT);
}

int keyboard_getchar_nonblock(void) {
    uint8_t scancode = keyboard_read_scancode();
    if (scancode == 0) {
        return KEY_NONE;
    }
    
    /* Check for key release (bit 7 set) */
    bool released = (scancode & 0x80) != 0;
    scancode &= 0x7F;  /* Clear release bit */
    
    /* Handle modifier keys */
    switch (scancode) {
        case 0x2A:  /* Left Shift */
        case 0x36:  /* Right Shift */
            shift_pressed = !released;
            return KEY_NONE;
            
        case 0x1D:  /* Left Ctrl */
            ctrl_pressed = !released;
            return KEY_NONE;
            
        case 0x38:  /* Left Alt */
            alt_pressed = !released;
            return KEY_NONE;
    }
    
    /* Ignore key releases for other keys */
    if (released) {
        return KEY_NONE;
    }
    
    /* Handle special keys (arrow keys, function keys, etc.) */
    switch (scancode) {
        case 0x48: return KEY_UP;
        case 0x50: return KEY_DOWN;
        case 0x4B: return KEY_LEFT;
        case 0x4D: return KEY_RIGHT;
        case 0x47: return KEY_HOME;
        case 0x4F: return KEY_END;
        case 0x49: return KEY_PAGEUP;
        case 0x51: return KEY_PAGEDOWN;
        case 0x52: return KEY_INSERT;
        case 0x53: return KEY_DELETE;
        case 0x3B: return KEY_F1;
        case 0x3C: return KEY_F2;
        case 0x3D: return KEY_F3;
        case 0x3E: return KEY_F4;
        case 0x3F: return KEY_F5;
        case 0x40: return KEY_F6;
        case 0x41: return KEY_F7;
        case 0x42: return KEY_F8;
        case 0x43: return KEY_F9;
        case 0x44: return KEY_F10;
        case 0x57: return KEY_F11;
        case 0x58: return KEY_F12;
    }
    
    /* Translate scancode to ASCII */
    const char *table = shift_pressed ? 
                        scancode_to_ascii_upper : 
                        scancode_to_ascii_lower;
    
    char c = table[scancode];
    if (c == 0) {
        return KEY_NONE;
    }
    
    /* Handle Ctrl+key combinations */
    if (ctrl_pressed && c >= 'a' && c <= 'z') {
        return c - 'a' + 1;  /* Ctrl+A = 1, Ctrl+B = 2, etc. */
    }
    
    return c;
}

int keyboard_getchar(void) {
    int c;
    while ((c = keyboard_getchar_nonblock()) == KEY_NONE) {
        /* Busy wait - polling mode since interrupts are disabled */
        /* Note: In a real OS with interrupts enabled, we could use HLT */
        __asm__ volatile("pause");  /* CPU hint: we're in a spin loop */
    }
    return c;
}

bool keyboard_shift_pressed(void) {
    return shift_pressed;
}

bool keyboard_ctrl_pressed(void) {
    return ctrl_pressed;
}

bool keyboard_alt_pressed(void) {
    return alt_pressed;
}
