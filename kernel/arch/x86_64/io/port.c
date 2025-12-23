/**
 * @file port.c
 * @brief Port I/O implementation (if non-inline versions needed)
 * 
 * The actual port I/O functions are defined as static inline in port.h.
 * This file exists for cases where non-inline versions are needed
 * (e.g., function pointers, debugging).
 * 
 * For normal use, include port.h directly.
 */

#include "port.h"

/* 
 * Non-inline versions for special cases.
 * These are rarely needed but provided for completeness.
 */

/**
 * @brief Non-inline version of outb
 * @see outb in port.h for the inline version
 */
void port_outb(uint16_t port, uint8_t value) {
    outb(port, value);
}

/**
 * @brief Non-inline version of inb
 * @see inb in port.h for the inline version
 */
uint8_t port_inb(uint16_t port) {
    return inb(port);
}

/**
 * @brief Non-inline version of outw
 */
void port_outw(uint16_t port, uint16_t value) {
    outw(port, value);
}

/**
 * @brief Non-inline version of inw
 */
uint16_t port_inw(uint16_t port) {
    return inw(port);
}
