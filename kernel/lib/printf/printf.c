/**
 * @file printf.c
 * @brief Minimal printf implementation for kernel
 * 
 * A simple printf implementation using VGA output. Supports the most
 * common format specifiers needed for kernel debugging and shell output.
 * 
 * IMPLEMENTATION APPROACH:
 *   This uses a "put character" callback approach so the same formatting
 *   code can output to VGA, serial, or a buffer.
 */

#include "printf.h"
#include <drivers/vga/vga_text.h>
#include <lib/string/string.h>

/* ============================================================================
 * Private Types and State
 * ============================================================================ */

/** @brief Output function type - prints a single character */
typedef void (*putchar_fn)(char c, void *ctx);

/* ============================================================================
 * Private Helper Functions
 * ============================================================================ */

/**
 * @brief Print character to VGA (for kprintf)
 */
static void vga_putchar_wrapper(char c, void *ctx UNUSED) {
    vga_putchar(c);
}

/**
 * @brief Print character to buffer (for ksprintf)
 */
typedef struct {
    char *buf;
    size_t pos;
    size_t max;
} sprintf_ctx_t;

static void buf_putchar(char c, void *ctx) {
    sprintf_ctx_t *sctx = (sprintf_ctx_t *)ctx;
    if (sctx->pos < sctx->max - 1) {
        sctx->buf[sctx->pos++] = c;
    }
}

/**
 * @brief Print an unsigned integer in given base
 * 
 * @param putc    Output function
 * @param ctx     Context for output function
 * @param value   Value to print
 * @param base    Base (10 or 16)
 * @param upper   Use uppercase hex digits
 * @param width   Minimum field width (0 = no minimum)
 * @param zeropad Pad with zeros instead of spaces
 * @return        Number of characters printed
 */
static int print_uint(putchar_fn putc, void *ctx, uint64_t value, 
                      int base, bool upper, int width, bool zeropad) {
    char buf[24];   /* Max: 20 digits for 64-bit decimal + sign + null */
    char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int pos = 0;
    int count = 0;
    
    /* Build number string in reverse */
    if (value == 0) {
        buf[pos++] = '0';
    } else {
        while (value > 0) {
            buf[pos++] = digits[value % base];
            value /= base;
        }
    }
    
    /* Calculate padding */
    int pad = width - pos;
    char padchar = zeropad ? '0' : ' ';
    
    /* Output padding */
    while (pad > 0) {
        putc(padchar, ctx);
        count++;
        pad--;
    }
    
    /* Output digits in correct order */
    while (pos > 0) {
        putc(buf[--pos], ctx);
        count++;
    }
    
    return count;
}

/**
 * @brief Print a signed integer
 */
static int print_int(putchar_fn putc, void *ctx, int64_t value,
                     int width, bool zeropad) {
    int count = 0;
    
    if (value < 0) {
        putc('-', ctx);
        count++;
        value = -value;
        if (width > 0) width--;
    }
    
    count += print_uint(putc, ctx, (uint64_t)value, 10, false, width, zeropad);
    return count;
}

/**
 * @brief Print a string with optional width/alignment
 */
static int print_string(putchar_fn putc, void *ctx, const char *str, 
                        int width, bool left_justify) {
    int count = 0;
    if (str == NULL) {
        str = "(null)";
    }
    
    /* Calculate string length */
    int len = 0;
    const char *s = str;
    while (*s++) len++;
    
    /* Right padding (left-justify): print string first, then pad */
    if (left_justify) {
        while (*str) {
            putc(*str++, ctx);
            count++;
        }
        while (count < width) {
            putc(' ', ctx);
            count++;
        }
    } else {
        /* Left padding (right-justify): pad first, then print string */
        int pad = width - len;
        while (pad > 0) {
            putc(' ', ctx);
            count++;
            pad--;
        }
        while (*str) {
            putc(*str++, ctx);
            count++;
        }
    }
    return count;
}

/**
 * @brief Core printf implementation
 */
static int do_printf(putchar_fn putc, void *ctx, const char *fmt, va_list args) {
    int count = 0;
    
    while (*fmt) {
        if (*fmt != '%') {
            /* Regular character */
            putc(*fmt++, ctx);
            count++;
            continue;
        }
        
        fmt++;  /* Skip '%' */
        
        /* Parse flags */
        bool zeropad = false;
        bool left_justify = false;
        
        while (*fmt == '-' || *fmt == '0') {
            if (*fmt == '-') left_justify = true;
            if (*fmt == '0') zeropad = true;
            fmt++;
        }
        
        /* Parse width */
        int width = 0;
        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }
        
        /* Handle specifier */
        switch (*fmt) {
            case '%':
                putc('%', ctx);
                count++;
                break;
                
            case 'c': {
                char c = (char)va_arg(args, int);
                putc(c, ctx);
                count++;
                break;
            }
            
            case 's': {
                const char *s = va_arg(args, const char *);
                count += print_string(putc, ctx, s, width, left_justify);
                break;
            }
            
            case 'd':
            case 'i': {
                int64_t val = va_arg(args, int);
                count += print_int(putc, ctx, val, width, zeropad);
                break;
            }
            
            case 'u': {
                uint64_t val = va_arg(args, unsigned int);
                count += print_uint(putc, ctx, val, 10, false, width, zeropad);
                break;
            }
            
            case 'x': {
                uint64_t val = va_arg(args, unsigned int);
                count += print_uint(putc, ctx, val, 16, false, width, zeropad);
                break;
            }
            
            case 'X': {
                uint64_t val = va_arg(args, unsigned int);
                count += print_uint(putc, ctx, val, 16, true, width, zeropad);
                break;
            }
            
            case 'p': {
                void *ptr = va_arg(args, void *);
                putc('0', ctx);
                putc('x', ctx);
                count += 2;
                count += print_uint(putc, ctx, (uint64_t)ptr, 16, false, 16, true);
                break;
            }
            
            case 'l': {
                /* Handle %ld, %lu, %lx, %llx etc. */
                fmt++;
                bool is_long_long = false;
                if (*fmt == 'l') {
                    is_long_long = true;
                    fmt++;
                }
                
                switch (*fmt) {
                    case 'd':
                    case 'i': {
                        int64_t val = is_long_long ? 
                            va_arg(args, long long) : va_arg(args, long);
                        count += print_int(putc, ctx, val, width, zeropad);
                        break;
                    }
                    case 'u': {
                        uint64_t val = is_long_long ?
                            va_arg(args, unsigned long long) : va_arg(args, unsigned long);
                        count += print_uint(putc, ctx, val, 10, false, width, zeropad);
                        break;
                    }
                    case 'x': {
                        uint64_t val = is_long_long ?
                            va_arg(args, unsigned long long) : va_arg(args, unsigned long);
                        count += print_uint(putc, ctx, val, 16, false, width, zeropad);
                        break;
                    }
                    default:
                        putc('%', ctx);
                        putc('l', ctx);
                        count += 2;
                        continue;  /* Don't advance fmt */
                }
                break;
            }
            
            default:
                /* Unknown specifier - print literally */
                putc('%', ctx);
                putc(*fmt, ctx);
                count += 2;
                break;
        }
        
        fmt++;
    }
    
    return count;
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

int kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = do_printf(vga_putchar_wrapper, NULL, fmt, args);
    va_end(args);
    return ret;
}

int kvprintf(const char *fmt, va_list args) {
    return do_printf(vga_putchar_wrapper, NULL, fmt, args);
}

int ksprintf(char *buf, const char *fmt, ...) {
    sprintf_ctx_t ctx = { buf, 0, SIZE_MAX };
    va_list args;
    va_start(args, fmt);
    int ret = do_printf(buf_putchar, &ctx, fmt, args);
    va_end(args);
    buf[ctx.pos] = '\0';
    return ret;
}

int ksnprintf(char *buf, size_t size, const char *fmt, ...) {
    sprintf_ctx_t ctx = { buf, 0, size };
    va_list args;
    va_start(args, fmt);
    int ret = do_printf(buf_putchar, &ctx, fmt, args);
    va_end(args);
    if (size > 0) {
        buf[ctx.pos] = '\0';
    }
    return ret;
}
