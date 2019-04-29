#include "interrupt.h"

#include "asm_functions.h"
#include "printk.h"

#define PIC1            0x20    // IO base address for master PIC
#define PIC2            0xa0    // IO base address for slave PIC
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1 + 1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2 + 1)

#define PIC_EOI         0x20    // End of interrupt command code
#define ICW1_ICW4	    0x01	// ICW4 (not) needed
#define ICW1_SINGLE	    0x02	// Single (cascade) mode 
#define ICW1_INTERVAL4	0x04	// Call address interval 4 (8) 
#define ICW1_LEVEL	    0x08	// Level triggered (edge) mode 
#define ICW1_INIT	    0x10	// Initialization - required! 

#define ICW4_8086	    0x01	// 8086/88 (MCS-80/85) mode 
#define ICW4_AUTO	    0x02	// Auto (normal) EOI 
#define ICW4_BUF_SLAVE	0x08	// Buffered mode/slave 
#define ICW4_BUF_MASTER	0x0C	// Buffered mode/master 
#define ICW4_SFNM	    0x10	// Special fully nested (not) 

#define IST_TRAP_GATE   0xf
#define IST_INT_GATE    0xe

extern "C" void isr_wrapper(void);

void PIC_sendROI(unsigned char irq) {
    if (irq >= 8)
        outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

/*
 * arguments:
 * 	offset1 - vector offset for master PIC
 * 		vectors on the master become offset1..offset1+7
 * 	offset2 - same for slave PIC: offset2..offset2+7
 */
void PIC_remap(int offset1, int offset2) {
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	//io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	//io_wait();
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	//io_wait();
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	//io_wait();
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)

	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	//io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	//io_wait();
	outb(PIC2_DATA, ICW4_8086);
	//io_wait();
 
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);
}

void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);
}

void init_interrupts() {
    PIC_remap(0x20, 0x28);
    for (int i = 0; i < 8; i++)
        IRQ_set_mask(i);

    uint64_t isr_addr_0_15 = ((uint64_t)isr_wrapper & 0xffff);
    uint64_t isr_addr_16_31 = ((uint64_t)isr_wrapper >> 16) & 0xffff;
    uint64_t isr_addr_32_64 = ((uint64_t)isr_wrapper >> 32) & 0xffffffff;
    for (int i = 0; i < 256; i++) {
        idt[i].offset_0_15 = isr_addr_0_15;
        idt[i].offset_16_31 = isr_addr_16_31;
        idt[i].offset_32_64 = isr_addr_32_64;
        idt[i].selector = 0x8;  // TODO: why???
        idt[i].int_stack_table_idx = 0;     // 0: Don't switch stacks
        idt[i].type = IST_INT_GATE;
        idt[i].protection_level = 0;
        idt[i].present = 1;
    }

    lidt(&idt, (uint16_t)sizeof(idt) - 1);
}

