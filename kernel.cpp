#include "kernel.h"

// #include "vga.hpp"
#include <stdint.h>

/*
inline void
clear() {
    int count = 80 * 25;
    uint16_t *buffer = (uint16_t *)0xb8000;
    while (count--) {
        *buffer = 0x004a;
        buffer++;
    }
}
*/    

void kernel_main(void) {
    // VGA vga;
    // vga.clear();
    uint16_t *buffer = (uint16_t *)0xb8000;
    *buffer = 0x004a;
    //clear();
    while (1) {
    }
}
