#include "console.h"
#include "syscall.h"

static int x = 0;
static int y = 0;
static int kprintf_res = 0;
static char color = 0x07;

void kputc(char c) {
	struct regstate state = {
			.eax = 0x100,
			.ebx = (uint32_t) c,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void kputs(const char* s) {
    while (*s) {
        kputc(*s++);
    }
}

void kputn(unsigned long x, int base) {
    char buf[65];
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p;

    if (base > 36) {
        return;
    }

    p = buf + 64;
    *p = '\0';
    do {
        *--p = digits[x % base];
        x /= base;
    } while (x);
    kputs(p);
}

void setclr(char clr) {
    color = clr;
}

int kprintf(const char* fmt, ...) {
    va_list ap;
    const char* s;
    unsigned long n;
    char c;

    va_start(ap, fmt);
    kprintf_res = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
            case 's':
                s = va_arg(ap, char*);
                kputs(s);
                break;
            case 'd':
            case 'u':
                n = va_arg(ap, unsigned long int);
                kputn(n, 10);
                break;
            case 'x':
            case 'p':
                n = va_arg(ap, unsigned long int);
                kputn(n, 16);
                break;
            case 'c':
                c = va_arg(ap, int);
                kputc(c);
                break;
            case '%':
                kputc('%');
                break;
            case '\0':
                goto out;
            default:
                kputc('%');
                kputc(*fmt);
                break;
            }
        } else {
            kputc(*fmt);
        }

        fmt++;
    }

    out:
    va_end(ap);

    return kprintf_res;
}
