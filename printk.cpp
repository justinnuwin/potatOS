#include "printk.h"

#include <stdarg.h>
#include <stddef.h>
#include "vga.h"

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
