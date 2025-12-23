/**
 * @file cmd_help.c
 * @brief Help command implementation
 * 
 * Displays a list of all available shell commands and their descriptions.
 */

#include <shell/shell.h>
#include <lib/printf/printf.h>
#include <lib/string/string.h>

/**
 * @brief Command entry structure (mirrored from shell.c)
 */
typedef struct {
    const char *name;
    const char *help;
    void (*handler)(int argc, char *argv[]);
} shell_command_t;

/* Defined in shell.c */
extern int shell_get_commands(const shell_command_t **out_commands);

/**
 * @brief Help command handler
 * 
 * Usage:
 *   help          - List all commands
 *   help <cmd>    - Show help for specific command (future)
 */
void cmd_help(int argc, char *argv[]) {
    (void)argc;  /* Unused for now */
    (void)argv;
    
    const shell_command_t *commands;
    int num = shell_get_commands(&commands);
    
    kprintf("\nAvailable commands:\n");
    kprintf("-------------------\n");
    
    for (int i = 0; i < num; i++) {
        kprintf("  %-10s - %s\n", commands[i].name, commands[i].help);
    }
    
    kprintf("\n");
}
