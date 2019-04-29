#include <stdint.h>

#ifndef INTERRUPT_H
#define INTERRUPT_H

struct interrupt_descriptor {
    uint16_t offset_0_15;
    uint16_t selector;
    uint8_t int_stack_table_idx:2;
    uint8_t reserved0:6;
    uint8_t type:4;
    uint8_t zero:1;
    uint8_t protection_level:2;
    uint8_t present:1;
    uint16_t offset_16_31;
    uint32_t offset_32_64;
    uint32_t reserved1;
} __attribute__ ((packed));

// Global initialization of interrupt descriptor table
static struct interrupt_descriptor idt[256];

void init_interrupts();
#endif
