#include "serial.h"

#include "asm_functions.h"
#include "interrupt.h"

Serial COM1;

void init_COM1() {
    COM1.init(COM1_PORT, COM_INT_STATUS_CHNG);
}

Serial::Serial() {}

Serial::Serial(uint16_t com_port_addr, uint8_t interrupt) {
    init(com_port_addr, interrupt);
}

void Serial::init(uint16_t com_port_addr, uint8_t interrupt) {
    port_num = com_port_addr;
    tx_head = tx_buff;
    tx_tail = tx_buff;  // TODO: See why lec notes do `&state->buff[0]`. Not garunteed contiguous?
    rx_head = rx_buff;
    rx_tail = rx_buff;
    tx_buff_end = tx_buff + BUFFER_SIZE;
    rx_buff_end = rx_buff + BUFFER_SIZE;
    state = SER_IDLE;

    outb(com_port_addr + 1, 0x00);    // Disable all interrupts
    outb(com_port_addr + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(com_port_addr + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(com_port_addr + 1, 0x00);    //                  (hi byte)
    outb(com_port_addr + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(com_port_addr + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(com_port_addr + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(com_port_addr + 1, interrupt);    // Set interrupts
    //register_isr(keyboard_isr_wrapper, 0x24);
    IRQ_clear_mask(4);
}

int Serial::serial_received() {
   return inb(port_num + 5) & 1;
}

// Bypasses buffer!
char Serial::poll_read_serial() {
   while (serial_received() == 0);

   return inb(port_num);
}

int Serial::is_transmit_empty() {
   return inb(port_num + 5) & 0x20;
}

// Bypasses buffer!
void Serial::poll_write_serial(char a) {
   while (is_transmit_empty() == 0);

   outb(port_num, a);
}

char *Serial::tx_buff_advance(char *tx_pointer) {
    cli();
    if (tx_pointer + 1 <= tx_buff_end)
        tx_pointer++;
    else
        tx_pointer = tx_buff;
    sti();
    return tx_pointer;
}

void Serial::write_buffer() {
    cli();
    if (state != SER_IDLE) {
        if (!is_transmit_empty()) {
            sti();
            return;
        }
    }
    state = SER_BUSY;
    while (tx_tail < tx_head) {
        poll_write_serial(*tx_tail);
        tx_tail = tx_buff_advance(tx_tail);
    }
    state = SER_IDLE;
    sti();
}

// Producer function
void Serial::_write_serial(char a) {
    *tx_head = a;
    tx_head = tx_buff_advance(tx_head);
}

void Serial::write_serial(char a) {
    _write_serial(a);
    write_buffer();
}

void Serial::write_serial(const char *str) {
    while (*str) {
        _write_serial(*str);
        str++;
    }
    write_buffer();
}

void serial_interrupt_handler(void) {
    uint8_t lcr = inb(COM1.port_num + 3);
    outb(COM1.port_num + 3, lcr & ~0x80);    // Disable DLAB (set baud rate divisor)
    uint8_t iir = inb(COM1.port_num + 2);
    uint8_t tx_empty = ((iir >> 1) & (0x1 == 0x1)) ? 1 : 0;
    uint8_t rx_available = ((iir >> 2) & (0x1 == 0x1)) ? 1 : 0;
    uint8_t lsr_change = ((iir >> 1) & (0x3 == 0x3)) ? 1 : 0; 
    if (tx_empty)
        COM1.state = SER_IDLE;
    if (lsr_change) {
        outb(COM1.port_num + 3, lcr | 0x80);    // Enable DLAB (set baud rate divisor)
        uint8_t lsr = inb(COM1.port_num + 5);
    }
}

// TODO: Implement read
