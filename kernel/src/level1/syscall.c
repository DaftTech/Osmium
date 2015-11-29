#include "level1/syscall.h"
#include "level0/console.h"
#include "level1/scheduler.h"

struct cpu_state* syscall(struct cpu_state* in) {
	struct cpu_state* new = in;

	switch(in->eax) {
	case 1:
		kprintf("Thread ended with %d...\n", in->ebx);
		new = terminate_current(in);
		break;


	default:
		kprintf("Terminated thread due to unhandled syscall...\n");
		new = terminate_current(in);
	}

	return new;
}
