#include "kernel.h"

#include "vga.hpp"

void wait_a_little() {
    int i = 100000;
    while (i--) {}
}

void wait_longer() {
    int i = 5000000;
    while (i--) {}
}

void kernel_main(void) {
    VGA vga;
    while (1) {
        vga.display_string("Hello world!\n\r\0", VGA_COLOR_BROWN, VGA_COLOR_LIGHT_MAGENTA);
        wait_longer();
    }
}
