#include <stdint.h>

#ifndef ASM_FUNCTIONS_H
#define ASM_FUNCTIONS_H

struct GDTR {
    uint16_t length;
    void *base;
} __attribute__ ((packed));

struct IDTR {
    uint16_t length;
    void *base;
} __attribute__ ((packed));

inline void lgdt(void *base, uint16_t size) {
    static struct GDTR gdtr = {size, base};

    asm volatile ("lgdt %0" : : "m"(gdtr));
}

inline void lidt(void *base, uint16_t size) {
    struct IDTR idtr = {size, base};    // TODO: static?

    asm volatile ("lidt %0" : : "m"(idtr));
}

inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
    return ret;
}

#endif
