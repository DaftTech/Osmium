#ifndef STDMEM_H
#define STDMEM_H

#include "stddef.h"
#include "stdmem.h"

void*        memset(void* buf, int c, size_t n);
void*        memcpy(void* dest, const void* src, size_t n);
int          memcmp(const void* ptr1, const void* ptr2, size_t num);

#endif
