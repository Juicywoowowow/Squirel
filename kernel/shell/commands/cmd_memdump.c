/**
 * @file cmd_memdump.c
 * @brief Memory dump command implementation
 * 
 * Displays memory contents at a specified address in hex format.
 */

#include <shell/shell.h>
#include <lib/printf/printf.h>
#include <lib/string/string.h>
#include <drivers/vga/vga_text.h>

/**
 * @brief Parse a hex string to uint64_t
 */
static bool parse_hex(const char *str, uint64_t *out) {
    uint64_t val = 0;
    
    /* Skip optional 0x prefix */
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }
    
    if (*str == '\0') {
        return false;
    }
    
    while (*str) {
        char c = *str++;
        uint64_t digit;
        
        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        } else {
            return false;
        }
        
        val = (val << 4) | digit;
    }
    
    *out = val;
    return true;
}

/**
 * @brief Parse a decimal string to uint64_t
 */
static bool parse_dec(const char *str, uint64_t *out) {
    uint64_t val = 0;
    
    if (*str == '\0') {
        return false;
    }
    
    while (*str) {
        char c = *str++;
        if (c >= '0' && c <= '9') {
            val = val * 10 + (c - '0');
        } else {
            return false;
        }
    }
    
    *out = val;
    return true;
}

/**
 * @brief Check if character is printable ASCII
 */
static char to_printable(uint8_t c) {
    return (c >= 0x20 && c <= 0x7E) ? (char)c : '.';
}

/**
 * @brief Memory dump command handler
 * 
 * Usage:
 *   memdump <address> [length]
 * 
 * Examples:
 *   memdump 0xB8000          - Dump 256 bytes starting at VGA buffer
 *   memdump 0x100000 512     - Dump 512 bytes at 1MB mark
 */
void cmd_memdump(int argc, char *argv[]) {
    if (argc < 2) {
        kprintf("Usage: memdump <address> [length]\n");
        kprintf("\nExamples:\n");
        kprintf("  memdump 0xB8000       - VGA text buffer\n");
        kprintf("  memdump 0x7C00 512    - MBR location\n");
        kprintf("  memdump 0x100000      - 1MB mark\n");
        return;
    }
    
    uint64_t address;
    uint64_t length = 256;  /* Default 256 bytes */
    
    /* Parse address */
    if (!parse_hex(argv[1], &address)) {
        kprintf("Error: Invalid address '%s'\n", argv[1]);
        return;
    }
    
    /* Parse optional length */
    if (argc >= 3) {
        if (!parse_dec(argv[2], &length) && !parse_hex(argv[2], &length)) {
            kprintf("Error: Invalid length '%s'\n", argv[2]);
            return;
        }
    }
    
    /* Limit length to prevent endless output */
    if (length > 4096) {
        kprintf("Warning: Limiting dump to 4096 bytes\n");
        length = 4096;
    }
    
    kprintf("\nMemory dump at 0x%llX (%llu bytes):\n\n", address, length);
    
    /* Print hex dump */
    uint8_t *ptr = (uint8_t *)address;
    
    for (uint64_t i = 0; i < length; i += 16) {
        /* Print address */
        vga_set_color(VGA_DARK_GRAY, VGA_BLACK);
        kprintf("%08llX: ", address + i);
        
        /* Print hex bytes */
        vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
        for (int j = 0; j < 16; j++) {
            if (i + j < length) {
                kprintf("%02X ", ptr[i + j]);
            } else {
                kprintf("   ");
            }
            if (j == 7) kprintf(" ");  /* Extra space in middle */
        }
        
        /* Print ASCII */
        vga_set_color(VGA_YELLOW, VGA_BLACK);
        kprintf(" |");
        for (int j = 0; j < 16 && i + j < length; j++) {
            kprintf("%c", to_printable(ptr[i + j]));
        }
        kprintf("|\n");
    }
    
    vga_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
    kprintf("\n");
}
