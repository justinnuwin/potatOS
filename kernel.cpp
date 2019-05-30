#include "kernel.h"

#include "vga.h"
#include "printk.h"
#include "ps2.h"
#include "keyboard.h"
#include "interrupt.h"
#include "gdt.h"
#include "serial.h"
#include "multiboot2_tags.h"
#include "page_table.h"

void wait_a_little() {
    int i = 100000;
    while (i--);
}

void wait_longer() {
    int i = 50000000;
    while (i--);
}

void kernel_main(void *multiboot2_tag) {
    setup_gdt_tss();
    init_COM1();
    if (init_interrupts())
        printk("Interrupts enabled!\n");
    if (init_ps2())
        init_keyboard();
    sti();
    read_multiboot2_tags(multiboot2_tag);
    MMU_pf_init();
    wait_longer();
    clear_screen();

    splash_screen();
    wait_longer();
    wait_longer();
    clear_screen();

    printk("Success!\n");
    char a = '\0';
    char *page = (char *)MMU_pf_alloc();
    printk("%x\n", page);
    MMU_pf_free(page);
    page = (char *)MMU_pf_alloc();
    printk("%x\n", page);
    while (1) {
        for (int i = 0; i < 4096; i++) {
                page[i] = a;
        }
        page = (char *)MMU_pf_alloc();
        if (page == 0x0)
            break;
        a = (a + 1) % 128;
    }
}
