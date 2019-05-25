#include <stdint.h>
#include <stdbool.h>

#ifndef SERIAL_H
#define SERIAL_H

#define COM1_PORT 0x3f8
#define COM2_PORT 0x2f8

#define COM_INT_DATA_AVAIL  0x01
#define COM_INT_TX_EMPTY    0x02
#define COM_INT_BREAK_ERR   0x04
#define COM_INT_STATUS_CHNG 0x08

#define BUFFER_SIZE 4096

enum SerialState {SER_IDLE, SER_BUSY};

class Serial {
    public:
    Serial();
    Serial(uint16_t com_port_addr, uint8_t interrupt);
    void init(uint16_t com_port_addr, uint8_t interrupt);
    void write_serial(char a);
    void write_serial(const char *str);
    
    private:
    uint16_t port_num;
    char tx_buff[BUFFER_SIZE], rx_buff[BUFFER_SIZE];
    char *tx_buff_end, *rx_buff_end;
    char *tx_head, *tx_tail, *rx_head, *rx_tail;
    enum SerialState state;

    int serial_received();
    char poll_read_serial();
    int is_transmit_empty();
    void poll_write_serial(char a);
    char *tx_buff_advance(char *tx_pointer);
    void _write_serial(char a);
    void write_buffer();

    friend void serial_interrupt_handler(void);
    friend int printk(const char *fmt, ...);
};

extern Serial COM1;
void init_COM1();

#endif
