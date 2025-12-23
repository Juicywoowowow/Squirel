/**
 * @file parser.h
 * @brief Command line parser interface
 * 
 * Parses command lines into argc/argv format, handling:
 *   - Whitespace separation
 *   - Quoted strings (future)
 *   - Escape sequences (future)
 */

#ifndef _SHELL_PARSER_H
#define _SHELL_PARSER_H

#include <squirel/types.h>
#include <squirel/config.h>

/**
 * @brief Parsed command structure
 */
typedef struct {
    int argc;                        /**< Number of arguments */
    char *argv[SHELL_MAX_ARGS];      /**< Argument pointers (into buffer) */
    char buffer[SHELL_MAX_CMD_LEN];  /**< Copy of command line */
} parsed_cmd_t;

/**
 * @brief Parse a command line into argc/argv
 * 
 * @param cmdline  The command line to parse
 * @param cmd      Output structure to fill
 * @return         true on success, false on error
 * 
 * @note The cmdline is copied into cmd->buffer and modified
 *       (spaces replaced with nulls). argv pointers point into buffer.
 */
bool parser_parse(const char *cmdline, parsed_cmd_t *cmd);

/**
 * @brief Skip leading whitespace in a string
 * 
 * @param str  The string
 * @return     Pointer to first non-whitespace character
 */
const char *parser_skip_whitespace(const char *str);

/**
 * @brief Trim trailing whitespace from a string (in-place)
 * 
 * @param str  The string to trim (will be modified)
 */
void parser_trim_trailing(char *str);

#endif /* _SHELL_PARSER_H */
