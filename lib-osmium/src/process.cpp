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

THREAD execp(char* path, void* data, uint32_t datasize) {
	/*driver_data* driverData = palloc();

	uint32_t elfsize = MAX_IO_BUFFER;
	void* elf = malloc(elfsize); //256KB for init load

	driverData->length = MAX_IO_BUFFER;

	kprintf("Reading init...\n");

	do {
		FUTURE f = fCall(path, CALL_READ, driverData);
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

	return ret;*/

	return 0;
}

THREAD execpn(char* path) {
	return execp(path, 0, 0);
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
