#include "printk.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include "vga.h"

int atoi_display(long abs_val) {
    int written = 0;
    long shifter = 1000000000000000000;     // Length of largest 64-bit unsigned
    bool first_digit = false;
    while (shifter) {
        long digit = abs_val / shifter;
        if (!first_digit && (shifter == 1 || digit > 0))
            first_digit = true;
        if (first_digit) {
            VGA::vga.display_char((char)digit + '0'); 
            VGA::vga.increment_cursor();
            written++;
        }
        abs_val -= digit * shifter;
        shifter /= 10;
    }
    return written;
}

int print_decimal(int value) {
    int written = 0;
    if (value < 0) {
        VGA::vga.display_char('-');
        VGA::vga.increment_cursor();
        value *= -1;
        written++;
    }
    written += atoi_display(value);
    return written;
}

// char a represents base for displaying letters in hex a:lowercase  A:uppercase
int print_hex(unsigned value, char a) {
    VGA::vga.display_string("0x");
    int written = 2;
    int index = 7;              // hex char index of 32bit value (8 indexes)
    unsigned mask = 0xf << 28;  // Assuming int is 32 bits
    bool first_char = false;
    while (mask) {
        char nibble = (value & mask) >> (index * 4);
        if (!first_char && (index == 0 || nibble > 0))
            first_char = true;
        if (nibble > 9)
            VGA::vga.display_char(nibble - 10 + a);
        else
            VGA::vga.display_char(nibble + '0');
        VGA::vga.increment_cursor();
        written++;
        index--;
        mask >>= 4;
    }
    return written;
}

int print_hex(unsigned value) {
    return print_hex(value, 'A');
}

int printk(const char *fmt, ...) {
    int written = 0;
    va_list vl;
    va_start(vl, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*(fmt + 1)) {
                case '%':
                    VGA::vga.display_char('%');
                    VGA::vga.increment_cursor();
                    written++;
                    break;
                case 'd':
                    written += print_decimal(va_arg(vl, int));
                    break;
                case 'u':
                    written += print_decimal((unsigned)va_arg(vl, unsigned));
                    break;
                case 'x':
                    written += print_hex(va_arg(vl, unsigned));
                    break;
                case 'c':
                    break;
                case 'p':
                    break;
                case 'h':
                    break;
                case 'l':
                    break;
                case 'q':
                    break;
                case 's':
                    break;
                case NULL:
                    return written;
                default:
                    break;
            }
            fmt++;
        } else {
            VGA::vga.display_char(*fmt);
            if (*fmt != '\n' && *fmt != '\r')
                VGA::vga.increment_cursor();
            written++;
        }
        fmt++;
    }
    va_end(vl);
    return written;
}
