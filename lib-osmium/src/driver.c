#include "driver.h"
#include "syscall.h"
#include "memory.h"

int register_driver(int dModifyID, int dInfoID, int dReadID, int dWriteID, char* drvName) {
	struct regstate state = {
			.eax = 0x300,
			.ebx = dModifyID,
			.ecx = dInfoID,
			.edx = dReadID,
			.esi = dWriteID,
			.edi = (uint32_t)drvName };

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

int register_irq_rpc(uint32_t irqID, int rpcID) {
	struct regstate state = {
			.eax = 0x600,
			.ebx = irqID,
			.ecx = rpcID,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

void outb(uint16_t port, uint8_t value) {
	struct regstate state = {
			.eax = 0x601,
			.ebx = port,
			.ecx = value,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void outw(uint16_t port, uint16_t value) {
	struct regstate state = {
			.eax = 0x602,
			.ebx = port,
			.ecx = value,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

void outl(uint16_t port, uint32_t value) {
	struct regstate state = {
			.eax = 0x603,
			.ebx = port,
			.ecx = value,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);
}

uint8_t inb(uint16_t port) {
	struct regstate state = {
			.eax = 0x604,
			.ebx = port,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

uint16_t inw(uint16_t port) {
	struct regstate state = {
			.eax = 0x605,
			.ebx = port,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

uint32_t inl(uint16_t port) {
	struct regstate state = {
			.eax = 0x606,
			.ebx = port,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fModify(char* path, struct driver_data* drvData) {
	struct regstate state = {
			.eax = 0x302,
			.ebx = (uint32_t)path,
			.ecx = (uint32_t)drvData,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fCall(char* driverName, int callID, struct driver_data* drvData) {
	struct regstate state = {
			.eax = 0x303,
			.ebx = (uint32_t)driverName,
			.ecx = (uint32_t)drvData,
			.edx = callID,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fWrite(char* path, struct driver_data* drvData) {
	struct regstate state = {
			.eax = 0x304,
			.ebx = (uint32_t)path,
			.ecx = (uint32_t)drvData,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

FUTURE fRead(char* path, struct driver_data* drvData) {
	struct regstate state = {
			.eax = 0x305,
			.ebx = (uint32_t)path,
			.ecx = (uint32_t)drvData,
			.edx = 0,
			.esi = 0,
			.edi = 0 };

	syscall(&state);

	return state.eax;
}

