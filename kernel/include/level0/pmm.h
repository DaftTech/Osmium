#ifndef PMM_H
#define PMM_H

#include "stdmem.h"
#include "stdint.h"
#include "multiboot.h"
#include "stddef.h"
#include "console.h"

void*     pmmAllocate();
void      pmmFree(void* addr);
void      pmmInit(MultibootInfo* mb_info);
uint32_t 	pmmGetFreeSpace(uint32_t divisor);
void      pmmPrintStats();

#endif
