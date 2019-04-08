#include "vga.hpp"

#include <stddef.h>
#include <stdbool.h>
#include "string.h"

VGA::VGA() {
    cursor_row = 0;
    cursor_col = 0;
    cursor_buffer = VGA_BUFFER_BASE_ADDR;
}

inline static uint16_t
vga_char(char c, enum vga_color fg, enum vga_color bg, bool blink) {
    uint16_t res = (blink << 15) | (bg << 12) | (fg << 8) | c;
    return res;
}

void VGA::fill(enum vga_color color) {
    int count = VGA_WIDTH * VGA_HEIGHT;
    uint16_t *buffer = VGA_BUFFER_BASE_ADDR;
    while (count--) {
        *buffer = vga_char(' ', VGA_COLOR_BLACK, color, false);
        buffer++;
    }
}

void VGA::clear() {
    fill(VGA_COLOR_BLACK);
}

uint16_t *VGA::coord_to_addr(unsigned row, unsigned col) {
    if (col >= VGA_WIDTH || row >= VGA_HEIGHT)
        return NULL;
    else
        return VGA_BUFFER_BASE_ADDR + row * VGA_WIDTH + col; 
}

void VGA::scroll(enum vga_color color) {
    memcpy(coord_to_addr(0, 0), coord_to_addr(1, 0),
           VGA_WIDTH * (VGA_HEIGHT - 1) * sizeof(*VGA_BUFFER_BASE_ADDR));
    uint16_t *last_row = coord_to_addr(VGA_HEIGHT - 1, 0);
    for (int i = 0; i < VGA_WIDTH; i++)
        last_row[i] = vga_char(' ', VGA_COLOR_BLACK, color, false);
}

void VGA::scroll() {
    scroll(VGA_COLOR_BLACK);
}

void VGA::display_char(char c, enum vga_color fg, enum vga_color bg) {
    switch (c) {
        case '\n':
            if (++cursor_row >= VGA_HEIGHT)
                scroll(bg);
            break;
        case '\r':
            cursor_col = 0;
            break;
        default:
            *cursor_buffer = vga_char(c, fg, bg, false);
            break;
    }
}

void VGA::display_char(char c) {
    display_char(c, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

void VGA::increment_cursor(enum vga_color bg) {
    if (++cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT) {
        scroll(bg);
        cursor_row--;
    }
    cursor_buffer = coord_to_addr(cursor_row, cursor_col);
}

void VGA::increment_cursor() {
    increment_cursor(VGA_COLOR_BLACK);
}

void VGA::display_string(const char *str, enum vga_color fg, enum vga_color bg) {
    for (int i = 0; i < (int)strlen(str); i++) {
        display_char(str[i], fg, bg);
        switch (str[i]) {
            case '\n':
                break;
            case '\r':
                break;
            default:
                increment_cursor(bg);
                break;
        }
    }
}

void VGA::display_string(const char *str) {
    display_string(str, VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}

