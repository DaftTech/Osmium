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
	PADDR phys_pdir;
	ADDRESS currentNewStackBottom;
};

struct module {
	struct environment* environment;

	ADDRESS rpc_handler_address;

	struct rpc* active_rpc;
	struct rpc_future* runningFutures;

	struct module* next; //Threads are organized as a double linked list
	struct module* prev;
};

struct rpc_future {
	int returnCode;
	FUTURE_STATE state;

	struct rpc_future* next;
};

struct rpc {
	uint32_t rpcID;
	uint32_t rpcARG0;
	PADDR data;
	void* mapped;

	uint32_t state;
	int returnCode;

	struct module* creatorModule;

	struct cpu_state cpuState;

	struct rpc_future* fullfills;
	struct rpc_future* runningFutures;

	struct rpc* next;
};

struct rpc_future*  init_rpc(struct module* t, uint32_t rpcID, uint32_t rpcARG0, PADDR data, struct module* calling);
void            	return_rpc(int resultCode);
void*               rpc_map(void);

struct environment* create_env(PADDR root);
struct module* 		register_module(struct environment* environment, ADDRESS entry);
struct module*    	get_current_thread(void);
struct module*    	get_task_by_pid(int pid);

struct cpu_state* 	terminate_current(struct cpu_state* cpu);
struct cpu_state* 	schedule_exception(struct cpu_state* cpu);
struct cpu_state*   schedule_to(struct module* next, struct cpu_state* cpu);
struct cpu_state* 	schedule(struct cpu_state* cpu);
struct cpu_state* 	save_cpu_state(struct cpu_state* cpu);

struct cpu_state*   optionForceSchedule(struct cpu_state* cpu);
void                registerForceSchedule(struct module* to);

void              	enableScheduling(void);
uint32_t          	isSchedulingEnabled(void);

#endif
