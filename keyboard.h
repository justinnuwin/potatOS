#ifndef KEYBOARD_H
#define KEYBOARD_H

#define PS2_KB_ACK                  0xfa
#define PS2_KB_RESEND               0xfe
#define PS2_KB_RESET_TEST_CMD       0xff
#define PS2_KB_TEST_SUCCESS         0xaa
#define PS2_KB_TEST_FAILURE1        0xfc
#define PS2_KB_TEST_FAILURE2        0xfd
#define PS2_KB_SCANCODE_CMD         0xf0
#define PS2_KB_GET_SCANCODE         0x0
#define PS2_KB_SET_SCANCODE1        0x1
#define PS2_KB_SET_SCANCODE2        0x2
#define PS2_KB_SET_SCANCODE3        0x3
#define PS2_KB_ENABLE_SCANNING      0xf4
#define PS2_KB_DISABLE_SCANNING     0xf5

void init_keyboard();
extern "C" void keyboard_interrupt_handler(void);

#endif
