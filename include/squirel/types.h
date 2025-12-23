/**
 * @file types.h
 * @brief Core type definitions for Squirel OS
 * 
 * Provides fixed-width integer types and common typedefs for a freestanding
 * environment. This replaces <stdint.h> and <stddef.h> from the standard
 * library.
 * 
 * TYPE WIDTHS (x86_64):
 *   char      = 1 byte  (8 bits)
 *   short     = 2 bytes (16 bits)
 *   int       = 4 bytes (32 bits)
 *   long      = 8 bytes (64 bits) on Linux, 4 bytes on Windows!
 *   long long = 8 bytes (64 bits)
 * 
 * NOTE: We use 'long long' for 64-bit types to be safe across platforms
 *       during development, even though we target x86_64 Linux ABI.
 */

#ifndef _SQUIREL_TYPES_H
#define _SQUIREL_TYPES_H

/* ============================================================================
 * Fixed-Width Unsigned Integer Types
 * ============================================================================ */

/** @brief 8-bit unsigned integer (0 to 255) */
typedef unsigned char       uint8_t;

/** @brief 16-bit unsigned integer (0 to 65,535) */
typedef unsigned short      uint16_t;

/** @brief 32-bit unsigned integer (0 to 4,294,967,295) */
typedef unsigned int        uint32_t;

/** @brief 64-bit unsigned integer (0 to 18,446,744,073,709,551,615) */
typedef unsigned long long  uint64_t;

/* ============================================================================
 * Fixed-Width Signed Integer Types
 * ============================================================================ */

/** @brief 8-bit signed integer (-128 to 127) */
typedef signed char         int8_t;

/** @brief 16-bit signed integer (-32,768 to 32,767) */
typedef signed short        int16_t;

/** @brief 32-bit signed integer (-2,147,483,648 to 2,147,483,647) */
typedef signed int          int32_t;

/** @brief 64-bit signed integer */
typedef signed long long    int64_t;

/* ============================================================================
 * Size Types
 * ============================================================================ */

/** @brief Unsigned size type (result of sizeof, array indices) */
typedef uint64_t            size_t;

/** @brief Signed size type (for functions that can return -1 on error) */
typedef int64_t             ssize_t;

/** @brief Pointer-sized unsigned integer (can hold any pointer value) */
typedef uint64_t            uintptr_t;

/** @brief Pointer-sized signed integer */
typedef int64_t             intptr_t;

/* ============================================================================
 * Special Values
 * ============================================================================ */

/** @brief Null pointer constant */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* ============================================================================
 * Boolean Type
 * ============================================================================ */

/** @brief Boolean type (true/false) */
typedef _Bool               bool;

/** @brief Boolean true value */
#define true  1

/** @brief Boolean false value */
#define false 0

/* ============================================================================
 * Compiler Attributes
 * ============================================================================ */

/** 
 * @brief Mark a function as never returning
 * @example NORETURN void panic(const char *msg);
 */
#define NORETURN __attribute__((noreturn))

/**
 * @brief Mark a variable/function as possibly unused (suppress warnings)
 * @example static UNUSED int debug_counter = 0;
 */
#define UNUSED __attribute__((unused))

/**
 * @brief Force a function to always be inlined
 * @example static ALWAYS_INLINE uint8_t inb(uint16_t port);
 */
#define ALWAYS_INLINE __attribute__((always_inline)) inline

/**
 * @brief Pack a struct with no padding
 * @example struct PACKED some_hardware_struct { ... };
 */
#define PACKED __attribute__((packed))

/**
 * @brief Align a variable/struct to a specific boundary
 * @example ALIGNED(4096) uint8_t page_table[4096];
 */
#define ALIGNED(x) __attribute__((aligned(x)))

/* ============================================================================
 * Limit Macros
 * ============================================================================ */

#define UINT8_MAX   ((uint8_t)0xFF)
#define UINT16_MAX  ((uint16_t)0xFFFF)
#define UINT32_MAX  ((uint32_t)0xFFFFFFFF)
#define UINT64_MAX  ((uint64_t)0xFFFFFFFFFFFFFFFF)

#define INT8_MIN    ((int8_t)-128)
#define INT8_MAX    ((int8_t)127)
#define INT16_MIN   ((int16_t)-32768)
#define INT16_MAX   ((int16_t)32767)
#define INT32_MIN   ((int32_t)-2147483648)
#define INT32_MAX   ((int32_t)2147483647)
#define INT64_MIN   ((int64_t)(-9223372036854775807LL - 1))
#define INT64_MAX   ((int64_t)9223372036854775807LL)

#define SIZE_MAX    UINT64_MAX

#endif /* _SQUIREL_TYPES_H */
