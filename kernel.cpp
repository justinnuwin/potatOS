#include "kernel.h"

#include "vga.h"
#include "printk.h"
#include "ps2.h"
#include "keyboard.h"
#include "interrupt.h"

void wait_a_little() {
    int i = 100000;
    while (i--);
}

void wait_longer() {
    int i = 50000000;
    while (i--);
}

void kernel_main(void) {
    if (poll_init_ps2())
        poll_init_ps2_keyboard();
    wait_longer();
    clear_screen();

    splash_screen();
    wait_longer();
    wait_longer();
    clear_screen();

    init_interrupts();
    while (1) {
        display_keyboard_input();
    }
}
