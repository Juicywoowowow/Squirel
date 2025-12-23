/**
 * @file cmd_echo.c
 * @brief Echo command implementation
 * 
 * Prints its arguments to the screen.
 */

#include <shell/shell.h>
#include <lib/printf/printf.h>

/**
 * @brief Echo command handler
 * 
 * Usage:
 *   echo <text>...
 * 
 * Prints all arguments separated by spaces, followed by a newline.
 */
void cmd_echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        kprintf("%s", argv[i]);
        if (i < argc - 1) {
            kprintf(" ");
        }
    }
    kprintf("\n");
}
