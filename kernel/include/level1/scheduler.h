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

struct Environment {
	PADDR phys_pdir;
	ADDRESS currentNewStackBottom;
};

struct RPC {
	uint32_t rpcID;
	uint32_t rpcARG0;

	uint32_t state;


	struct CPUState cpuState;

	struct RPC* next;
};

struct Module {
	Environment* environment;
	ADDRESS rpc_handler_address;

	RPC* active_rpc;

	Module* next;
	Module* prev;
};

void         	remoteCall(struct Module* t, uint32_t rpcID, uint32_t rpcARG0);
void         	returnRPC(int resultCode);
void*        	rpc_map(void);

Environment*	createEnvironment(PADDR root);
Module* 		registerModule(Environment* environment, ADDRESS entry);
Module*    		getCurrentThread(void);

CPUState* 		terminateCurrent(CPUState* cpu);
CPUState* 		scheduleException(CPUState* cpu);
CPUState*   	scheduleToModule(struct Module* next, struct CPUState* cpu);
CPUState* 		schedule(struct CPUState* cpu);

void            enableScheduling(void);
uint32_t        isSchedulingEnabled(void);

#endif
