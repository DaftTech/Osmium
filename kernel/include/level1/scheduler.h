#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "stdint.h"
#include "stdlib.h"
#include "stddef.h"
#include "level0/vmm.h"
#include "level0/idt.h"

#define HANDLE_COUNT 1024

#define PMID_STDOUT  1
#define PMID_STDIN   2
#define PMID_STDERR  3

#define RPCT_IRQ     0
#define RPCT_KERNEL  1
#define RPCT_U2U     2

#define RPCE_OK      0
#define RPCE_UNKNOWN 1
#define RPCE_NODEST  2
#define RPCE_NOFUNC  3

#define THREAD_STACK_SIZE 0x10000

struct environment {
	PHYSICAL phys_pdir;
	ADDRESS currentNewStackBottom;
};

struct thread {
	struct cpu_state* cpuState;
	struct environment* environment;

	void* argsptr;

	ADDRESS user_stack_bottom;


	struct thread* next; //Threads are organized as a double linked list
	struct thread* prev;
};


struct environment* create_env(PHYSICAL root);
struct thread* 		create_thread(struct environment* environment, void* entry);
struct thread*    	get_current_thread(void);
struct thread*    	get_task_by_pid(int pid);

void				setargsptr(struct thread* t, void* value);

struct cpu_state* 	terminate_current(struct cpu_state* cpu);
struct cpu_state* 	schedule_exception(struct cpu_state* cpu);
struct cpu_state* 	schedule_to_task(struct thread* task);
struct cpu_state* 	schedule(struct cpu_state* cpu);
struct cpu_state* 	save_cpu_state(struct cpu_state* cpu);

void              	enableScheduling(void);
uint32_t          	isSchedulingEnabled(void);

#endif
