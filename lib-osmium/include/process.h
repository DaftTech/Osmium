#ifndef PROCESS_H
#define PROCESS_H

#include "syscall.h"

typedef uint32_t THREAD;

void     exit(int returncode);
void*    getargsptr();
void     yield();

THREAD   thread(void* function, void* args);
THREAD   exec(void* elf, uint32_t elfSize, void* data, uint32_t dataSize);
THREAD   execn(void* elf, uint32_t elfSize);

void throwEventByName(char* name);
uint32_t registerToEventByName(char* name);

#endif
