#include <stdbool.h>

#include "ps2.h"

class Keyboard {
    public:
    char read_input();
    private:
    static Keyboard keyboard;

    Keyboard();
    bool shift;
    bool caps;
};

Keyboard Keyboard::keyboard;

char Keyboard::read_input() {
    uint8_t scancode = ps2_poll_read();
    switch (scancode) {
        case 0x12:  // Left Shift
            shift = true;
            break;
        case 0x58:  // Caps Lock
            caps = true;
            break;
        case 0x59:  // Right Shift
            shift = true;
            break;
        case 0xF0:  // Release
            switch (ps2_poll__read()) {
                case 0x12:  // Left Shift
                    shift = false;
                    break;
                case 0x58:  // Caps Lock
                    caps = false;
                    break;
                case 0x59:  // Right Shift
                    shift = false;
                    break;
                default:
                    break;
            };
    };
            
}
