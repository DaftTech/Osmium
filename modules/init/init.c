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

int main(void* initrfsPtr) {
	int dCreateID = rpc_register_handler(&dCreate);
	int dRemoveID = rpc_register_handler(&dRemove);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dCreateID, dRemoveID, dReadID, dWriteID);

	kprintf("Init registered driver %d (%d, %d, %d, %d)\n", driverID, dCreateID, dRemoveID, dReadID, dWriteID);

	tar_extract(initrfsPtr, (uint32_t**)files, driverID);


	struct driver_data* driverData = palloc();

	driverData->length = 128;

	do {
		FUTURE f = fRead("test/print", driverData);
		while(rpc_check_future(f));
		driverData->data[driverData->bytesDone] = 0;
		kprintf("%s", driverData->data);
		driverData->pos += driverData->bytesDone;
	} while(driverData->bytesDone != 0);


	while(1);

	return initrfsPtr;
}
