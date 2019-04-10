#include "kernel.h"

#include "vga.h"
#include "printk.h"

void wait_a_little() {
    int i = 100000;
    while (i--) {}
}

void wait_longer() {
    int i = 50000000;
    while (i--) {}
}

void kernel_main(void) {
    clear_screen();
    wait_longer();
    fill_screen(VGA_COLOR_BROWN);
    wait_longer();
    clear_screen();

    int i = 0;
    while (1) {
        printk("%%Hello, world!%d%%\n\r", i++);
        wait_longer();
    }
}
