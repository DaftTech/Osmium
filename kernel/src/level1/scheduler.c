#include "level1/scheduler.h"
#include "level0/console.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/catofdeath.h"

uint32_t schedulingEnabled = 0;

struct module* first_module = 0;
struct module* root_module = 0;
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
    struct module* rModule = calloc(1, sizeof(struct module));
    rModule->rpc_handler_address = entry;

    rModule->environment = environment;

    rModule->next = (void*) first_module;
    rModule->prev = (void*) 0;

    if (first_module != 0) {
        first_module->prev = rModule;
    }

    if(root_module == 0) {
    	root_module = rModule;
    }

    first_module = rModule;

    return rModule;
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

struct cpu_state* schedule(struct cpu_state* cpu) {
    if (first_module != 0 && schedulingEnabled) {
    	if(current_module && current_module->active_rpc && current_module->active_rpc->state == RPC_STATE_EXECUTING) {
    		memcpy(&current_module->active_rpc->cpuState, cpu, sizeof(struct cpu_state));
    	}

    	if(current_module && current_module->active_rpc == 0) {
    		current_module = 0;
    	}

    	struct module* next = first_module;

		if(current_module && current_module->next) {
			next = current_module->next;
		}

    	while(next != 0) {
			if(next->active_rpc) {
				return schedule_to(next, cpu);
			}

			next = next->next;
    	}

    	init_rpc(root_module, 0, 0x1D7E, 0, 0);

    	return schedule_to(root_module, cpu);
    }

    return cpu;
}
