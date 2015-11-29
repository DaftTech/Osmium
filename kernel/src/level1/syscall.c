#include "level1/syscall.h"
#include "level0/console.h"
#include "level1/scheduler.h"

struct cpu_state* syscall(struct cpu_state* in) {
	struct cpu_state* new = in;

	switch(in->eax) {
	case 0x1:
		kprintf("Thread ended with %d...\n", in->ebx);
		new = terminate_current(in);
		break;

	case 0x2:
		kprintf("Told %x that argsptr is %x\n", get_current_thread(), get_current_thread()->argsptr);
		new->eax = (uint32_t)get_current_thread()->argsptr;
		break;

	case 0x100:
		kputs((char*)in->ebx);
		break;

	default:
		kprintf("Terminated thread due to unhandled syscall...\n");
		new = terminate_current(in);
	}

	return new;
}
