/**
 * @file shell.c
 * @brief Basic shell implementation
 * 
 * The heart of user interaction with Squirel OS.
 * Provides a command prompt, line editing, and command execution.
 * 
 * LINE EDITING FEATURES:
 *   - Backspace: Delete character before cursor
 *   - Enter: Execute command
 *   - Ctrl+C: Cancel current line (future)
 *   - Ctrl+L: Clear screen (future)
 */

#include "shell.h"
#include "parser/parser.h"
#include <squirel/config.h>
#include <drivers/vga/vga_text.h>
#include <drivers/keyboard/keyboard.h>
#include <lib/string/string.h>
#include <lib/printf/printf.h>
#include <lib/memory/memory.h>

/* ============================================================================
 * Command Table
 * ============================================================================ */

/** @brief Maximum number of registered commands */
#define MAX_COMMANDS 32

/**
 * @brief Shell command entry
 */
typedef struct {
    const char *name;           /**< Command name */
    const char *help;           /**< Help text */
    shell_cmd_fn handler;       /**< Command handler function */
} shell_command_t;

/** @brief Registered commands */
static shell_command_t commands[MAX_COMMANDS];
static int num_commands = 0;

/* ============================================================================
 * Forward Declarations for Built-in Commands
 * ============================================================================ */

extern void cmd_help(int argc, char *argv[]);
extern void cmd_clear(int argc, char *argv[]);
extern void cmd_echo(int argc, char *argv[]);
extern void cmd_info(int argc, char *argv[]);

/* ============================================================================
 * Private Functions
 * ============================================================================ */

/**
 * @brief Read a line of input from the keyboard
 * 
 * @param buffer  Buffer to store the line
 * @param maxlen  Maximum length of buffer
 * @return        Number of characters read
 * 
 * Handles:
 *   - Regular character input with echo
 *   - Backspace (deletes previous character)
 *   - Enter (ends input)
 */
static int shell_readline(char *buffer, size_t maxlen) {
    size_t pos = 0;
    
    while (pos < maxlen - 1) {
        int c = keyboard_getchar();
        
        if (c == '\n' || c == KEY_ENTER) {
            /* End of line */
            buffer[pos] = '\0';
            vga_putchar('\n');
            return pos;
        } else if (c == '\b' || c == KEY_BACKSPACE) {
            /* Backspace */
            if (pos > 0) {
                pos--;
                /* Move cursor back, print space, move back again */
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            }
        } else if (c >= ' ' && c <= '~') {
            /* Printable ASCII character */
            buffer[pos++] = (char)c;
            vga_putchar((char)c);
        }
        /* Ignore other characters (arrow keys, etc. for now) */
    }
    
    buffer[pos] = '\0';
    return pos;
}

/**
 * @brief Find a command by name
 * 
 * @param name  Command name to find
 * @return      Pointer to command entry, or NULL if not found
 */
static shell_command_t *shell_find_command(const char *name) {
    for (int i = 0; i < num_commands; i++) {
        if (strcmp(commands[i].name, name) == 0) {
            return &commands[i];
        }
    }
    return NULL;
}

/**
 * @brief Initialize built-in commands
 */
static void shell_init_commands(void) {
    shell_register_command("help",  "Display available commands",    cmd_help);
    shell_register_command("clear", "Clear the screen",              cmd_clear);
    shell_register_command("echo",  "Print text to screen",          cmd_echo);
    shell_register_command("info",  "Display system information",    cmd_info);
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

void shell_register_command(const char *name, const char *help, shell_cmd_fn handler) {
    if (num_commands >= MAX_COMMANDS) {
        return;  /* Table full */
    }
    
    commands[num_commands].name = name;
    commands[num_commands].help = help;
    commands[num_commands].handler = handler;
    num_commands++;
}

void shell_execute(const char *cmdline) {
    parsed_cmd_t cmd;
    
    /* Parse command line */
    if (!parser_parse(cmdline, &cmd)) {
        kprintf("Error: Failed to parse command\n");
        return;
    }
    
    /* Empty command */
    if (cmd.argc == 0) {
        return;
    }
    
    /* Find and execute command */
    shell_command_t *command = shell_find_command(cmd.argv[0]);
    if (command != NULL) {
        command->handler(cmd.argc, cmd.argv);
    } else {
        kprintf("Unknown command: %s\n", cmd.argv[0]);
        kprintf("Type 'help' for available commands.\n");
    }
}

NORETURN void shell_run(void) {
    char line[SHELL_MAX_CMD_LEN];
    
    /* Initialize commands */
    shell_init_commands();
    
    /* Welcome message */
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    kprintf("\n");
    kprintf("  ____              _          _    ___  ____  \n");
    kprintf(" / ___|  __ _ _   _(_)_ __ ___| |  / _ \\/ ___| \n");
    kprintf(" \\___ \\ / _` | | | | | '__/ _ \\ | | | | \\___ \\ \n");
    kprintf("  ___) | (_| | |_| | | | |  __/ | | |_| |___) |\n");
    kprintf(" |____/ \\__, |\\__,_|_|_|  \\___|_|  \\___/|____/ \n");
    kprintf("           |_|                                 \n");
    vga_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
    kprintf("\n");
    kprintf("Welcome to Squirel OS v%s\n", SQUIREL_VERSION_STRING);
    kprintf("Type 'help' for available commands.\n\n");
    
    /* Main shell loop */
    for (;;) {
        /* Print prompt */
        vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
        kprintf("%s", SHELL_PROMPT);
        vga_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
        
        /* Read input */
        shell_readline(line, sizeof(line));
        
        /* Execute command */
        shell_execute(line);
    }
}

/* ============================================================================
 * Helper Function for Commands
 * ============================================================================ */

/**
 * @brief Get the list of registered commands
 * 
 * Used by the help command to list available commands.
 */
int shell_get_commands(const shell_command_t **out_commands) {
    *out_commands = commands;
    return num_commands;
}
