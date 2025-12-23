/**
 * @file memory.h
 * @brief Freestanding memory manipulation functions
 * 
 * Standard memory functions implemented without libc dependency.
 * GCC may generate implicit calls to these functions for struct
 * assignments and initializations, so they MUST be available.
 */

#ifndef _LIB_MEMORY_H
#define _LIB_MEMORY_H

#include <squirel/types.h>

/**
 * @brief Copy memory from source to destination
 * 
 * @param dest  Destination buffer
 * @param src   Source buffer
 * @param n     Number of bytes to copy
 * @return      Pointer to dest
 * 
 * @warning Behavior undefined if regions overlap - use memmove instead
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * @brief Copy memory, handling overlapping regions
 * 
 * @param dest  Destination buffer
 * @param src   Source buffer
 * @param n     Number of bytes to copy
 * @return      Pointer to dest
 * 
 * @note Safe for overlapping regions (copies via temp or reverse)
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * @brief Fill memory with a byte value
 * 
 * @param dest  Destination buffer
 * @param c     Byte value to fill with (cast to unsigned char)
 * @param n     Number of bytes to fill
 * @return      Pointer to dest
 */
void *memset(void *dest, int c, size_t n);

/**
 * @brief Compare two memory regions
 * 
 * @param s1  First memory region
 * @param s2  Second memory region
 * @param n   Number of bytes to compare
 * @return    <0 if s1<s2, 0 if equal, >0 if s1>s2
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Find a byte in memory
 * 
 * @param s  Memory region to search
 * @param c  Byte value to find
 * @param n  Number of bytes to search
 * @return   Pointer to first occurrence, or NULL
 */
void *memchr(const void *s, int c, size_t n);

#endif /* _LIB_MEMORY_H */
