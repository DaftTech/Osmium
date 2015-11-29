#ifndef PROCESS_H
#define PROCESS_H

#include "syscall.h"

void     _start();
void     exit(int returncode);
void*    getargsptr();

#endif
