#include "rpc.h"
#include "syscall.h"
#include "console.h"

#define RPC_HANDLERS 1024

static int(*rpcHandlers[RPC_HANDLERS])(void*);

void rpc_return(int returnCode) {
	struct regstate state = {
			.eax = 0x201,
			.ebx = returnCode,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);

	while(1);
}

void rpc_init() {
	for(int i = 0; i < RPC_HANDLERS; i++) {
		rpcHandlers[i] = (int(*)(void*))0;
	}

	struct regstate state = {
			.eax = 0x202,
			.ebx = (uint32_t)&rpc_handler,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);
}

void* rpc_map(uint32_t* rpcID) {
	struct regstate state = {
			.eax = 0x200,
			.ebx = 0,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);

	*rpcID = state.ebx;
	return (void*) state.eax;
}


int rpc_check_future(FUTURE fut) {
	struct regstate state = {
			.eax = 0x203,
			.ebx = fut,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);

	return state.eax;
}

int rpc_register_handler(int(*fptr)(void*)) {
	for(int i = 0; i < RPC_HANDLERS; i++) {
		if(rpcHandlers[i] == (int(*)(void*))0) {
			rpcHandlers[i] = fptr;
			return i;
		}
	}
	return -1;
}

void rpc_handler() {
	uint32_t rpcID;
	void* rpcData = rpc_map(&rpcID);

	kprintf("HANDLED RPC!\n");

	int returnValue = -1;

	if(rpcID < RPC_HANDLERS && rpcHandlers[rpcID] != 0) {
		returnValue = rpcHandlers[rpcID](rpcData);
	}

	rpc_return(returnValue);
}
