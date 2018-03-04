#ifndef STDLIB_H
#define STDLIB_H

#include "stdmem.h"

#define PAGESIZE 4096

struct MemoryNode {
    uint32_t size;
    uint32_t address;
    MemoryNode* next;
};

void* malloc(size_t size);
void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

#endif
