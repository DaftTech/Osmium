#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "level0/idt.h"

struct CPUState* syscall(struct CPUState* in);

#endif
