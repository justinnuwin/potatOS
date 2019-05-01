#include "interrupt.h"

#include "printk.h"

extern "C" {
    void interrupt_handler() {
        printk("Hello, world!\n");
    }
}
