#ifndef DRIVER_H
#define DRIVER_H

#include "stdint.h"
#include "rpc.h"

#define CALL_CREATE 0x100

int  register_driver(int dModifyID, int dInfoID, int dReadID, int dWriteID, char* drvName);
int  register_path(char* path, int driverID, int resourceID);
int  register_irq_rpc(uint32_t irqID, int rpcID);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outl(uint16_t port, uint32_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

enum driver_result {
	S_OK = 0,
	E_ERROR,
	E_PERMISSION,
	E_USER0,
	E_USER1,
	E_USER2,
	E_USER3,
	E_USER4,
	E_USER5,
	E_USER6,
	E_USER7,
	E_USER8,
	E_USER9,
	E_USER10,
	E_USER11,
	E_USER12,
	E_USER13,
	E_USER14,
	E_USER15
};

#define MAX_IO_BUFFER 3800

struct driver_data {
	uint64_t pos;
	uint32_t length;
	uint32_t bytesDone;
	enum driver_result result;

	uint8_t data[MAX_IO_BUFFER];
};

FUTURE fCall  (char* driverName, int callID, struct driver_data* drvData);
FUTURE fModify(char* path, struct driver_data* drvData);
FUTURE fWrite (char* path, struct driver_data* drvData);
FUTURE fRead  (char* path, struct driver_data* drvData);

#endif
