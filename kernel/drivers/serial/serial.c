/**
 * @file serial.c
 * @brief Serial port driver implementation
 * 
 * Implements serial port output for debugging via QEMU.
 * Uses polling (not interrupts) for simplicity.
 */

#include "serial.h"
#include <squirel/config.h>
#include <arch/x86_64/io/port.h>
#include <stdarg.h>

/* ============================================================================
 * Port Offsets (relative to COM1_PORT)
 * ============================================================================ */

#define SERIAL_DATA         0   /* Data register (R/W) */
#define SERIAL_INT_ENABLE   1   /* Interrupt enable */
#define SERIAL_FIFO_CTRL    2   /* FIFO control */
#define SERIAL_LINE_CTRL    3   /* Line control */
#define SERIAL_MODEM_CTRL   4   /* Modem control */
#define SERIAL_LINE_STATUS  5   /* Line status */
#define SERIAL_MODEM_STATUS 6   /* Modem status */

/* Line status register bits */
#define SERIAL_STATUS_THRE  0x20  /* Transmit holding register empty */

/* ============================================================================
 * Public Functions
 * ============================================================================ */

void serial_init(void) {
    uint16_t port = COM1_PORT;
    
    /* Disable interrupts */
    outb(port + SERIAL_INT_ENABLE, 0x00);
    
    /* Enable DLAB (set baud rate divisor) */
    outb(port + SERIAL_LINE_CTRL, 0x80);
    
    /* 
     * Set baud rate divisor (115200 baud)
     * Divisor = 115200 / baud
     * For 115200: divisor = 1
     * Low byte goes to DATA, high byte to INT_ENABLE (when DLAB=1)
     */
    outb(port + SERIAL_DATA, 0x01);         /* Divisor low byte */
    outb(port + SERIAL_INT_ENABLE, 0x00);   /* Divisor high byte */
    
    /* 
     * Set line control:
     * Bits 0-1: 11 = 8 data bits
     * Bit 2: 0 = 1 stop bit
     * Bits 3-5: 000 = no parity
     * Bit 7: 0 = disable DLAB
     */
    outb(port + SERIAL_LINE_CTRL, 0x03);    /* 8N1 */
    
    /* 
     * Enable FIFO, clear them, with 14-byte threshold
     */
    outb(port + SERIAL_FIFO_CTRL, 0xC7);
    
    /* 
     * Enable DTR, RTS, and OUT2 (interrupt enable in some systems)
     */
    outb(port + SERIAL_MODEM_CTRL, 0x0B);
}

bool serial_ready(void) {
    return (inb(COM1_PORT + SERIAL_LINE_STATUS) & SERIAL_STATUS_THRE) != 0;
}

void serial_putchar(char c) {
    /* Wait for transmit buffer to be empty */
    while (!serial_ready()) {
        /* Busy wait */
    }
    
    outb(COM1_PORT + SERIAL_DATA, c);
}

void serial_print(const char *str) {
    while (*str) {
        /* Convert newline to CRLF for proper terminal display */
        if (*str == '\n') {
            serial_putchar('\r');
        }
        serial_putchar(*str++);
    }
}

/* Simple serial printf (limited implementation) */
int serial_printf(const char *fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    
    /* Use our kernel snprintf */
    extern int ksnprintf(char *buf, size_t size, const char *fmt, ...);
    int len = ksnprintf(buf, sizeof(buf), fmt, args);
    
    va_end(args);
    
    serial_print(buf);
    return len;
}
