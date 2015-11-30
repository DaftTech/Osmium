#include "rpc.h"
#include "syscall.h"
#include "console.h"

#define RPC_HANDLERS 1024

static int(*rpcHandlers[RPC_HANDLERS])(int, void*);

void rpc_return(int returnCode) {
	struct regstate state = {
			.eax = 0x201,
			.ebx = returnCode,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	while(1);
}

void rpc_init() {
	for(int i = 0; i < RPC_HANDLERS; i++) {
		rpcHandlers[i] = (int(*)(int, void*))0;
	}

	struct regstate state = {
			.eax = 0x202,
			.ebx = (uint32_t)&rpc_handler,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void* rpc_map(uint32_t* rpcID, uint32_t* rpcARG0) {
	struct regstate state = {
			.eax = 0x200,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	*rpcID = state.ebx;
	*rpcARG0 = state.ecx;
	return (void*) state.eax;
}


int rpc_check_future(FUTURE fut) {
	struct regstate state = {
			.eax = 0x203,
			.ebx = fut,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

int rpc_register_handler(int(*fptr)(int, void*)) {
	for(int i = 0; i < RPC_HANDLERS; i++) {
		if(rpcHandlers[i] == (int(*)(int, void*))0) {
			rpcHandlers[i] = fptr;
			return i;
		}
	}
	return -1;
}

void rpc_handler() {
	uint32_t rpcID;
	uint32_t rpcARG0;
	void* rpcData = rpc_map(&rpcID, &rpcARG0);

	int returnValue = -1;

	if(rpcID < RPC_HANDLERS && rpcHandlers[rpcID] != 0) {
		returnValue = rpcHandlers[rpcID](rpcARG0, rpcData);
	}

	rpc_return(returnValue);
}
