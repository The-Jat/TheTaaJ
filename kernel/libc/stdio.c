#include <stdio.h>
#include <video/boot_terminal.h>

void itoa(int value, char* str, int base) {
    char* ptr = str;
    char* ptr1 = str;
    char tmp_char;
    int tmp_value;

    if (value == 0) {
        *str++ = '0';
        *str = '\0';
        return;
    }

    while (value != 0) {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? (tmp_value + '0') : (tmp_value - 10 + 'a');
        value /= base;
    }

    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr = *ptr1;
        *ptr1 = tmp_char;
        ptr--;
        ptr1++;
    }
}


int vsprintf(char* buffer, const char* format, va_list args) {
    char* buf_ptr = buffer;
    const char* fmt_ptr = format;
    char ch;
    char tmp[32];

    while ((ch = *fmt_ptr++) != '\0') {
        if (ch != '%') {
            *buf_ptr++ = ch;
            continue;
        }

        ch = *fmt_ptr++;
        switch (ch) {
            case 'd': {
                int value = va_arg(args, int);
                itoa(value, tmp, 10);
                for (char* tmp_ptr = tmp; *tmp_ptr != '\0'; tmp_ptr++) {
                    *buf_ptr++ = *tmp_ptr;
                }
                break;
            }
            case 'x': {
                int value = va_arg(args, int);
                itoa(value, tmp, 16);
                for (char* tmp_ptr = tmp; *tmp_ptr != '\0'; tmp_ptr++) {
                    *buf_ptr++ = *tmp_ptr;
                }
                break;
            }
            case 's': {
                char* str = va_arg(args, char*);
                while (*str != '\0') {
                    *buf_ptr++ = *str++;
                }
                break;
            }
            case 'c': {
                char value = (char)va_arg(args, int);
                *buf_ptr++ = value;
                break;
            }
            default: {
                *buf_ptr++ = ch;
                break;
            }
        }
    }

    *buf_ptr = '\0';
    return buf_ptr - buffer;
}


void printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    vsprintf(buffer, format, args);

    va_end(args);

    BootTerminalPrintString(buffer);
}
