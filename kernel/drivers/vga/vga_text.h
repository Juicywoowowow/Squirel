/**
 * @file vga_text.h
 * @brief VGA text mode driver interface
 * 
 * Provides functions to write text to the screen using VGA text mode.
 * VGA text mode is the simplest way to display text on x86 systems.
 * 
 * VGA TEXT MODE OVERVIEW:
 *   Resolution: 80 columns x 25 rows = 2000 characters
 *   Colors: 16 foreground + 16 background colors
 *   Buffer: Memory-mapped at 0xB8000
 * 
 * MEMORY LAYOUT:
 *   Each character cell is 2 bytes:
 *     Byte 0: ASCII character code (0-255)
 *     Byte 1: Attribute byte
 *   
 *   Attribute byte format:
 *     Bit 7: Blink (or bright background, depending on mode)
 *     Bits 6-4: Background color (0-7)
 *     Bit 3: Foreground intensity
 *     Bits 2-0: Foreground color (0-7)
 * 
 *   Total buffer size: 80 * 25 * 2 = 4000 bytes
 */

#ifndef _DRIVERS_VGA_TEXT_H
#define _DRIVERS_VGA_TEXT_H

#include <squirel/types.h>

/* ============================================================================
 * VGA Color Definitions
 * ============================================================================ */

/** @brief VGA color palette (16 colors) */
typedef enum {
    VGA_BLACK         = 0,
    VGA_BLUE          = 1,
    VGA_GREEN         = 2,
    VGA_CYAN          = 3,
    VGA_RED           = 4,
    VGA_MAGENTA       = 5,
    VGA_BROWN         = 6,
    VGA_LIGHT_GRAY    = 7,
    VGA_DARK_GRAY     = 8,
    VGA_LIGHT_BLUE    = 9,
    VGA_LIGHT_GREEN   = 10,
    VGA_LIGHT_CYAN    = 11,
    VGA_LIGHT_RED     = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW        = 14,
    VGA_WHITE         = 15
} vga_color_t;

/* ============================================================================
 * VGA Functions
 * ============================================================================ */

/**
 * @brief Initialize the VGA text mode driver
 * 
 * Clears the screen, sets default colors (light gray on black),
 * and positions the cursor at (0, 0).
 * 
 * @note Must be called before any other vga_* functions
 */
void vga_init(void);

/**
 * @brief Clear the screen
 * 
 * Fills the entire screen with spaces using the current color.
 * Resets cursor position to (0, 0).
 */
void vga_clear(void);

/**
 * @brief Set the text colors
 * 
 * @param fg  Foreground (text) color (0-15)
 * @param bg  Background color (0-7, or 0-15 if blink disabled)
 * 
 * @example vga_set_color(VGA_WHITE, VGA_BLUE);  // White on blue
 */
void vga_set_color(vga_color_t fg, vga_color_t bg);

/**
 * @brief Print a single character at the current cursor position
 * 
 * @param c  The character to print
 * 
 * Handles special characters:
 *   '\n' - Newline (moves to start of next line)
 *   '\r' - Carriage return (moves to start of current line)
 *   '\t' - Tab (moves to next 8-column boundary)
 *   '\b' - Backspace (moves cursor back one position)
 */
void vga_putchar(char c);

/**
 * @brief Print a null-terminated string
 * 
 * @param str  The string to print
 */
void vga_print(const char *str);

/**
 * @brief Print a string with a newline
 * 
 * @param str  The string to print
 */
void vga_println(const char *str);

/**
 * @brief Set the cursor position
 * 
 * @param x  Column (0-79)
 * @param y  Row (0-24)
 */
void vga_set_cursor(int x, int y);

/**
 * @brief Get the current cursor X position
 * @return Current column (0-79)
 */
int vga_get_cursor_x(void);

/**
 * @brief Get the current cursor Y position
 * @return Current row (0-24)
 */
int vga_get_cursor_y(void);

/**
 * @brief Enable or disable the hardware cursor
 * 
 * @param enable  true to show cursor, false to hide
 */
void vga_cursor_enable(bool enable);

/**
 * @brief Scroll the screen up by one line
 * 
 * Moves all lines up by one, clears the bottom line.
 * Called automatically when printing past the last row.
 */
void vga_scroll(void);

#endif /* _DRIVERS_VGA_TEXT_H */
