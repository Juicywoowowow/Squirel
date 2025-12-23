/**
 * @file printf.h
 * @brief Minimal printf implementation for kernel
 * 
 * Provides printf-like formatting for kernel output.
 * Uses VGA driver by default, but can output to any function.
 * 
 * SUPPORTED FORMAT SPECIFIERS:
 *   %s  - String
 *   %c  - Character
 *   %d  - Signed decimal integer
 *   %i  - Signed decimal integer (same as %d)
 *   %u  - Unsigned decimal integer
 *   %x  - Lowercase hexadecimal
 *   %X  - Uppercase hexadecimal
 *   %p  - Pointer (printed as 0x...)
 *   %%  - Literal percent sign
 * 
 * FLAGS (partial support):
 *   0   - Pad with zeros (for %x, %X, %d, %u)
 *   -   - Left justify (not implemented)
 *   +   - Show + for positive numbers (not implemented)
 * 
 * WIDTH:
 *   Number after % specifies minimum field width
 */

#ifndef _LIB_PRINTF_H
#define _LIB_PRINTF_H

#include <squirel/types.h>

/* We need stdarg.h from the compiler (this is freestanding-safe) */
#include <stdarg.h>

/**
 * @brief Printf to VGA screen
 * 
 * @param fmt  Format string
 * @param ...  Variable arguments
 * @return     Number of characters printed
 */
int kprintf(const char *fmt, ...);

/**
 * @brief Printf with va_list
 * 
 * @param fmt  Format string
 * @param args Variable argument list
 * @return     Number of characters printed
 */
int kvprintf(const char *fmt, va_list args);

/**
 * @brief Sprintf to buffer
 * 
 * @param buf  Destination buffer
 * @param fmt  Format string
 * @param ...  Variable arguments
 * @return     Number of characters written (excluding null)
 * 
 * @warning No buffer size checking - use ksnprintf for safety
 */
int ksprintf(char *buf, const char *fmt, ...);

/**
 * @brief Snprintf with buffer size limit
 * 
 * @param buf   Destination buffer
 * @param size  Buffer size (including space for null)
 * @param fmt   Format string
 * @param ...   Variable arguments
 * @return      Number of characters that would have been written
 */
int ksnprintf(char *buf, size_t size, const char *fmt, ...);

#endif /* _LIB_PRINTF_H */
