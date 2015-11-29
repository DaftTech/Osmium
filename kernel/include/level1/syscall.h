#ifndef SYSCALL_H_
#define SYSCALL_H_

#include "level0/idt.h"

struct cpu_state* syscall(struct cpu_state* in);

#endif
