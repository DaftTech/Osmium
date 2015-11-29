#include "process.h"

extern int main(void* args);

void _start() {
	int result = main(getargsptr());

	exit(result);
}

void exit(int returncode) {
	struct regstate state = {
			.eax = 0x1,
			.ebx = returncode,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);

	while (1) {
	}
}

void* getargsptr() {
	struct regstate state = {
			.eax = 0x2,
			.ebx = 0,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);

	return (void*) state.eax;
}

void tempputs(char* string) {
	struct regstate state = {
			.eax = 0x100,
			.ebx = (uint32_t) string,
			.ecx = 0, .edx = 0,
			.esi = 0, .edi = 0 };

	syscall(&state);
}
