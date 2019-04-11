#include "asm_functions.h"

/*
inline void lgdt(void *base, uint16_t size) {
    static struct GDTR {
        uint16_t length;
        void *base;
    } __attribute__ ((packed));

    GDTR.length size;
    GDTR.base = base;

    asm ("lgdt %0" : : "m"(GDTR));
}
*/

inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("intb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
    return ret;
}
