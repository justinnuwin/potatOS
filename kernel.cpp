#include "kernel.h"

#include "vga.hpp"

void wait_a_little() {
    int i = 100000;
    while (i--) {}
}

void kernel_main(void) {
    VGA vga;
    char c = ' ';
    while (1) {
        vga.display_char(c);
        vga.increment_cursor();
        wait_a_little();
        if (c == '~')
            c = ' ';
        else
            c++;
    }
}
