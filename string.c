#include "string.h"

#include <stdint.h>
#include <stdbool.h>

void *memset(void *ptr, char value, size_t num) {
    uint8_t *p = (uint8_t *)ptr; 
    while (num--)
        p[num] = (uint8_t)value;
    return ptr;
}

void *memcpy(void *destination, const void *source, size_t num) {
    uint8_t *dest = (uint8_t *)destination;
    const uint8_t *src = (uint8_t *)source;
    for (size_t i = 0; i < num; i++)
        dest[i] = src[i];
    return destination;
}

size_t strlen(const char *str) {
    size_t length = 0;
    while (str[length])
        length++;
    return length;
}

const char *strchr(const char *str, char c) {
    while (*str != c) {
        if (!*str)
            return NULL;
        else
            str++;
    }
    return str;
}

