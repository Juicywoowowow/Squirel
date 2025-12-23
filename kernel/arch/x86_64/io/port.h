/**
 * @file port.h
 * @brief x86_64 I/O port access functions
 * 
 * Provides inline assembly functions for reading/writing hardware I/O ports.
 * These are the fundamental building blocks for all hardware communication.
 * 
 * PORT I/O OVERVIEW:
 *   x86 has a separate I/O address space (0x0000 - 0xFFFF) for hardware.
 *   Devices like VGA, keyboard, serial ports are accessed through port I/O.
 *   
 *   Common port ranges:
 *     0x000-0x01F : DMA controller
 *     0x020-0x03F : Programmable Interrupt Controller (PIC)
 *     0x040-0x05F : System timer (PIT)
 *     0x060-0x06F : Keyboard controller
 *     0x0CF-0x0CF : Fast A20 gate
 *     0x3F8-0x3FF : COM1 serial port
 *     0x3D4-0x3D5 : VGA CRT controller
 * 
 * INLINE ASSEMBLY SYNTAX (GCC):
 *   __asm__ volatile ("instruction" : outputs : inputs : clobbers);
 *   
 *   Constraints:
 *     "a" = AL/AX/EAX/RAX register
 *     "d" = DL/DX/EDX/RDX register
 *     "N" = 8-bit immediate (0-255)
 *     "=" = write-only operand
 */

#ifndef _ARCH_X86_64_PORT_H
#define _ARCH_X86_64_PORT_H

#include <squirel/types.h>

/* ============================================================================
 * 8-bit Port I/O
 * ============================================================================ */

/**
 * @brief Write a byte to an I/O port
 * 
 * @param port   The port number (0-65535)
 * @param value  The byte value to write (0-255)
 * 
 * @note This is the most common port I/O operation.
 * 
 * ASSEMBLY:
 *   outb %al, %dx
 *   - AL contains the value to write
 *   - DX contains the port number
 */
static ALWAYS_INLINE void outb(uint16_t port, uint8_t value) {
    __asm__ volatile (
        "outb %0, %1"           /* Output byte from AL to port in DX */
        :                       /* No outputs */
        : "a"(value),           /* Input: value in AL */
          "Nd"(port)            /* Input: port in DX (or immediate if < 256) */
    );
}

/**
 * @brief Read a byte from an I/O port
 * 
 * @param port  The port number (0-65535)
 * @return      The byte value read from the port
 * 
 * ASSEMBLY:
 *   inb %dx, %al
 *   - DX contains the port number
 *   - AL receives the value read
 */
static ALWAYS_INLINE uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile (
        "inb %1, %0"            /* Input byte from port in DX to AL */
        : "=a"(ret)             /* Output: result in AL */
        : "Nd"(port)            /* Input: port in DX */
    );
    return ret;
}

/* ============================================================================
 * 16-bit Port I/O
 * ============================================================================ */

/**
 * @brief Write a word (16-bit) to an I/O port
 * 
 * @param port   The port number
 * @param value  The word value to write
 */
static ALWAYS_INLINE void outw(uint16_t port, uint16_t value) {
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"(value),
          "Nd"(port)
    );
}

/**
 * @brief Read a word (16-bit) from an I/O port
 * 
 * @param port  The port number
 * @return      The word value read
 */
static ALWAYS_INLINE uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile (
        "inw %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

/* ============================================================================
 * 32-bit Port I/O
 * ============================================================================ */

/**
 * @brief Write a doubleword (32-bit) to an I/O port
 * 
 * @param port   The port number
 * @param value  The doubleword value to write
 */
static ALWAYS_INLINE void outl(uint16_t port, uint32_t value) {
    __asm__ volatile (
        "outl %0, %1"
        :
        : "a"(value),
          "Nd"(port)
    );
}

/**
 * @brief Read a doubleword (32-bit) from an I/O port
 * 
 * @param port  The port number
 * @return      The doubleword value read
 */
static ALWAYS_INLINE uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile (
        "inl %1, %0"
        : "=a"(ret)
        : "Nd"(port)
    );
    return ret;
}

/* ============================================================================
 * I/O Wait (for slow devices)
 * ============================================================================ */

/**
 * @brief Wait for an I/O operation to complete
 * 
 * Some older hardware requires a small delay between I/O operations.
 * Writing to port 0x80 (POST diagnostic port) is a common way to
 * introduce this delay without side effects.
 */
static ALWAYS_INLINE void io_wait(void) {
    outb(0x80, 0);
}

#endif /* _ARCH_X86_64_PORT_H */
