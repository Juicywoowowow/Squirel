/**
 * @file config.h
 * @brief Squirel OS build configuration and constants
 * 
 * Central location for build-time configuration options and system constants.
 * Modify these values to customize the kernel behavior.
 */

#ifndef _SQUIREL_CONFIG_H
#define _SQUIREL_CONFIG_H

/* ============================================================================
 * Version Information
 * ============================================================================ */

#define SQUIREL_VERSION_MAJOR   0
#define SQUIREL_VERSION_MINOR   1
#define SQUIREL_VERSION_PATCH   0
#define SQUIREL_VERSION_STRING  "0.1.0"
#define SQUIREL_NAME            "Squirel OS"

/* ============================================================================
 * Memory Configuration
 * ============================================================================ */

/** @brief Kernel is loaded at 1MB (0x100000) */
#define KERNEL_LOAD_ADDR        0x100000

/** @brief Kernel stack top address */
#define KERNEL_STACK_TOP        0x90000

/** @brief Kernel stack size (64KB) */
#define KERNEL_STACK_SIZE       0x10000

/* ============================================================================
 * VGA Configuration
 * ============================================================================ */

/** @brief VGA text mode buffer address */
#define VGA_BUFFER_ADDR         0xB8000

/** @brief VGA text mode width in characters */
#define VGA_WIDTH               80

/** @brief VGA text mode height in characters */
#define VGA_HEIGHT              25

/* ============================================================================
 * Serial Port Configuration
 * ============================================================================ */

/** @brief COM1 base port address */
#define COM1_PORT               0x3F8

/** @brief COM2 base port address */
#define COM2_PORT               0x2F8

/** @brief Default baud rate for serial */
#define SERIAL_BAUD_RATE        115200

/* ============================================================================
 * Keyboard Configuration
 * ============================================================================ */

/** @brief PS/2 keyboard data port */
#define KEYBOARD_DATA_PORT      0x60

/** @brief PS/2 keyboard status port */
#define KEYBOARD_STATUS_PORT    0x64

/** @brief Keyboard buffer size */
#define KEYBOARD_BUFFER_SIZE    256

/* ============================================================================
 * Shell Configuration
 * ============================================================================ */

/** @brief Shell prompt string */
#define SHELL_PROMPT            "squirel$ "

/** @brief Maximum command line length */
#define SHELL_MAX_CMD_LEN       256

/** @brief Maximum number of command arguments */
#define SHELL_MAX_ARGS          16

/* ============================================================================
 * Debug Configuration
 * ============================================================================ */

/** @brief Enable debug output to serial port */
#define DEBUG_SERIAL_OUTPUT     1

/** @brief Enable verbose boot messages */
#define DEBUG_VERBOSE_BOOT      1

#endif /* _SQUIREL_CONFIG_H */
