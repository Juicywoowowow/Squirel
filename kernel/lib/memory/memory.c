/**
 * @file memory.c
 * @brief Freestanding memory manipulation functions implementation
 * 
 * These functions are REQUIRED by GCC for freestanding code.
 * The compiler generates implicit calls to memcpy/memset for
 * struct assignments, array initialization, etc.
 * 
 * OPTIMIZATION NOTES:
 *   - For production, these could be optimized with SSE/AVX
 *   - Current implementation prioritizes simplicity and correctness
 *   - Word-aligned copies would improve performance for large blocks
 */

#include "memory.h"

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    /* Simple byte-by-byte copy */
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    if (d < s) {
        /* Copy forward (same as memcpy) */
        while (n--) {
            *d++ = *s++;
        }
    } else if (d > s) {
        /* Copy backward to handle overlap */
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    /* If d == s, nothing to do */
    
    return dest;
}

void *memset(void *dest, int c, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t byte = (uint8_t)c;
    
    while (n--) {
        *d++ = byte;
    }
    
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t *)s;
    uint8_t byte = (uint8_t)c;
    
    while (n--) {
        if (*p == byte) {
            return (void *)p;
        }
        p++;
    }
    
    return NULL;
}
