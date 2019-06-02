#include <stdint.h>
#include <stdbool.h>

#ifndef INTERRUPT_H
#define INTERRUPT_H

struct InterruptDescriptor {
    uint16_t offset_0_15;
    uint16_t selector;
    uint8_t ist:2;
    uint8_t reserved0:6;
    uint8_t type:4;
    uint8_t zero:1;
    uint8_t protection_level:2;
    uint8_t present:1;
    uint16_t offset_16_31;
    uint32_t offset_32_64;
    uint32_t reserved1;
} __attribute__ ((packed));

extern "C" void PIC_sendEOI(unsigned char irq);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);
void register_isr(void (*isr_entry_pt)(void), uint8_t int_num);
bool init_interrupts();

inline void sti() {
    asm volatile ("sti" : : );
}

inline void cli() {
    asm volatile ("cli" : : );
}

#endif
