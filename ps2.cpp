#include "ps2.h"

#include "asm_functions.h"
#include "printk.h"

inline void ps2_poll_command(uint8_t port, uint8_t command) {
    uint8_t status;
    do {    // Poll for input buffer clear to write command
        status = inb(PS2_CMD_PORT);
    } while (PS2_INPUT_STATUS(status));
    outb(port, command);
}

inline uint8_t ps2_poll_read() {
    uint8_t status;
    do {    // Poll for output buffer ready to read
        status = inb(PS2_CMD_PORT);
    } while (!PS2_OUTPUT_STATUS(status));
    return inb(PS2_DATA_PORT);
}

// Returns 1 on success, 0 on failure
int init_ps2() {
    // TODO: It would be a good idea to determine if PS/2 even exists
    ps2_poll_command(PS2_CMD_PORT, PS2_DISABLE_1ST_PORT);
    ps2_poll_command(PS2_CMD_PORT, PS2_DISABLE_2ND_PORT);
    inb(PS2_DATA_PORT);     // Flush output buffer
    ps2_poll_command(PS2_CMD_PORT, PS2_READ_BYTE_COMMAND(0));
    uint8_t ps2_cfg = ps2_poll_read();
    ps2_cfg |= PS2_CFG_1ST_PORT_INT_BIT;    // Enable port 1 interrupts
    ps2_cfg |= PS2_CFG_SYSTEM_FLAG_BIT;     // Set system flag, POSTed
    ps2_cfg &= ~PS2_CFG_1ST_PORT_CLOCK_BIT; // Enable port 1 clock
    ps2_cfg &= ~PS2_CFG_1ST_PORT_XLATE_BIT; // Disable translation
    if (PS2_CFG_2ND_PORT_CLOCK(ps2_cfg)) {  // 2 port PS/2 controller
        ps2_cfg &= ~PS2_CFG_2ND_PORT_INT_BIT;   // Disable port 2 interrupts
        ps2_cfg |= PS2_CFG_2ND_PORT_CLOCK_BIT;  // Disable port 2 clock
    }
    // Poll write config to RAM byte 0
    ps2_poll_command(PS2_CMD_PORT, PS2_WRITE_BYTE_COMMAND(0));
    ps2_poll_command(PS2_DATA_PORT, ps2_cfg);
    // Test PS/2 controller
    ps2_poll_command(PS2_CMD_PORT, PS2_TEST_CONTROLLER);
    uint8_t self_test_res = ps2_poll_read();
        ps2_poll_command(PS2_CMD_PORT, PS2_READ_BYTE_COMMAND(0));
        ps2_cfg = ps2_poll_read();
    if (self_test_res != PS2_TEST_SUCCESS) {
        printk("Error initializing PS/2 controller\n");
        return 0;
    }
    ps2_poll_command(PS2_CMD_PORT, PS2_ENABLE_1ST_PORT);
    printk("Successfully initialized PS/2 controller\n");
    return 1;
}
