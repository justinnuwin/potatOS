#ifndef VGA_H
#define VGA_H

#include <stdint.h>

static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

enum vga_color {
	VGA_BLACK = 0,
	VGA_BLUE = 1,
	VGA_GREEN = 2,
	VGA_CYAN = 3,
	VGA_RED = 4,
	VGA_MAGENTA = 5,
	VGA_BROWN = 6,
	VGA_LIGHT_GREY = 7,
	VGA_DARK_GREY = 8,
	VGA_LIGHT_BLUE = 9,
	VGA_LIGHT_GREEN = 10,
	VGA_LIGHT_CYAN = 11,
	VGA_LIGHT_RED = 12,
	VGA_LIGHT_MAGENTA = 13,
	VGA_LIGHT_YELLOW = 14,
	VGA_WHITE = 15,
};

class VGA {
    public:
    void fill(enum vga_color color);
    void clear();
    void scroll(enum vga_color color);
    void scroll();
    void display_char(char c, enum vga_color fg, enum vga_color bg);
    void display_char_loc(int x, int y, char c, enum vga_color fg, enum vga_color bg);
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

    friend void init_VGA();
    friend void clear_screen();
    friend void fill_screen(enum vga_color color);
    friend void splash_screen();
    friend int printk(const char *fmt, ...);
    friend int atoi_display(unsigned long long abs_value);
    friend int print_decimal(int value);
    friend int atoi_base16_display(uint64_t value, char a);
    friend void VGA_display_attr_char(int x, int y, char c, enum vga_color fg, enum vga_color bg);
};

void init_VGA();
void clear_screen();
void fill_screen(enum vga_color color);
void splash_screen();
void VGA_display_attr_char(int x, int y, char c, enum vga_color fg, enum vga_color bg);

#endif
