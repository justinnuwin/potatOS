#include <stdint.h>

#ifndef VGA_H
#define VGA_H

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
    void fill(enum vga_color color);
    void clear();
    void scroll(enum vga_color color);
    void scroll();
    void display_char(char c, enum vga_color fg, enum vga_color bg);
    void display_char(char c);
    void display_string(const char *str, enum vga_color fg, enum vga_color bg);
    void display_string(const char *str);
    private:
    static VGA vga;
    unsigned cursor_row;
    unsigned cursor_col;
    uint16_t *cursor_buffer;

    VGA();
    uint16_t *coord_to_addr(unsigned row, unsigned col);
    uint16_t *coord_to_addr();
    void increment_cursor(enum vga_color bg);
    void increment_cursor();

    friend void clear_screen();
    friend void fill_screen(enum vga_color color);
    friend int printk(const char *fmt, ...);
};

void clear_screen();
void fill_screen(enum vga_color color);

#endif
