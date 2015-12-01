#ifndef PROCESS_H
#define PROCESS_H

#include "syscall.h"

typedef uint32_t THREAD;

void     _start();
void     exit(int returncode);
void*    getargsptr();

THREAD   thread(void* function, void* args);
THREAD   exec(void* elf, uint32_t elfSize, void* data, uint32_t dataSize);
THREAD   execn(void* elf, uint32_t elfSize);
THREAD   execp(char* path, void* data, uint32_t datasize);
THREAD   execpn(char* path);

#endif
