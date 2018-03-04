#include "memory.h"
#include "stdmem.h"
#include "syscall.h"

void* palloc() {
	return pcalloc(1);
}

void* pcalloc(uint32_t pages) {
	RegState state = {
			.eax = 0x400,
			.ebx = pages,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	memset((void*)state.eax, 0, pages * 0x1000);

	return (void*)state.eax;
}

void pfree(void* page) {
	RegState state = {
			.eax = 0x401,
			.ebx = (uint32_t)page,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}
