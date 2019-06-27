#include "process.h"
#include "rpc.h"
#include "console.h"
#include "stdlib.h"
#include "stdmem.h"
#include "rpc.h"

void yield() {
	RegState state = {
			.eax = 0x3,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void throwEventByName(char* name) {
    RegState state = {
            .eax = 0x4,
            .ebx = (uint32_t)name,
            .ecx = 0,
            .edx = 0,
            .esi = 0,
            .edi = 0 };

    syscall(&state);
}

THREAD execn(void* elf, uint32_t elfSize) {
	return exec(elf, elfSize, 0, 0);
}

THREAD exec(void* elf, uint32_t elfSize, void* data, uint32_t dataSize) {
	RegState state = {
			.eax = 0x501,
			.ebx = (uint32_t)data,
			.ecx = (uint32_t)dataSize,
			.edx = (uint32_t)elf,
			.esi = (uint32_t)elfSize,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}
