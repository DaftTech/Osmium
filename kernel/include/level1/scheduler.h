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

	struct module* next;
	struct module* prev;
};

struct rpc {
	uint32_t rpcID;
	uint32_t rpcARG0;

	uint32_t state;


	struct cpu_state cpuState;

	struct rpc* next;
};

void                remoteCall(struct module* t, uint32_t rpcID, uint32_t rpcARG0);
void            	return_rpc(int resultCode);
void*               rpc_map(void);

struct environment* create_env(PADDR root);
struct module* 		register_module(struct environment* environment, ADDRESS entry);
struct module*    	get_current_thread(void);

struct cpu_state* 	terminate_current(struct cpu_state* cpu);
struct cpu_state* 	schedule_exception(struct cpu_state* cpu);
struct cpu_state*   schedule_to(struct module* next, struct cpu_state* cpu);
struct cpu_state* 	schedule(struct cpu_state* cpu);

void              	enableScheduling(void);
uint32_t          	isSchedulingEnabled(void);

#endif
