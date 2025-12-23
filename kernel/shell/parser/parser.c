/**
 * @file parser.c
 * @brief Command line parser implementation
 * 
 * Splits a command line into individual arguments.
 */

#include "parser.h"
#include <lib/string/string.h>
#include <lib/memory/memory.h>

const char *parser_skip_whitespace(const char *str) {
    while (*str && isspace(*str)) {
        str++;
    }
    return str;
}

void parser_trim_trailing(char *str) {
    if (!str || !*str) return;
    
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace(*end)) {
        *end = '\0';
        end--;
    }
}

bool parser_parse(const char *cmdline, parsed_cmd_t *cmd) {
    if (!cmdline || !cmd) {
        return false;
    }
    
    /* Initialize output */
    cmd->argc = 0;
    memset(cmd->argv, 0, sizeof(cmd->argv));
    memset(cmd->buffer, 0, sizeof(cmd->buffer));
    
    /* Copy command line to buffer */
    size_t len = strlen(cmdline);
    if (len >= SHELL_MAX_CMD_LEN) {
        len = SHELL_MAX_CMD_LEN - 1;
    }
    memcpy(cmd->buffer, cmdline, len);
    cmd->buffer[len] = '\0';
    
    /* Trim trailing whitespace */
    parser_trim_trailing(cmd->buffer);
    
    /* Parse arguments */
    char *ptr = cmd->buffer;
    
    while (*ptr && cmd->argc < SHELL_MAX_ARGS) {
        /* Skip leading whitespace */
        while (*ptr && isspace(*ptr)) {
            ptr++;
        }
        
        if (!*ptr) {
            break;  /* End of string */
        }
        
        /* Found start of argument */
        cmd->argv[cmd->argc++] = ptr;
        
        /* Find end of argument */
        while (*ptr && !isspace(*ptr)) {
            ptr++;
        }
        
        /* Null-terminate this argument */
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
    }
    
    return true;
}
