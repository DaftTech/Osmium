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

	return 0;
}

int dRemove(int arg0, void* data) {

	return 0;
}

int dRead(int arg0, void* data) {
	struct driver_data* drvData = data;

	if(drvData->pos + drvData->length <= files[arg0]->size) {
		memcpy(drvData->data, &(files[arg0]->content) + drvData->pos, drvData->length);
		kprintf("[Driver] Copied %d bytes to %x", drvData->length, drvData->data);
	}

	return 0;
}

int dWrite(int arg0, void* data) {
	struct driver_data* drvData = data;

	return 0;
}

int main(void* initrfsPtr) {
	int dCreateID = rpc_register_handler(&dCreate);
	int dRemoveID = rpc_register_handler(&dRemove);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dCreateID, dRemoveID, dReadID, dWriteID);

	kprintf("Init registered driver %d (%d, %d, %d, %d)\n", driverID, dCreateID, dRemoveID, dReadID, dWriteID);

	tar_extract(initrfsPtr, files, driverID);

	struct driver_data* drvData[8];

	for(int i = 0; i < 8; i++) {
		fRead("/test/print", i*3, 3, &(drvData[i]));
	}

	for(int i = 0; i < 8; i++) {
		kprintf("File read (before wait): %s\n", drvData[i]->data);
	}

	while(rpc_check_future(0));

	for(int i = 0; i < 8; i++) {
		kprintf("File read (after wait): %s\n", drvData[i]->data);
	}


	while(1);

	return initrfsPtr;
}
