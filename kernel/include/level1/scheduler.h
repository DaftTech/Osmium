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

#define RPC_STATE_AWAITING 0
#define RPC_STATE_EXECUTING 1
#define RPC_STATE_RETURNED 2

#define THREAD_STACK_SIZE 0x10000

#define FSTATE_RETURNED 0
#define FSTATE_RUNNING 1

typedef uint32_t FUTURE_STATE;

struct environment {
	PHYSICAL phys_pdir;
	ADDRESS currentNewStackBottom;
};

struct thread {
	struct cpu_state* cpuState;
	struct environment* environment;

	void* argsptr;

	ADDRESS user_stack_bottom;
	ADDRESS rpc_handler_address;

	struct rpc* active_rpc;
	struct rpc_future* blockedBy;

	struct thread* next; //Threads are organized as a double linked list
	struct thread* prev;
};

struct rpc_future {
	int returnCode;
	FUTURE_STATE state;

	struct rpc_future* next;
};

struct rpc {
	uint32_t rpcID;
	PHYSICAL data;
	void* mapped;

	uint32_t state;
	int returnCode;

	struct cpu_state cpuState;

	struct rpc_future* fullfills;
	struct rpc_future* blockedBy;

	struct rpc* next;
};

struct rpc_future*  init_rpc(struct thread* t, uint32_t rpcID, PHYSICAL data);
void 				return_rpc(int resultCode);
void*               rpc_map(void);

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
