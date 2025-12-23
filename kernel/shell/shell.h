/**
 * @file shell.h
 * @brief Basic shell interface
 * 
 * Provides a simple command-line interface for interacting with the OS.
 * The shell reads commands from keyboard, parses them, and executes
 * built-in commands.
 */

#ifndef _SHELL_H
#define _SHELL_H

#include <squirel/types.h>

/**
 * @brief Start the shell main loop
 * 
 * This function never returns. It continuously:
 *   1. Displays the prompt ("squirel$ ")
 *   2. Reads a line of input
 *   3. Parses the command
 *   4. Executes the command
 *   5. Repeats
 */
NORETURN void shell_run(void);

/**
 * @brief Execute a single command
 * 
 * @param cmdline  The full command line to execute
 */
void shell_execute(const char *cmdline);

/**
 * @brief Register a shell command
 * 
 * @param name    Command name (what user types)
 * @param help    Help text for the command
 * @param handler Function to call when command is executed
 */
typedef void (*shell_cmd_fn)(int argc, char *argv[]);

void shell_register_command(const char *name, const char *help, shell_cmd_fn handler);

#endif /* _SHELL_H */
