#include "kernel.h"

#include "vga.h"
#include "printk.h"
#include "ps2.h"

void wait_a_little() {
    int i = 100000;
    while (i--);
}

void wait_longer() {
    int i = 50000000;
    while (i--);
}

void kernel_main(void) {
    if (poll_initialize_ps2())
        poll_initialize_ps2_keyboard();
    wait_longer();
    clear_screen();

    splash_screen();
    wait_longer();
    wait_longer();
    clear_screen();

    int i = 0;
    while (1) {
        printk("%x\n", i++);
    }
}
