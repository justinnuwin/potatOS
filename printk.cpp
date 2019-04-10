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
        long result = abs_val / shifter;
        if (!first_digit && (shifter == 1 || result >= 1))
            first_digit = true;
        if (first_digit) {
            VGA::vga.display_char((char)result + '0'); 
            VGA::vga.increment_cursor();
            written++;
        }
        abs_val -= result;
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
                    break;
                case 'd':
                    print_decimal(va_arg(vl, int));
                    break;
                case 'u':
                    break;
                case 'x':
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
        }
        written++;
        fmt++;
    }
    va_end(vl);
    return written;
}
