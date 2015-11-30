#include "driver.h"
#include "syscall.h"
#include "memory.h"

int register_driver(int dCreateID, int dRemoveID, int dReadID, int dWriteID) {
	struct regstate state = {
			.eax = 0x300,
			.ebx = dCreateID,
			.ecx = dRemoveID,
			.edx = dReadID,
			.esi = dWriteID,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

int register_path(char* path, int driverID, int resourceID) {
	struct regstate state = {
			.eax = 0x301,
			.ebx = (uint32_t)path,
			.ecx = driverID,
			.edx = resourceID,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fCreate(char* path) {
	struct regstate state = {
			.eax = 0x302,
			.ebx = (uint32_t)path,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fRemove(char* path) {
	struct regstate state = {
			.eax = 0x303,
			.ebx = (uint32_t)path,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fWrite(char* path, uint64_t pos, uint32_t length, struct driver_data** drvData) {
	*drvData = palloc();

	(*drvData)->pos = pos;
	(*drvData)->length = length;

	struct regstate state = {
			.eax = 0x304,
			.ebx = (uint32_t)path,
			.ecx = (uint32_t)*drvData,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fRead(char* path, uint64_t pos, uint32_t length, struct driver_data** drvData) {
	*drvData = palloc();

	(*drvData)->pos = pos;
	(*drvData)->length = length;

	struct regstate state = {
			.eax = 0x305,
			.ebx = (uint32_t)path,
			.ecx = (uint32_t)*drvData,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

