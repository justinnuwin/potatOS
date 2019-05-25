#include <stdint.h>

#ifndef PS2_H
#define PS2_H

#define PS2_DATA_PORT 0x60
#define PS2_CMD_PORT  0x64

#define PS2_OUTPUT_STATUS(x) ((x & 0x1) >> 0)
#define PS2_INPUT_STATUS(x)  ((x & 0x2) >> 1)

#define PS2_READ_BYTE_COMMAND(x)  ((x + 0x20))
#define PS2_WRITE_BYTE_COMMAND(x) ((x + 0x60))
#define PS2_DISABLE_1ST_PORT 0xad
#define PS2_ENABLE_1ST_PORT  0xae
#define PS2_DISABLE_2ND_PORT 0xa7
#define PS2_ENABLE_2ND_PORT  0xa8
#define PS2_TEST_CONTROLLER  0xaa
#define PS2_TEST_SUCCESS     0x55
#define PS2_TEST_FAILURE     0xfc

#define PS2_CFG_1ST_PORT_INTERRUPT(x)   ((x & 0x1)  >> 0)
#define PS2_CFG_2ND_PORT_INTERRUPT(x)   ((x & 0x2)  >> 1)
#define PS2_CFG_SYSTEM_FLAG(x)          ((x & 0x4)  >> 2)
#define PS2_CFG_ZERO1(x)                ((x & 0x8)  >> 3)
#define PS2_CFG_1ST_PORT_CLOCK(x)       ((x & 0x10) >> 4)
#define PS2_CFG_2ND_PORT_CLOCK(x)       ((x & 0x20) >> 5)
#define PS2_CFG_1ST_PORT_XLATE(x)       ((x & 0x40) >> 6)
#define PS2_CFG_ZERO2(x)                ((x & 0x80) >> 7)
#define PS2_CFG_1ST_PORT_INT_BIT    0x1
#define PS2_CFG_2ND_PORT_INT_BIT    0x2
#define PS2_CFG_SYSTEM_FLAG_BIT     0x4
#define PS2_CFG_1ST_PORT_CLOCK_BIT  0x10
#define PS2_CFG_2ND_PORT_CLOCK_BIT  0x20
#define PS2_CFG_1ST_PORT_XLATE_BIT  0x40

uint8_t ps2_poll_read();
void ps2_poll_command(uint8_t port, uint8_t command);
int init_ps2();

#endif
