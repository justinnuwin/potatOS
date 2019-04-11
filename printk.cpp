#include "printk.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "vga.h"
#include "string.h"

int atoi_display(unsigned long long abs_val) {
    int written = 0;
    unsigned long long shifter = 1000000000000000000;     // Length of largest 64-bit unsigned
    bool first_digit = false;
    while (shifter) {
        int digit = abs_val / shifter;
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
    written += atoi_display((uint64_t)value);
    return written;
}

// char a represents base for displaying letters in hex a:lowercase  A:uppercase
int atoi_base16_display(uint64_t value, char a) {
    VGA::vga.display_string("0x");
    int written = 2;
    int index = 15;         // hex char index of 64bit value (16 indexes)
    // uint64_t mask = 0xf << (64 - 4);
    uint64_t mask = 0xf000000000000000;
    bool first_char = false;
    while (mask) {
        char nibble = (char)((value & mask) >> (index * 4));
        if (!first_char && (index == 0 || nibble > 0))
            first_char = true;
        if (first_char) {
            if (nibble > 9)
                VGA::vga.display_char(nibble - 10 + a);
            else
                VGA::vga.display_char(nibble + '0');
            VGA::vga.increment_cursor();
            written++;
        }
        index--;
        mask >>= 4;
    }
    return written;
}

int print_hex(unsigned value, char a) {
    return atoi_base16_display((uint64_t)value, a);
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
            const char *str;
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
                    VGA::vga.display_char((char)va_arg(vl, int));   // variadic upconverts to int
                    VGA::vga.increment_cursor();
                    written++;
                    break;
                case 'p':
                    written += print_hex((uint64_t)va_arg(vl, void *));
                    break;
                case 'h':
                    break;
                case 'l':
                    break;
                case 'q':
                    break;
                case 's':
                    str = va_arg(vl, const char *);
                    VGA::vga.display_string(str);
                    written += strlen(str);
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
