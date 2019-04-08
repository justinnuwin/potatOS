#include <stdint.h>

#ifndef VGA_H
#define VGA_H

static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static uint16_t *const VGA_BUFFER_BASE_ADDR = (uint16_t *)0xb8000;

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

class VGA {
    public:
    VGA();
//    void printk(const char *string);
    void fill(enum vga_color color);
    void clear();
    void scroll();
    void display_char(char c);
    void increment_cursor();
//    void display_string(const char *str);
    private:
    unsigned cursor_row;
    unsigned cursor_col;
    uint16_t *cursor_buffer;
    uint16_t *coord_to_addr(unsigned row, unsigned col);
};

#endif
