#include "rpc.h"
#include "syscall.h"
#include "console.h"

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


void rpc_handler() {
	uint32_t rpcID;
	void* rpcData = rpc_map(&rpcID);

	kprintf("RPC with id %d\n", rpcID);

	rpc_return(0);
}
