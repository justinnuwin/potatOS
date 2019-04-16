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
    poll_initialize_ps2();
    splash_screen();
    wait_longer();
    wait_longer();
    clear_screen();
    printk("%s\n\r", "Finished initializing PS/2");
    poll_initialize_ps2_keyboard();

    int i = 0;
    int _i = 0;
    while (1) {
        //printk("%x\n\r", ps2_poll_read());
        i++;
        _i--;
        int j = 30;
        while (j) {
            wait_a_little();
            j--;
        }
    }
}
