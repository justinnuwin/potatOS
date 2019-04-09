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
    wait_a_little();
    fill_screen(VGA_COLOR_BROWN);
    wait_a_little();
    clear_screen();

    while (1) {
        printk("%%boobs!%d%%\n\r");
        wait_longer();
    }
}
