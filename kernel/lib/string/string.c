/**
 * @file string.c
 * @brief Freestanding string manipulation functions implementation
 * 
 * Implementation of standard string functions for freestanding environment.
 * These are essential for any C code and are often expected by the compiler.
 */

#include "string.h"

/* ============================================================================
 * String Length Functions
 * ============================================================================ */

size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

size_t strnlen(const char *str, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && str[len]) {
        len++;
    }
    return len;
}

/* ============================================================================
 * String Comparison Functions
 * ============================================================================ */

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

/* ============================================================================
 * String Copy Functions
 * ============================================================================ */

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    while (n > 0) {
        *dest++ = '\0';
        n--;
    }
    return ret;
}

/* ============================================================================
 * String Concatenation Functions
 * ============================================================================ */

char *strcat(char *dest, const char *src) {
    char *ret = dest;
    /* Find end of dest */
    while (*dest) {
        dest++;
    }
    /* Copy src */
    while ((*dest++ = *src++));
    return ret;
}

char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    /* Find end of dest */
    while (*dest) {
        dest++;
    }
    /* Copy up to n characters */
    while (n > 0 && *src) {
        *dest++ = *src++;
        n--;
    }
    *dest = '\0';
    return ret;
}

/* ============================================================================
 * String Search Functions
 * ============================================================================ */

char *strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c) {
            return (char *)str;
        }
        str++;
    }
    /* Also check for null terminator */
    if ((char)c == '\0') {
        return (char *)str;
    }
    return NULL;
}

char *strrchr(const char *str, int c) {
    const char *last = NULL;
    while (*str) {
        if (*str == (char)c) {
            last = str;
        }
        str++;
    }
    if ((char)c == '\0') {
        return (char *)str;
    }
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle) {
        return (char *)haystack;
    }
    
    while (*haystack) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }
        
        if (!*n) {
            return (char *)haystack;
        }
        
        haystack++;
    }
    
    return NULL;
}

/* ============================================================================
 * Character Classification Functions
 * ============================================================================ */

int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || 
           c == '\r' || c == '\f' || c == '\v';
}

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }
    return c;
}

int toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }
    return c;
}
