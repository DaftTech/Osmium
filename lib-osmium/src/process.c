#include "process.h"
#include "rpc.h"
#include "console.h"

extern int main(void* args);

static FUTURE testRPC() {
	struct regstate state = {
			.eax = 0x101,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

void _start() {
	rpc_init();

	int result = main(getargsptr());

	exit(result);
}

void exit(int returncode) {
	struct regstate state = {
			.eax = 0x1,
			.ebx = returncode,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	while (1) {
	}
}

void* getargsptr() {
	struct regstate state = {
			.eax = 0x2,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return (void*) state.eax;
}
