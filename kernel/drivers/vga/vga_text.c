/**
 * @file vga_text.c
 * @brief VGA text mode driver implementation
 * 
 * Implements VGA text mode output by directly writing to the VGA buffer
 * at 0xB8000. This is memory-mapped I/O - writing to these addresses
 * directly updates the screen.
 * 
 * IMPLEMENTATION NOTES:
 *   - The VGA buffer is a 2D array of 16-bit entries
 *   - We maintain cursor position in software
 *   - Hardware cursor is updated via VGA CRT controller ports
 *   - Scrolling copies memory and clears the bottom line
 */

#include "vga_text.h"
#include <squirel/config.h>
#include <arch/x86_64/io/port.h>

/* ============================================================================
 * Private State
 * ============================================================================ */

/** @brief Pointer to VGA text buffer (memory-mapped I/O) */
static uint16_t *vga_buffer = (uint16_t *)VGA_BUFFER_ADDR;

/** @brief Current cursor column (0-79) */
static int cursor_x = 0;

/** @brief Current cursor row (0-24) */
static int cursor_y = 0;

/** @brief Current attribute byte (color) */
static uint8_t current_attr = 0x07;  /* Light gray on black */

/* ============================================================================
 * Private Helper Functions
 * ============================================================================ */

/**
 * @brief Create a VGA attribute byte from foreground and background colors
 * 
 * @param fg  Foreground color (0-15)
 * @param bg  Background color (0-15)
 * @return    Combined attribute byte
 */
static uint8_t vga_make_attr(vga_color_t fg, vga_color_t bg) {
    return (uint8_t)((bg << 4) | (fg & 0x0F));
}

/**
 * @brief Create a VGA entry (character + attribute)
 * 
 * @param c     ASCII character
 * @param attr  Attribute byte
 * @return      16-bit VGA entry
 */
static uint16_t vga_make_entry(char c, uint8_t attr) {
    return (uint16_t)c | ((uint16_t)attr << 8);
}

/**
 * @brief Update the hardware cursor position
 * 
 * The VGA CRT controller maintains a hardware cursor separate from
 * our software cursor. We update it to match.
 * 
 * CRT Controller ports:
 *   0x3D4 - Index register (select which CRT register to access)
 *   0x3D5 - Data register (read/write the selected register)
 * 
 * Cursor registers:
 *   0x0E - Cursor location high byte
 *   0x0F - Cursor location low byte
 */
static void vga_update_cursor(void) {
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    
    outb(0x3D4, 0x0F);           /* Select cursor low register */
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    
    outb(0x3D4, 0x0E);           /* Select cursor high register */
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

void vga_init(void) {
    /* Set default colors: light gray on black */
    current_attr = vga_make_attr(VGA_LIGHT_GRAY, VGA_BLACK);
    
    /* Clear the screen */
    vga_clear();
    
    /* Enable hardware cursor */
    vga_cursor_enable(true);
}

void vga_clear(void) {
    uint16_t blank = vga_make_entry(' ', current_attr);
    
    /* Fill entire buffer with blank spaces */
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
    
    /* Reset cursor to top-left */
    cursor_x = 0;
    cursor_y = 0;
    vga_update_cursor();
}

void vga_set_color(vga_color_t fg, vga_color_t bg) {
    current_attr = vga_make_attr(fg, bg);
}

void vga_putchar(char c) {
    switch (c) {
        case '\n':
            /* Newline: move to start of next line */
            cursor_x = 0;
            cursor_y++;
            break;
            
        case '\r':
            /* Carriage return: move to start of current line */
            cursor_x = 0;
            break;
            
        case '\t':
            /* Tab: move to next 8-column boundary */
            cursor_x = (cursor_x + 8) & ~7;
            if (cursor_x >= VGA_WIDTH) {
                cursor_x = 0;
                cursor_y++;
            }
            break;
            
        case '\b':
            /* Backspace: move back one position */
            if (cursor_x > 0) {
                cursor_x--;
            } else if (cursor_y > 0) {
                cursor_y--;
                cursor_x = VGA_WIDTH - 1;
            }
            break;
            
        default:
            /* Regular character: write to buffer */
            if (c >= ' ') {  /* Printable characters only */
                int offset = cursor_y * VGA_WIDTH + cursor_x;
                vga_buffer[offset] = vga_make_entry(c, current_attr);
                cursor_x++;
                
                /* Wrap at end of line */
                if (cursor_x >= VGA_WIDTH) {
                    cursor_x = 0;
                    cursor_y++;
                }
            }
            break;
    }
    
    /* Scroll if necessary */
    if (cursor_y >= VGA_HEIGHT) {
        vga_scroll();
        cursor_y = VGA_HEIGHT - 1;
    }
    
    vga_update_cursor();
}

void vga_print(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

void vga_println(const char *str) {
    vga_print(str);
    vga_putchar('\n');
}

void vga_set_cursor(int x, int y) {
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        cursor_x = x;
        cursor_y = y;
        vga_update_cursor();
    }
}

int vga_get_cursor_x(void) {
    return cursor_x;
}

int vga_get_cursor_y(void) {
    return cursor_y;
}

void vga_cursor_enable(bool enable) {
    if (enable) {
        /* 
         * Set cursor shape: start scanline 14, end scanline 15
         * This creates a underline cursor
         */
        outb(0x3D4, 0x0A);           /* Cursor start register */
        outb(0x3D5, 0x0E);           /* Start scanline 14 */
        
        outb(0x3D4, 0x0B);           /* Cursor end register */
        outb(0x3D5, 0x0F);           /* End scanline 15 */
    } else {
        /* Disable cursor by setting bit 5 of start register */
        outb(0x3D4, 0x0A);
        outb(0x3D5, 0x20);           /* Bit 5 = cursor disable */
    }
}

void vga_scroll(void) {
    uint16_t blank = vga_make_entry(' ', current_attr);
    
    /* Move everything up one line */
    for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
    }
    
    /* Clear the last line */
    for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = blank;
    }
}
