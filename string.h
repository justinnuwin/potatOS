#include <stddef.h>

#ifndef STRING_H
#define STRING_H

void *memset(void *ptr, char value, size_t num);
void *memcpy(void *dest, const void *source, size_t num);
size_t strlen(const char *str);
const char *strchr(const char *str, char c);

#endif
