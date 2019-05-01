#include "keyboard.h"

#include <stdbool.h>
#include "ps2.h"
#include "interrupt.h"
#include "printk.h"

#define KEYBOARD_BUFFER_SIZE 1024

extern "C" void keyboard_isr_wrapper(void);

// Fn keys mapped from 201 - 212
const char scancode2[0x84] = {
    0, 209, 0, 205, 203, 201, 202, 212, 0, 210, 208,206, 204, '\t', '`', 0,     // 0x0 - 0xf
    0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,               // 0x10 - 0x1f
    0, 'c', 'x', 'd', 'e', '4','3', 0, 0, ' ', 'v', 'f', 't', 'r', '5', 0,      // 0x20 - 0x2f
    0, 'n', 'b', 'h', 'g', 'y', '6', 0, 0, 0, 'm', 'j', 'u', '7', '8', 0,       // 0x30 - 0x3f
    0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,     // 0x40 - 0x4f
    0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, '\n', ']', 0, '\\', 0, 0,              // 0x50 - 0x5f
    0, 0, 0, 0, 0, 0, '\b', 0, 0, '1', 0, '4', '7', 0, 0, 0,                    // 0x60 - 0x6f
    '0', '.', '2', '5', '6', '8', 0x1b, 0, 211, '+', '3', '-', '*', '9', 0, 0,  // 0x70 - 0x7f
    0, 0, 0, 207};                                                              // 0x80 - 0x83

class Keyboard {
    public:
    private:
    static Keyboard keyboard;

    Keyboard();
    volatile bool shift;
    volatile bool caps;
    volatile bool release;

    friend char parse_keyboard_scancode(uint8_t scancode);
    friend void keyboard_interrupt_handler(void);
};

Keyboard Keyboard::keyboard;

Keyboard::Keyboard() {
    shift = false;
    caps = false;
    release = false;
};

void init_keyboard() {
    uint8_t ack;
    // Reset/test keyboard
    do {
        ps2_poll_command(PS2_DATA_PORT, PS2_KB_RESET_TEST_CMD);
        ack = ps2_poll_read();
    } while (ack == PS2_KB_RESEND);
    if (ps2_poll_read() != PS2_KB_TEST_SUCCESS)
        printk("Error resetting/testing keyboard\n");
    // Set scancode to set 2
    do {
        ps2_poll_command(PS2_DATA_PORT, PS2_KB_SCANCODE_CMD);
        ack = ps2_poll_read();
    } while (ack == PS2_KB_RESEND);
    do {
        ps2_poll_command(PS2_DATA_PORT, PS2_KB_SET_SCANCODE2);
        ack = ps2_poll_read();
    } while (ack == PS2_KB_RESEND);
    register_isr(keyboard_isr_wrapper, 0x21);
    IRQ_clear_mask(1);
    printk("Successfully initialized keyboard\n");
}

char parse_keyboard_scancode(uint8_t scancode) {
    char value;
    switch (scancode) {
        case 0x12:  // Left Shift
            Keyboard::keyboard.shift = !Keyboard::keyboard.shift;
            break;
        case 0x58:  // Caps Lock
            Keyboard::keyboard.caps = !Keyboard::keyboard.caps;
            break;
        case 0x59:  // Right Shift
            Keyboard::keyboard.shift = !Keyboard::keyboard.shift;
            break;
        case 0xf0:  // Release
            Keyboard::keyboard.release = true;
            return scancode;
        default:
            value = scancode2[scancode];
            break;
    };
    if ((Keyboard::keyboard.shift || Keyboard::keyboard.caps) && value >= 'a' && value <= 'z')
        value -= ('a' - 'A');
    return value;
}

void keyboard_interrupt_handler(void) {
    char input = parse_keyboard_scancode(inb(PS2_DATA_PORT));
    if (input == 0xf0)
        int i = 1;

    if (Keyboard::keyboard.release) {
        Keyboard::keyboard.release = false;
    } else {
        if (input != 0xf0)
            printk("%c", input);
    }
}
