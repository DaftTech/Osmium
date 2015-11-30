#include "level1/scheduler.h"
#include "level0/console.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/catofdeath.h"

uint32_t schedulingEnabled = 0;

struct thread* first_thread = 0;
struct thread* current_thread = 0;

void enableScheduling(void) {
    schedulingEnabled = 1;

	while (1) {
	}
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

    //vmm_destroy();
    //FIXME: Destroy environment if it has no more threads.

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


void setargsptr(struct thread* t, void* value) {
    t->argsptr = value;
}

struct environment* create_env(PHYSICAL root) {
	struct environment* rootEnv = malloc(sizeof(struct environment));
	rootEnv->phys_pdir = root;
	rootEnv->currentNewStackBottom = 0xFFC00000 - THREAD_STACK_SIZE;

	return rootEnv;
}

struct thread* create_thread(struct environment* environment, void* entry) {
    struct thread* nthread = calloc(1, sizeof(struct thread));
    nthread->cpuState = calloc(1, sizeof(struct cpu_state));

    nthread->environment = environment;
    nthread->user_stack_bottom = environment->currentNewStackBottom; //0xFFC00000 - 4KB
    environment->currentNewStackBottom -= THREAD_STACK_SIZE;

    nthread->next = (void*) first_thread;
    nthread->prev = (void*) 0;

    if (first_thread != 0) {
        first_thread->prev = nthread;
    }

    first_thread = nthread;

    PHYSICAL rest_pdir = vmm_get_current_physical();
    vmm_activate_pagedir(environment->phys_pdir);

    for(ADDRESS addr = nthread->user_stack_bottom; (uint32_t)addr < nthread->user_stack_bottom + THREAD_STACK_SIZE; addr += 0x1000) {
        vmm_alloc_addr((void*)addr, 0);
    }

    struct cpu_state nstate = { .eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
            .esi = 0, .edi = 0, .ebp = 0, .esp = nthread->user_stack_bottom + THREAD_STACK_SIZE - 4,
			.eip = (uint32_t) entry,

            // Ring-3-Segmentregister
            .cs = 0x18 | 0x03, .ss = 0x20 | 0x03,

            .eflags = 0x200, };

    memcpy(nthread->cpuState, &nstate, sizeof(struct cpu_state));

    vmm_activate_pagedir(rest_pdir);

    return nthread;
}

struct cpu_state* save_cpu_state(struct cpu_state* cpu) {
	if(current_thread->active_rpc != 0 && current_thread->active_rpc->state != RPC_STATE_AWAITING) {
		memcpy(&current_thread->active_rpc->cpuState, cpu, sizeof(struct cpu_state));
	}
	else
	{
		memcpy(current_thread->cpuState, cpu, sizeof(struct cpu_state));
	}
	return cpu;
}

struct cpu_state* schedule(struct cpu_state* cpu) {
    if (first_thread != 0 && schedulingEnabled) {
        if (current_thread == 0) {
            current_thread = first_thread;
            vmm_activate_pagedir(current_thread->environment->phys_pdir);
            return current_thread->cpuState;
        }

        save_cpu_state(cpu);

    	struct thread* next = current_thread;

        do {
        	next = next->next;
			if (next == 0) {
				next = first_thread;
			}

			current_thread = next;
			vmm_activate_pagedir(next->environment->phys_pdir);

			if(next->active_rpc) {
				if(next->active_rpc->state == RPC_STATE_RETURNED)
				{
					kprintf("Closed returned RPC thread\n");

					next->active_rpc->fullfills->state = FSTATE_RETURNED;
					next->active_rpc->fullfills->returnCode = next->active_rpc->returnCode;

					void* ptr = next->active_rpc;
					next->active_rpc = next->active_rpc->next;

					free(ptr);
				}
			}

			if(next->active_rpc) {
				if(next->active_rpc->state == RPC_STATE_AWAITING) {
					struct cpu_state nstate = {
							.eax = 0, .ebx = 0, .ecx = 0, .edx = 0,
							.esi = 0, .edi = 0, .ebp = 0, .esp = next->cpuState->esp - 4,
							.eip = (uint32_t) next->rpc_handler_address,

							// Ring-3-Segmentregister
							.cs = 0x18 | 0x03, .ss = 0x20 | 0x03,

							.eflags = 0x200, };

					memcpy(&(next->active_rpc->cpuState), &nstate, sizeof(struct cpu_state));
					next->active_rpc->state = RPC_STATE_EXECUTING;
				}

				struct rpc_future* bCheck = next->active_rpc->blockedBy;
				struct rpc_future** previous = &(next->active_rpc->blockedBy);

				while(bCheck != 0) {
					if(bCheck->state == FSTATE_RETURNED) {
						kprintf("[SCHEDTO] RPC block-release...\n");

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

				return &(next->active_rpc->cpuState);
			}
			else
			{
				struct rpc_future* bCheck = next->blockedBy;
				struct rpc_future** previous = &(next->blockedBy);

				while(bCheck != 0) {
					if(bCheck->state == FSTATE_RETURNED) {
						kprintf("[SCHEDTO] THREAD block-release...\n");

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

				return next->cpuState;
			}
			kprintf("[NOSCHED]");
        } while(1);
    }
    return cpu;
}
