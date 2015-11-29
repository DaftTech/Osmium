#include "level1/scheduler.h"
#include "level0/console.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/catofdeath.h"

uint32_t schedulingEnabled = 0;

struct thread* first_thread = 0;
struct thread* current_thread = 0;

int nextPID = 1; //FIXME int.max_value overflow

void enableScheduling(void) {
    schedulingEnabled = 1;
}

uint32_t isSchedulingEnabled(void) {
    return schedulingEnabled;
}

struct thread* get_current_thread(void) {
    return current_thread;
}

struct cpu_state* schedule_exception(struct cpu_state* cpu) {
    if (current_thread == first_thread && current_thread->next == 0) {
        //Only one process is running, which just crashed. Stop system.
        setclr(0x04);
        kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_thread, cpu->intr, cpu->error);
        show_cod(cpu, "Last thread crashed. Terminating kernel...");

        //will never occur cause COD terminates execution
        return 0;
    } else {
        //Potential security leaks available in following code.
        setclr(0x04);
        kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_thread, cpu->intr, cpu->error);
        kprintf("\n");
        show_dump(cpu);
        setclr(0x07);

        return terminate_current(cpu);
    }
}

struct cpu_state* terminate_current(struct cpu_state* cpu) {
    struct thread* next = current_thread->next;
    struct thread* prev = current_thread->prev;
    struct thread* old = current_thread;

    vmm_destroy();

    if (current_thread == first_thread) {
        first_thread = current_thread->next;
    }

    if (next != 0) {
        next->prev = prev;
    }

    if (prev != 0) {
        prev->next = next;
    }

    if (next == 0) {
        next = first_thread;
    }

    current_thread = next;

    free(old->cpuState);
    free(old);

    if(current_thread == 0) {
        show_cod(cpu, "Last thread terminated.");
    }

    vmm_activate_pagedir(current_thread->environment->phys_pdir);
    return current_thread->cpuState;
}


void push(struct thread* t, uint32_t value) {
    PHYSICAL rest_pdir = vmm_get_current_physical();
    vmm_activate_pagedir(t->environment->phys_pdir);

    t->cpuState->esp -= 4;

    uint32_t* espAcc = (uint32_t*)t->cpuState->esp;

    *espAcc = value;

    vmm_activate_pagedir(rest_pdir);
}

struct thread* create_thread(struct environment* environment, void* entry) {
    struct thread* nthread = calloc(1, sizeof(struct thread));
    nthread->cpuState = calloc(1, sizeof(struct cpu_state));

    nthread->environment = environment;
    nthread->user_stack_bottom = (void*) 0xFFBFF000; //0xFFC00000 - 4KB

    nthread->next = (void*) first_thread;
    nthread->prev = (void*) 0;

    if (first_thread != 0) {
        first_thread->prev = nthread;
    }

    first_thread = nthread;

    PHYSICAL rest_pdir = vmm_get_current_physical();
    vmm_activate_pagedir(environment->phys_pdir);

    for(uint8_t* addr = nthread->user_stack_bottom; (uint32_t)addr < 0xFFC00000; addr += 0x1000) {
        vmm_alloc_addr(addr, 0);
    }

    struct cpu_state nstate = { .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
            .esi = 0, .edi = 0, .ebp = 0, .esp = 0xFFC00000,
			.eip = (uint32_t) entry,

            // Ring-3-Segmentregister
            .cs = 0x18 | 0x03, .ss = 0x20 | 0x03,

            .eflags = 0x200, };

    memcpy(nthread->cpuState, &nstate, sizeof(struct cpu_state));

    vmm_activate_pagedir(rest_pdir);

    return nthread;
}

struct cpu_state* save_cpu_state(struct cpu_state* cpu) {
	memcpy(current_thread->cpuState, cpu, sizeof(struct cpu_state));
	return current_thread->cpuState;
}

struct cpu_state* schedule_to_task(struct thread* dest) {
    current_thread = dest;

    vmm_activate_pagedir(dest->environment->phys_pdir);

	return dest->cpuState;
}

static int canExecute(struct thread* t) {
	return 1;
}

struct cpu_state* schedule(struct cpu_state* cpu) {
    if (first_thread != 0 && schedulingEnabled) {
        if (current_thread == 0) {
            current_thread = first_thread;
            vmm_activate_pagedir(current_thread->environment->phys_pdir);
            return current_thread->cpuState;
        }

    	struct thread* next = current_thread;

        do {
        	next = next->next;
			if (next == 0) {
				next = first_thread;
			}
        } while(!canExecute(next));

        save_cpu_state(cpu);

        return schedule_to_task(next);
    }
    return cpu;
}
