#include "kernel.h"

#include "vga.h"
#include "printk.h"
#include "ps2.h"
#include "keyboard.h"
#include "interrupt.h"
#include "gdt.h"
#include "serial.h"

void wait_a_little() {
    int i = 100000;
    while (i--);
}

void wait_longer() {
    int i = 50000000;
    while (i--);
}

void kernel_main(int multiboot2_tag) {
    setup_gdt_tss();
    init_COM1();
    if (init_interrupts())
        printk("Interrupts enabled!\n");
    if (init_ps2())
        init_keyboard();
    sti();
    wait_longer();
    clear_screen();

    splash_screen();
    wait_longer();
    wait_longer();
    clear_screen();

    printk("%x\n", multiboot2_tag);
    printk("Success!\n");
    while (1) {
    }
}
