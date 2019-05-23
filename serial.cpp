#include "serial.h"

#include "asm_functions.h"

#define COM1_PORT 0x3f8
#define COM2_PORT 0x2f8

Serial::Serial(uint16_t com_port_addr) {
    port_num = com_port_addr;
    outb(com_port_addr + 1, 0x00);    // Disable all interrupts
    outb(com_port_addr + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(com_port_addr + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(com_port_addr + 1, 0x00);    //                  (hi byte)
    outb(com_port_addr + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(com_port_addr + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(com_port_addr + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

int Serial::serial_received() {
   return inb(port_num + 5) & 1;
}

char Serial::read_serial() {
   while (serial_received() == 0);

   return inb(port_num);
}

int Serial::is_transmit_empty() {
   return inb(port_num + 5) & 0x20;
}

void Serial::write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(port_num ,a);
}
