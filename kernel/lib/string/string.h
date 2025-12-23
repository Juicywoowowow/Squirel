/**
 * @file string.h
 * @brief Freestanding string manipulation functions
 * 
 * Standard string functions implemented without libc dependency.
 * These match the behavior of their libc counterparts.
 */

#ifndef _LIB_STRING_H
#define _LIB_STRING_H

#include <squirel/types.h>

/* ============================================================================
 * String Length Functions
 * ============================================================================ */

/**
 * @brief Calculate the length of a string
 * 
 * @param str  Null-terminated string
 * @return     Number of characters before the null terminator
 */
size_t strlen(const char *str);

/**
 * @brief Calculate the length of a string, limited to maxlen
 * 
 * @param str     Null-terminated string
 * @param maxlen  Maximum number of characters to scan
 * @return        Length of string, or maxlen if no null found
 */
size_t strnlen(const char *str, size_t maxlen);

/* ============================================================================
 * String Comparison Functions
 * ============================================================================ */

/**
 * @brief Compare two strings
 * 
 * @param s1  First string
 * @param s2  Second string
 * @return    <0 if s1<s2, 0 if equal, >0 if s1>s2
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Compare two strings up to n characters
 * 
 * @param s1  First string
 * @param s2  Second string
 * @param n   Maximum characters to compare
 * @return    <0 if s1<s2, 0 if equal, >0 if s1>s2
 */
int strncmp(const char *s1, const char *s2, size_t n);

/* ============================================================================
 * String Copy Functions
 * ============================================================================ */

/**
 * @brief Copy a string
 * 
 * @param dest  Destination buffer (must be large enough!)
 * @param src   Source string
 * @return      Pointer to dest
 * 
 * @warning No bounds checking - use strncpy for safety
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief Copy a string with length limit
 * 
 * @param dest  Destination buffer
 * @param src   Source string
 * @param n     Maximum characters to copy
 * @return      Pointer to dest
 * 
 * @note If src < n, dest is padded with nulls
 * @note If src >= n, dest is NOT null-terminated!
 */
char *strncpy(char *dest, const char *src, size_t n);

/* ============================================================================
 * String Concatenation Functions
 * ============================================================================ */

/**
 * @brief Concatenate two strings
 * 
 * @param dest  Destination string (must have enough space!)
 * @param src   Source string to append
 * @return      Pointer to dest
 */
char *strcat(char *dest, const char *src);

/**
 * @brief Concatenate with length limit
 * 
 * @param dest  Destination string
 * @param src   Source string to append
 * @param n     Maximum characters to append
 * @return      Pointer to dest
 */
char *strncat(char *dest, const char *src, size_t n);

/* ============================================================================
 * String Search Functions
 * ============================================================================ */

/**
 * @brief Find first occurrence of a character
 * 
 * @param str  String to search
 * @param c    Character to find
 * @return     Pointer to character, or NULL if not found
 */
char *strchr(const char *str, int c);

/**
 * @brief Find last occurrence of a character
 * 
 * @param str  String to search
 * @param c    Character to find
 * @return     Pointer to character, or NULL if not found
 */
char *strrchr(const char *str, int c);

/**
 * @brief Find a substring
 * 
 * @param haystack  String to search in
 * @param needle    Substring to find
 * @return          Pointer to first occurrence, or NULL
 */
char *strstr(const char *haystack, const char *needle);

/* ============================================================================
 * Character Classification Functions
 * ============================================================================ */

/**
 * @brief Check if character is whitespace
 */
int isspace(int c);

/**
 * @brief Check if character is a digit
 */
int isdigit(int c);

/**
 * @brief Check if character is alphabetic
 */
int isalpha(int c);

/**
 * @brief Check if character is alphanumeric
 */
int isalnum(int c);

/**
 * @brief Convert character to lowercase
 */
int tolower(int c);

/**
 * @brief Convert character to uppercase
 */
int toupper(int c);

#endif /* _LIB_STRING_H */
