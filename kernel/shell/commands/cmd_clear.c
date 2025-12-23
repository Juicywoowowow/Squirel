/**
 * @file cmd_clear.c
 * @brief Clear command implementation
 * 
 * Clears the screen and resets cursor to top-left.
 */

#include <shell/shell.h>
#include <drivers/vga/vga_text.h>

/**
 * @brief Clear command handler
 * 
 * Usage:
 *   clear
 */
void cmd_clear(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    vga_clear();
}
