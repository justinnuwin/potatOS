#include "kernel.h"

#include "vga.hpp"

void wait_a_little() {
    int i = 100000;
    while (i--) {}
}

void wait_longer() {
    int i = 50000000;
    while (i--) {}
}

void kernel_main(void) {
    VGA vga;
    while (1) {
        vga.display_string("Hello world!\n\r");
        wait_longer();
    }
}
