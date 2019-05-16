#include "generic_isr.h"

#include "interrupt.h"
#include "printk.h"

void halt(void) {
    asm volatile ("hlt"::);
}

void generic_exception_handler(unsigned number, unsigned code) {
    switch (number) {
        case 0x0:
            printk("#DE Unhandled exception %x: Divide by zero error fault\n", number);
            break;
        case 0x1:
            printk("#DB Unhandled exception %x: Debug fault\n", number);
            break;
        case 0x2:
            printk("Unhandled non-maskable interrupt %x\n", number);
            break;
        case 0x3:
            printk("#BP Unhandled exception %x: Breakpoint fault\n", number);
            break;
        case 0x4:
            printk("#DB Unhandled exception %x: Debug fault\n", number);
            break;
        case 0x5:
            printk("#BR Unhandled exception %x: Bound Range Exceeded fault\n", number);
            break;
        case 0x6:
            printk("#UD Unhandled exception %x: Invalid Opcode fault\n", number);
            break;
        case 0x7:
            printk("#NM Unhandled exception %x: Device not available fault\n", number);
            break;
        case 0x8:
            printk("#DF Unhandled fatal exception %x: Double fault\n", number);
            halt();
            break;
        case 0x9:
            printk("#Unhandled exception %x: Coprocessor segment overrun fault\n", number);
            break;
        case 0xa:
            printk("#TS Unhandled exception %x: Invalid TSS Fault: code %u\n", number, code);
            break;
        case 0xb:
            printk("#NP Unhandled fatal exception %x: Segment not present fault: code %u\n", number, code);
            halt();
            break;
        case 0xc:
            printk("#SS Unhandled exception %x: Stack segment fault: code %u\n", number, code);
            break;
        case 0xd:
            printk("#GP Unhandled exception %x: General protection fault: code %u\n", number, code);
            halt();
            break;
        case 0xe:
            printk("#PF Unhandled exception %x: Page fault: code %u\n", number, code);
            break;
        case 0x10:
            printk("#MF Unhandled exception %x: x87 Floating-Point fault\n", number);
            break;
        case 0x11:
            printk("#AC Unhandled exception %x: Alignment check fault: code %u\n", number, code);
            break;
        case 0x12:
            printk("#MC Unhandled fatal exception %x: Machine check fault\n", number);
            halt();
            break;
        case 0x13:
            printk("#XM Unhandled exception %x: SIMD Floating-Point fault\n", number);
            break;
        case 0x14:
            printk("#VE Unhandled exception %x: Virtualization fault\n", number);
            break;
        case 0x1e:
            printk("#SX Unhandled exception %x: Security fault: code %u\n", number, code);
            break;
        default:
            printk("Unhandled exception %x\n", number);
            break;
    }
    return;
}

void generic_interrupt_handler(unsigned number) {
    printk("Unhandled interrupt %x (%u)\n", number, number);
    return;
}

void load_generic_isr() {
    register_isr(isr_wrapper_0x0, 0x0);
    register_isr(isr_wrapper_0x1, 0x1);
    register_isr(isr_wrapper_0x2, 0x2);
    register_isr(isr_wrapper_0x3, 0x3);
    register_isr(isr_wrapper_0x4, 0x4);
    register_isr(isr_wrapper_0x5, 0x5);
    register_isr(isr_wrapper_0x6, 0x6);
    register_isr(isr_wrapper_0x7, 0x7);
    register_isr(isr_wrapper_0x8, 0x8);
    register_isr(isr_wrapper_0x9, 0x9);
    register_isr(isr_wrapper_0xa, 0xa);
    register_isr(isr_wrapper_0xb, 0xb);
    register_isr(isr_wrapper_0xc, 0xc);
    register_isr(isr_wrapper_0xd, 0xd);
    register_isr(isr_wrapper_0xe, 0xe);
    register_isr(isr_wrapper_0xf, 0xf);
                                       
    register_isr(isr_wrapper_0x10, 0x10);
    register_isr(isr_wrapper_0x11, 0x11);
    register_isr(isr_wrapper_0x12, 0x12);
    register_isr(isr_wrapper_0x13, 0x13);
    register_isr(isr_wrapper_0x14, 0x14);
    register_isr(isr_wrapper_0x15, 0x15);
    register_isr(isr_wrapper_0x16, 0x16);
    register_isr(isr_wrapper_0x17, 0x17);
    register_isr(isr_wrapper_0x18, 0x18);
    register_isr(isr_wrapper_0x19, 0x19);
    register_isr(isr_wrapper_0x1a, 0x1a);
    register_isr(isr_wrapper_0x1b, 0x1b);
    register_isr(isr_wrapper_0x1c, 0x1c);
    register_isr(isr_wrapper_0x1d, 0x1d);
    register_isr(isr_wrapper_0x1e, 0x1e);
    register_isr(isr_wrapper_0x1f, 0x1f);
                                       
    register_isr(isr_wrapper_0x20, 0x20);
    register_isr(isr_wrapper_0x21, 0x21);
    register_isr(isr_wrapper_0x22, 0x22);
    register_isr(isr_wrapper_0x23, 0x23);
    register_isr(isr_wrapper_0x24, 0x24);
    register_isr(isr_wrapper_0x25, 0x25);
    register_isr(isr_wrapper_0x26, 0x26);
    register_isr(isr_wrapper_0x27, 0x27);
    register_isr(isr_wrapper_0x28, 0x28);
    register_isr(isr_wrapper_0x29, 0x29);
    register_isr(isr_wrapper_0x2a, 0x2a);
    register_isr(isr_wrapper_0x2b, 0x2b);
    register_isr(isr_wrapper_0x2c, 0x2c);
    register_isr(isr_wrapper_0x2d, 0x2d);
    register_isr(isr_wrapper_0x2e, 0x2e);
    register_isr(isr_wrapper_0x2f, 0x2f);
                                       
    register_isr(isr_wrapper_0x30, 0x30);
    register_isr(isr_wrapper_0x31, 0x31);
    register_isr(isr_wrapper_0x32, 0x32);
    register_isr(isr_wrapper_0x33, 0x33);
    register_isr(isr_wrapper_0x34, 0x34);
    register_isr(isr_wrapper_0x35, 0x35);
    register_isr(isr_wrapper_0x36, 0x36);
    register_isr(isr_wrapper_0x37, 0x37);
    register_isr(isr_wrapper_0x38, 0x38);
    register_isr(isr_wrapper_0x39, 0x39);
    register_isr(isr_wrapper_0x3a, 0x3a);
    register_isr(isr_wrapper_0x3b, 0x3b);
    register_isr(isr_wrapper_0x3c, 0x3c);
    register_isr(isr_wrapper_0x3d, 0x3d);
    register_isr(isr_wrapper_0x3e, 0x3e);
    register_isr(isr_wrapper_0x3f, 0x3f);
                                       
    register_isr(isr_wrapper_0x40, 0x40);
    register_isr(isr_wrapper_0x41, 0x41);
    register_isr(isr_wrapper_0x42, 0x42);
    register_isr(isr_wrapper_0x43, 0x43);
    register_isr(isr_wrapper_0x44, 0x44);
    register_isr(isr_wrapper_0x45, 0x45);
    register_isr(isr_wrapper_0x46, 0x46);
    register_isr(isr_wrapper_0x47, 0x47);
    register_isr(isr_wrapper_0x48, 0x48);
    register_isr(isr_wrapper_0x49, 0x49);
    register_isr(isr_wrapper_0x4a, 0x4a);
    register_isr(isr_wrapper_0x4b, 0x4b);
    register_isr(isr_wrapper_0x4c, 0x4c);
    register_isr(isr_wrapper_0x4d, 0x4d);
    register_isr(isr_wrapper_0x4e, 0x4e);
    register_isr(isr_wrapper_0x4f, 0x4f);
                                       
    register_isr(isr_wrapper_0x50, 0x50);
    register_isr(isr_wrapper_0x51, 0x51);
    register_isr(isr_wrapper_0x52, 0x52);
    register_isr(isr_wrapper_0x53, 0x53);
    register_isr(isr_wrapper_0x54, 0x54);
    register_isr(isr_wrapper_0x55, 0x55);
    register_isr(isr_wrapper_0x56, 0x56);
    register_isr(isr_wrapper_0x57, 0x57);
    register_isr(isr_wrapper_0x58, 0x58);
    register_isr(isr_wrapper_0x59, 0x59);
    register_isr(isr_wrapper_0x5a, 0x5a);
    register_isr(isr_wrapper_0x5b, 0x5b);
    register_isr(isr_wrapper_0x5c, 0x5c);
    register_isr(isr_wrapper_0x5d, 0x5d);
    register_isr(isr_wrapper_0x5e, 0x5e);
    register_isr(isr_wrapper_0x5f, 0x5f);
                                       
    register_isr(isr_wrapper_0x60, 0x60);
    register_isr(isr_wrapper_0x61, 0x61);
    register_isr(isr_wrapper_0x62, 0x62);
    register_isr(isr_wrapper_0x63, 0x63);
    register_isr(isr_wrapper_0x64, 0x64);
    register_isr(isr_wrapper_0x65, 0x65);
    register_isr(isr_wrapper_0x66, 0x66);
    register_isr(isr_wrapper_0x67, 0x67);
    register_isr(isr_wrapper_0x68, 0x68);
    register_isr(isr_wrapper_0x69, 0x69);
    register_isr(isr_wrapper_0x6a, 0x6a);
    register_isr(isr_wrapper_0x6b, 0x6b);
    register_isr(isr_wrapper_0x6c, 0x6c);
    register_isr(isr_wrapper_0x6d, 0x6d);
    register_isr(isr_wrapper_0x6e, 0x6e);
    register_isr(isr_wrapper_0x6f, 0x6f);
                                       
    register_isr(isr_wrapper_0x70, 0x70);
    register_isr(isr_wrapper_0x71, 0x71);
    register_isr(isr_wrapper_0x72, 0x72);
    register_isr(isr_wrapper_0x73, 0x73);
    register_isr(isr_wrapper_0x74, 0x74);
    register_isr(isr_wrapper_0x75, 0x75);
    register_isr(isr_wrapper_0x76, 0x76);
    register_isr(isr_wrapper_0x77, 0x77);
    register_isr(isr_wrapper_0x78, 0x78);
    register_isr(isr_wrapper_0x79, 0x79);
    register_isr(isr_wrapper_0x7a, 0x7a);
    register_isr(isr_wrapper_0x7b, 0x7b);
    register_isr(isr_wrapper_0x7c, 0x7c);
    register_isr(isr_wrapper_0x7d, 0x7d);
    register_isr(isr_wrapper_0x7e, 0x7e);
    register_isr(isr_wrapper_0x7f, 0x7f);
                                       
    register_isr(isr_wrapper_0x80, 0x80);
    register_isr(isr_wrapper_0x81, 0x81);
    register_isr(isr_wrapper_0x82, 0x82);
    register_isr(isr_wrapper_0x83, 0x83);
    register_isr(isr_wrapper_0x84, 0x84);
    register_isr(isr_wrapper_0x85, 0x85);
    register_isr(isr_wrapper_0x86, 0x86);
    register_isr(isr_wrapper_0x87, 0x87);
    register_isr(isr_wrapper_0x88, 0x88);
    register_isr(isr_wrapper_0x89, 0x89);
    register_isr(isr_wrapper_0x8a, 0x8a);
    register_isr(isr_wrapper_0x8b, 0x8b);
    register_isr(isr_wrapper_0x8c, 0x8c);
    register_isr(isr_wrapper_0x8d, 0x8d);
    register_isr(isr_wrapper_0x8e, 0x8e);
    register_isr(isr_wrapper_0x8f, 0x8f);
                                       
    register_isr(isr_wrapper_0x90, 0x90);
    register_isr(isr_wrapper_0x91, 0x91);
    register_isr(isr_wrapper_0x92, 0x92);
    register_isr(isr_wrapper_0x93, 0x93);
    register_isr(isr_wrapper_0x94, 0x94);
    register_isr(isr_wrapper_0x95, 0x95);
    register_isr(isr_wrapper_0x96, 0x96);
    register_isr(isr_wrapper_0x97, 0x97);
    register_isr(isr_wrapper_0x98, 0x98);
    register_isr(isr_wrapper_0x99, 0x99);
    register_isr(isr_wrapper_0x9a, 0x9a);
    register_isr(isr_wrapper_0x9b, 0x9b);
    register_isr(isr_wrapper_0x9c, 0x9c);
    register_isr(isr_wrapper_0x9d, 0x9d);
    register_isr(isr_wrapper_0x9e, 0x9e);
    register_isr(isr_wrapper_0x9f, 0x9f);
}

