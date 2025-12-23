/**
 * @file serial.h
 * @brief Serial port driver interface
 * 
 * Provides output to the serial port (COM1) for debugging.
 * In QEMU, serial output can be captured with -serial stdio.
 * 
 * SERIAL PORT OVERVIEW:
 *   COM1 is at I/O port 0x3F8
 *   Baud rate is typically 115200
 *   
 *   Port offsets:
 *     +0: Data (read/write)
 *     +1: Interrupt Enable
 *     +2: FIFO Control (write) / Interrupt ID (read)
 *     +3: Line Control
 *     +4: Modem Control
 *     +5: Line Status
 *     +6: Modem Status
 *     +7: Scratch Register
 */

#ifndef _DRIVERS_SERIAL_H
#define _DRIVERS_SERIAL_H

#include <squirel/types.h>

/**
 * @brief Initialize the serial port (COM1)
 * 
 * Configures COM1 with 115200 baud, 8N1 (8 data bits, no parity, 1 stop bit)
 */
void serial_init(void);

/**
 * @brief Check if transmit buffer is empty
 * 
 * @return true if ready to send, false if busy
 */
bool serial_ready(void);

/**
 * @brief Write a character to serial port
 * 
 * @param c  Character to send
 */
void serial_putchar(char c);

/**
 * @brief Write a string to serial port
 * 
 * @param str  Null-terminated string
 */
void serial_print(const char *str);

/**
 * @brief Printf-style output to serial port
 * 
 * @param fmt  Format string
 * @param ...  Arguments
 * @return     Number of characters written
 */
int serial_printf(const char *fmt, ...);

#endif /* _DRIVERS_SERIAL_H */
