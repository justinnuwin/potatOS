#include "ps2.h"

#include <stdint.h>
#include "asm_functions.h"

#include "printk.h"

uint8_t ps2_poll_read() {
    uint8_t status;
    do {    // Poll for output buffer ready to read
        status = inb(PS2_CMD_PORT);
    } while (!PS2_OUTPUT_STATUS(status));
    return inb(PS2_DATA_PORT);
}

void ps2_poll_command(uint8_t command) {
    uint8_t status;
    do {    // Poll for input buffer clear to write command
        status = inb(PS2_CMD_PORT);
    } while (PS2_INPUT_STATUS(status));
    outb(PS2_CMD_PORT, command);
}

void ps2_poll_write(uint8_t command, uint8_t data) {
    ps2_poll_command(command);
    uint8_t status;
    do {    // Poll for input buffer clear to write data 
        status = inb(PS2_CMD_PORT);
    } while (PS2_INPUT_STATUS(status));
    outb(PS2_DATA_PORT, data);
}

uint8_t ps2_poll_rw(uint8_t command) {
    ps2_poll_command(command);
    return ps2_poll_read();
}

void poll_initialize_ps2() {
    // TODO: It would be a good idea to determine if PS/2 even exists
    ps2_poll_command(PS2_DISABLE_1ST_PORT);
    ps2_poll_command(PS2_DISABLE_2ND_PORT);
    uint8_t ps2_test_result = ps2_poll_rw(PS2_TEST_CONTROLLER);
    if (ps2_test_result == PS2_TEST_SUCCESS) {  // May reset controller
        ps2_poll_command(PS2_DISABLE_1ST_PORT);
        ps2_poll_command(PS2_DISABLE_2ND_PORT);
    } else if (ps2_test_result == PS2_TEST_FAILURE) {
        // printk("Error");
    } else {
        // printk("Unknown error");
    }
    uint8_t ps2_cfg = ps2_poll_rw(PS2_READ_BYTE_COMMAND(0));
    uint8_t is_2port_ps2 = PS2_CFG_2ND_PORT_CLOCK(ps2_cfg);
    ps2_cfg &= ~PS2_CFG_1ST_PORT_INT_BIT;   // Disable port 1 interrupts
    ps2_cfg &= ~PS2_CFG_2ND_PORT_INT_BIT;   // Disable port 2 interrupts
    ps2_cfg |= PS2_CFG_1ST_PORT_CLOCK_BIT;  // Enable port 1 clock
    ps2_cfg &= ~PS2_CFG_1ST_PORT_XLATE_BIT; // Disable translation
    ps2_poll_write(PS2_WRITE_BYTE_COMMAND(0), ps2_cfg);
    ps2_cfg = ps2_poll_rw(PS2_READ_BYTE_COMMAND(0));
}

void poll_initialize_ps2_keyboard() {
    uint8_t test;
    do {
        test = ps2_poll_rw(PS2_KB_RESET_TEST_CMD);
        printk("%x\n\r", test);
    } while (test == PS2_KB_RESEND);
    /*
    if (test == PS2_KB_TEST_SUCCESS)
        // YAY
    else if (test == PS2_KB_TEST_FAILURE1 || test == PS2_KB_TEST_FAILURE2)
        // printk(error)
        //
    */
    uint8_t scancode;
    do {
        scancode = ps2_poll_rw(PS2_KB_GET_SCANCODE);
    } while (scancode == PS2_KB_RESEND);
    if (scancode == PS2_KB_ACK) {
        scancode = ps2_poll_read();
        printk("%x\n\r", scancode);
        if (scancode != 1)
            ps2_poll_write(PS2_KB_SCANCODE_CMD, PS2_KB_SET_SCANCODE1);
    } // print error 
}
