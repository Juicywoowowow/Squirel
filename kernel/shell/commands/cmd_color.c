/**
 * @file cmd_color.c
 * @brief Color command implementation
 * 
 * Allows changing the text foreground and background colors.
 */

#include <shell/shell.h>
#include <lib/printf/printf.h>
#include <lib/string/string.h>
#include <drivers/vga/vga_text.h>

/**
 * @brief Color name to VGA color mapping
 */
static const struct {
    const char *name;
    vga_color_t color;
} color_names[] = {
    { "black",        VGA_BLACK },
    { "blue",         VGA_BLUE },
    { "green",        VGA_GREEN },
    { "cyan",         VGA_CYAN },
    { "red",          VGA_RED },
    { "magenta",      VGA_MAGENTA },
    { "brown",        VGA_BROWN },
    { "gray",         VGA_LIGHT_GRAY },
    { "darkgray",     VGA_DARK_GRAY },
    { "lightblue",    VGA_LIGHT_BLUE },
    { "lightgreen",   VGA_LIGHT_GREEN },
    { "lightcyan",    VGA_LIGHT_CYAN },
    { "lightred",     VGA_LIGHT_RED },
    { "lightmagenta", VGA_LIGHT_MAGENTA },
    { "yellow",       VGA_YELLOW },
    { "white",        VGA_WHITE },
    { NULL, 0 }
};

/**
 * @brief Parse a color string (name or number 0-15)
 */
static bool parse_color(const char *str, vga_color_t *out) {
    /* Try numeric value first */
    if (str[0] >= '0' && str[0] <= '9') {
        int val = 0;
        while (*str >= '0' && *str <= '9') {
            val = val * 10 + (*str - '0');
            str++;
        }
        if (*str == '\0' && val >= 0 && val <= 15) {
            *out = (vga_color_t)val;
            return true;
        }
        return false;
    }
    
    /* Try color name */
    for (int i = 0; color_names[i].name != NULL; i++) {
        if (strcmp(str, color_names[i].name) == 0) {
            *out = color_names[i].color;
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Color command handler
 * 
 * Usage:
 *   color                    - Show current colors and available options
 *   color <fg>               - Set foreground color
 *   color <fg> <bg>          - Set foreground and background colors
 * 
 * Colors can be specified by name or number (0-15).
 */
void cmd_color(int argc, char *argv[]) {
    if (argc == 1) {
        /* Show available colors */
        kprintf("\nUsage: color <fg> [bg]\n\n");
        kprintf("Available colors:\n");
        
        for (int i = 0; color_names[i].name != NULL; i++) {
            vga_set_color(color_names[i].color, VGA_BLACK);
            kprintf("  %2d: %-12s", color_names[i].color, color_names[i].name);
            if ((i + 1) % 4 == 0) {
                kprintf("\n");
            }
        }
        
        vga_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
        kprintf("\n\nExample: color yellow blue\n\n");
        return;
    }
    
    vga_color_t fg, bg;
    
    /* Parse foreground color */
    if (!parse_color(argv[1], &fg)) {
        kprintf("Error: Invalid color '%s'\n", argv[1]);
        return;
    }
    
    /* Parse optional background color */
    if (argc >= 3) {
        if (!parse_color(argv[2], &bg)) {
            kprintf("Error: Invalid color '%s'\n", argv[2]);
            return;
        }
    } else {
        bg = VGA_BLACK;  /* Default background */
    }
    
    vga_set_color(fg, bg);
    kprintf("Color set to %s on %s\n", argv[1], argc >= 3 ? argv[2] : "black");
}
