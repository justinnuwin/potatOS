#include "vga.hpp"

#include <stdbool.h>
//#include "string.h"

VGA::VGA() {
    cursor_row = 0;
    cursor_col = 0;
    cursor_buffer = VGA_ADDR;
}

inline static uint16_t
vga_char(char c, enum vga_color fg, enum vga_color bg, bool blink) {
    uint16_t res = (blink << 15) | (bg << 12) | (fg << 8) | c;
    return res;
}

void VGA::clear() {
    int count = VGA_WIDTH * VGA_HEIGHT;
    uint16_t *buffer = VGA_ADDR;
    while (count--) {
        *buffer = vga_char(' ', VGA_COLOR_BLACK, VGA_COLOR_BLACK, false);
        buffer++;
    }
}

/*
void VGA::scroll() {
    memcpy(coord_to_addr(0, 0), coord_to_addr(1, 0), VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*VGA_ADDR));
}


void VGA::display_char(char c) {
    switch c {
        case '\n':
            current
    *current_buffer = vga_char(c, VGA_COLOR_WHITE, VGA_COLOR_BLACK, false);
}

uint16_t *VGA::coord_to_addr(unsigned row, unsigned col) {
    if (col > VGA_WIDTH || row > VGA_HEIGHT)
        return NULL;
    return VGA_ADDR + row * VGA_HEIGHT + col; 
}

void VGA::increment_cursor() {
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT) {
        scroll();
        cursor_row = 0;
    }
}


void VGA::newline() {

    */
