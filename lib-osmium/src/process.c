#include "process.h"
#include "rpc.h"
#include "console.h"
#include "driver.h"
#include "stdlib.h"

extern int rmain(void* args);

void yield() {
	struct regstate state = {
			.eax = 0x3,
			.ebx = 0,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void _start() {
	rpc_init();

	int result = rmain(getargsptr());

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


THREAD thread(void* function, void* args) {
	struct regstate state = {
			.eax = 0x500,
			.ebx = (uint32_t)function,
			.ecx = (uint32_t)args,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

THREAD execp(char* path, void* data, uint32_t datasize) {
	struct driver_data* driverData = palloc();

	uint32_t elfsize = MAX_IO_BUFFER;
	void* elf = malloc(elfsize); //256KB for init load

	driverData->length = MAX_IO_BUFFER;

	kprintf("Reading init...\n");

	do {
		FUTURE f = fRead(path, driverData);
		while(rpc_check_future(f));

		if(driverData->pos + driverData->bytesDone >= elfsize) {
			elfsize *= 2;
			elf = realloc(elf, elfsize);
		}

		if(driverData->bytesDone) memcpy(elf + driverData->pos, driverData->data, driverData->bytesDone);

		driverData->pos += driverData->bytesDone;
	} while(driverData->bytesDone != 0);

	kprintf("Executing init from init...\n");
	THREAD ret = exec(elf, driverData->pos, data, datasize);

	pfree(driverData);
	free(elf);

	return ret;
}

THREAD execpn(char* path) {
	return execp(path, 0, 0);
}

THREAD execn(void* elf, uint32_t elfSize) {
	return exec(elf, elfSize, 0, 0);
}

THREAD exec(void* elf, uint32_t elfSize, void* data, uint32_t dataSize) {
	struct regstate state = {
			.eax = 0x501,
			.ebx = (uint32_t)data,
			.ecx = (uint32_t)dataSize,
			.edx = (uint32_t)elf,
			.esi = (uint32_t)elfSize,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}
