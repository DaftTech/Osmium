#ifndef DRIVER_H
#define DRIVER_H

#include "stdint.h"
#include "rpc.h"

int register_driver(int dCreateID, int dRemoveID, int dReadID, int dWriteID);
int register_path(char* path, int driverID, int resourceID);

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

FUTURE fCreate(char* path, struct driver_data* drvData);
FUTURE fRemove(char* path, struct driver_data* drvData);
FUTURE fWrite (char* path, struct driver_data* drvData);
FUTURE fRead  (char* path, struct driver_data* drvData);

#endif
