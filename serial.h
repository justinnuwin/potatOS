#include <stdint.h>

#ifndef SERIAL_H
#define SERIAL_H

class Serial {
    public:
    Serial(uint16_t com_port_addr);
    int serial_received();
    char read_serial();
    int is_transmit_empty();
    void write_serial(char a);
    private:
    uint16_t port_num;


};

#endif
