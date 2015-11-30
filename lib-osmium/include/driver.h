#ifndef DRIVER_H
#define DRIVER_H

#include "stdint.h"
#include "rpc.h"

int register_driver(int dCreateID, int dRemoveID, int dReadID, int dWriteID);
int register_path(char* path, int driverID, int resourceID);

struct driver_data {
	uint64_t pos;
	uint32_t length;
	uint8_t data[2048];
};

FUTURE fCreate(char* path);
FUTURE fRemove(char* path);
FUTURE fWrite (char* path, uint64_t pos, uint32_t length, struct driver_data** drvData);
FUTURE fRead  (char* path, uint64_t pos, uint32_t length, struct driver_data** drvData);

#endif
