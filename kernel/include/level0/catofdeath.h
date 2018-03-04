#ifndef CATOFDEATH_H
#define CATOFDEATH_H

//#define SCREEN_COD

#include "console.h"
#include "idt.h"

uint32_t in_cod();
void showCOD(CPUState* cpu, char* fstr);
void showDump(CPUState* cpu);

#endif
