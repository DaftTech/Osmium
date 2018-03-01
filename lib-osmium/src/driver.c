#include "driver.h"
#include "syscall.h"
#include "memory.h"

int register_driver(int dCallID, char* drvName) {
	struct regstate state = {
			.eax = 0x300,
			.ebx = dCallID,
			.ecx = 0,
			.edx = 0,
			.esi = 0,
			.edi = (uint32_t)drvName };

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
