#include "process.h"
#include "console.h"
#include "memory.h"
#include "rpc.h"
#include "driver.h"
#include "stdint.h"
#include "tar.h"

#define INITRFS_MAX_FILE_COUNT 65536

struct file {
	uint32_t size;
	uint8_t content;
};

static struct file* files[INITRFS_MAX_FILE_COUNT];

int dCreate(int arg0, void* data) {
	struct driver_data* drvData = data;

	drvData->result = E_ERROR;
	return 0;
}

int dRemove(int arg0, void* data) {
	struct driver_data* drvData = data;

	drvData->result = E_ERROR;
	return 0;
}

int dRead(int arg0, void* data) {
	struct driver_data* drvData = data;

	if(drvData->pos >= files[arg0]->size) {
		drvData->result = E_ERROR;
		drvData->bytesDone = 0;
		return 0;
	}

	drvData->bytesDone = (drvData->pos + drvData->length <= files[arg0]->size) ? drvData->length : (files[arg0]->size - drvData->pos);

	memcpy(drvData->data, &(files[arg0]->content) + drvData->pos, drvData->bytesDone);
	drvData->result = S_OK;

	return 0;
}

int dWrite(int arg0, void* data) {
	struct driver_data* drvData = data;

	drvData->result = E_ERROR;
	return 0;
}

void createDriver(void* initrfsPtr) {
	int dCreateID = rpc_register_handler(&dCreate);
	int dRemoveID = rpc_register_handler(&dRemove);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dCreateID, dRemoveID, dReadID, dWriteID);

	kprintf("Init registered driver %d (%d, %d, %d, %d)\n", driverID, dCreateID, dRemoveID, dReadID, dWriteID);

	tar_extract(initrfsPtr, (uint32_t**)files, driverID);

	kprintf("Done! (Doener)\n");
}

int main(void* initrfsPtr) {
	if(!initrfsPtr) {
		kprintf("Init called without initrfsptr!\nTerminating...\n");
		return 0;
	}

	createDriver(initrfsPtr);

	execpn("init");

	while(1);

	return initrfsPtr;
}
