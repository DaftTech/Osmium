#include <syscall.h>
#include "rpc.h"
#include "syscall.h"
#include "console.h"

#define RPC_HANDLERS 1024

static int(*rpcHandlers[RPC_HANDLERS*2])(int);
int rpc_initialized = 0;

void rpc_return(int returnCode) {
	RegState state = {
			.eax = 0x201,
			.ebx = (uint32_t) returnCode,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	kprintf("RET RPC DIDNT WORK\n");
	while(1) {
	}
}

void rpc_map(uint32_t* rpcID, void** rpcData, uint32_t* rpcSize) {
	RegState state = {
			.eax = 0x200,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	*rpcID = state.ebx;
	*rpcData = (void*)state.ecx;
  *rpcSize = state.edx;
}

extern int processEvent(uint32_t arg0, void* data, uint32_t size);

extern "C" void _start() {
	uint32_t rpcID;
  void* rpcData;
  uint32_t rpcSize;

	rpc_map(&rpcID, &rpcData, &rpcSize);

	int returnValue = -1;

	returnValue = processEvent(rpcID, rpcData, rpcSize);

	rpc_return(returnValue);
}
