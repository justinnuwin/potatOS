#include "keyboard.h"

#include <stdbool.h>
#include "ps2.h"


#include "printk.h"

#define KEYBOARD_BUFFER_SIZE 1024

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
    bool shift;
    bool caps;

    friend char keyboard_read_input();
};

Keyboard Keyboard::keyboard;

Keyboard::Keyboard() {
    shift = false;
    caps = false;
};

char keyboard_read_input() {
    uint8_t scancode = ps2_poll_read();
    char value;
    switch (scancode) {
        case 0x12:  // Left Shift
            Keyboard::keyboard.shift = true;
            break;
        case 0x58:  // Caps Lock
            Keyboard::keyboard.caps = true;
            break;
        case 0x59:  // Right Shift
            Keyboard::keyboard.shift = true;
            break;
        case 0xF0:  // Release
            switch (ps2_poll_read()) {
                case 0x12:  // Left Shift
                    Keyboard::keyboard.shift = false;
                    break;
                case 0x58:  // Caps Lock
                    Keyboard::keyboard.caps = false;
                    break;
                case 0x59:  // Right Shift
                    Keyboard::keyboard.shift = false;
                    break;
                default:
                    scancode = 0;
                    value = 0;
                    break;
            };
            break;
        default:
            value = scancode2[scancode];
            break;
    };
    if ((Keyboard::keyboard.shift || Keyboard::keyboard.caps) && value >= 'a' && value <= 'z')
        value -= ('a' - 'A');
    return value;
}

void display_keyboard_input() {
    char input = keyboard_read_input();
    if (input == '\n')
        printk("\n");
    else if (input)
        printk("%c", input);
}
        
