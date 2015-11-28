#ifndef CONSOLE_H
#define CONSOLE_H

#define SCLR_BLACK 0x0
#define SCLR_BLUE 0x1
#define SCLR_GREEN 0x2
#define SCLR_CYAN 0x3
#define SCLR_RED 0x4
#define SCLR_MAGENTA 0x5
#define SCLR_BROWN 0x6
#define SCLR_LGREY 0x7
#define SCLR_DGREY 0x8
#define SCLR_LBLUE 0x9
#define SCLR_LGREEN 0xA
#define SCLR_LCYAN 0xB
#define SCLR_LRED 0xC
#define SCLR_LMAGENTA 0xD
#define SCLR_YELLOW 0xE
#define SCLR_WHITE 0xF

#define COLOR(B, F) ((B << 4) + F)

#define C_DEFAULT COLOR(SCLR_BLACK, SCLR_WHITE)
#define C_SUCCESS COLOR(SCLR_BLACK, SCLR_LGREEN)

#include "stdarg.h"

void clrscr(void);
void setclr(char color);
int kprintf(const char* fmt, ...);

#endif
