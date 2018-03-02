#include "level1/scheduler.h"
#include "level0/console.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/catofdeath.h"

uint32_t schedulingEnabled = 0;
uint32_t idleInitialized = 0;

struct cpu_state idleState;

struct module* first_module = 0;
struct module* current_module = 0;

void enableScheduling(void) {
    schedulingEnabled = 1;
}

uint32_t isSchedulingEnabled(void) {
    return schedulingEnabled;
}

struct module* get_current_thread(void) {
    return current_module;
}

struct cpu_state* schedule_exception(struct cpu_state* cpu) {
	if (current_module == 0) {
        kprintf("\n~~~ Kernel exception %x:%x \n", cpu->intr, cpu->error);
        kprintf("\n");
        show_cod(cpu, "Kernel panic...");

        return 0;
	}

    if (current_module == first_module && current_module->next == 0) {
        //Only one process is running, which just crashed. Stop system.
        setclr(0x04);
        kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_module, cpu->intr, cpu->error);
        show_cod(cpu, "Last thread crashed. Terminating kernel...");

        //will never occur cause COD terminates execution
        return 0;
    } else {
        //Potential security leaks available in following code.
        setclr(0x04);
        kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_module, cpu->intr, cpu->error);
        kprintf("\n");
        show_dump(cpu);
        setclr(C_DEFAULT);

        while(1) {

        };

        return terminate_current(cpu);
    }
}

struct cpu_state* terminate_current(struct cpu_state* cpu) {
	if(current_module == 0) {
		kprintf("kernel fault");

		return cpu;
	}

    struct module* next = current_module->next;
    struct module* prev = current_module->prev;
    struct module* old = current_module;

    //vmm_destroy();
    //FIXME: Destroy environment if it has no more threads.

    if (current_module == first_module) {
        first_module = current_module->next;
    }

    if (next != 0) {
        next->prev = prev;
    }

    if (prev != 0) {
        prev->next = next;
    }

    if (next == 0) {
        next = first_module;
    }

    current_module = next;

    free(old);

    if(current_module == 0) {
        show_cod(cpu, "Last thread terminated.");
    }

    vmm_activate_pagedir(current_module->environment->phys_pdir);
    return &current_module->active_rpc->cpuState;
}


struct environment* create_env(PADDR root) {
	struct environment* rootEnv = malloc(sizeof(struct environment));
	rootEnv->phys_pdir = root;
	rootEnv->currentNewStackBottom = 0xFFC00000 - THREAD_STACK_SIZE;

	return rootEnv;
}

struct module* register_module(struct environment* environment, ADDRESS entry) {
    struct module* nthread = calloc(1, sizeof(struct module));
    nthread->rpc_handler_address = entry;

    nthread->environment = environment;

    nthread->next = (void*) first_module;
    nthread->prev = (void*) 0;

    if (first_module != 0) {
        first_module->prev = nthread;
    }

    first_module = nthread;

    return nthread;
}

struct cpu_state* schedule_to(struct module* next, struct cpu_state* cpu) {
	current_module = next;
	vmm_activate_pagedir(next->environment->phys_pdir);

	struct rpc_future* bCheck = next->active_rpc->runningFutures;
	struct rpc_future** previous = &(next->active_rpc->runningFutures);

	while(bCheck != 0) {
		if(bCheck->state == FSTATE_RETURNED) {
			void* bptr = bCheck;

			*previous = bCheck->next;
			previous = &(bCheck->next);
			bCheck = bCheck->next;

			free(bptr);
		}
		else
		{
			previous = &(bCheck->next);
			bCheck = bCheck->next;
		}
	}

	if(next->active_rpc->state == RPC_STATE_AWAITING) {
		next->active_rpc->state = RPC_STATE_EXECUTING;
	}

	return &(next->active_rpc->cpuState);
}

void idleThread() {
	while(1) {
	}
}

struct cpu_state* schedule(struct cpu_state* cpu) {
    if (first_module != 0 && schedulingEnabled) {
    	if(current_module && current_module->active_rpc && current_module->active_rpc->state == RPC_STATE_EXECUTING) {
    		memcpy(&current_module->active_rpc->cpuState, cpu, sizeof(struct cpu_state));
    	}

        if (current_module == 0) {
        	if(idleInitialized) {
        		memcpy(&idleState, cpu, sizeof(struct cpu_state));
        	}
        }
        else if(current_module->active_rpc == 0) {
        	current_module = 0;
        }

    	struct module* next = first_module;

    	while(next != 0) {
			if(next->active_rpc) {
				return schedule_to(next, cpu);
			}

			next = next->next;
    	}

    	if(!idleInitialized) {
    	    struct cpu_state nstate = { .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
    	            .esi = 0, .edi = 0, .ebp = 0, .esp = (uint32_t) vmm_alloc_cont(1) + 0x1000,
    				.eip = (uint32_t) idleThread,

    	            // Ring-0-Segmentregister
    	            .cs = 0x08 | 0x00, .ss = 0x00 | 0x00,

    	            .eflags = 0x200,
    	    };

    	    memcpy(&idleState, &nstate, sizeof(struct cpu_state));

    	    idleInitialized = 1;
    	}

    	return &idleState;
    }

    return cpu;
}
