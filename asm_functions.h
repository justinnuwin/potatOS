#include <stdint.h>

#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

inline void outb(uint16_t port, uint8_t val);
inline uint8_t inb(uint16_t port);

#endif
