#include "process.h"
#include "console.h"
#include "memory.h"
#include "rpc.h"
#include "driver.h"
#include "stdint.h"
#include "string.h"
#include "tar.h"

#define INITRFS_MAX_FILE_COUNT 65536

struct file {
	uint32_t size;
	uint8_t content;
};

static uint32_t state;

static struct file* files[INITRFS_MAX_FILE_COUNT];

int dModify(int resourceID, void* data) {
	kprintf("initrfs driver modify");
	return 0;
}

int dCall(int arg0, void* data) {
	state = 0;
	kprintf("initrfs driver call");
	return 0;
}

int dWrite(int resourceID, void* data) {
	return 0;
}

int dRead(int resourceID, void* data) {
	struct driver_data* drvData = data;
	if(drvData == 0) return 0;

	if(drvData->pos >= files[resourceID]->size) {
		drvData->result = E_ERROR;
		drvData->bytesDone = 0;
		return 0;
	}

	drvData->bytesDone = (drvData->pos + drvData->length <= files[resourceID]->size) ? drvData->length : (files[resourceID]->size - drvData->pos);

	memcpy(drvData->data, &(files[resourceID]->content) + drvData->pos, drvData->bytesDone);
	drvData->result = S_OK;

	return 0;
}

void createDriver(void* initrfsPtr) {
	int dModifyID = rpc_register_handler(&dModify);
	int dCallID = rpc_register_handler(&dCall);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dModifyID, dCallID, dReadID, dWriteID, "initrfs");

	tar_extract(initrfsPtr, (uint32_t**)files, driverID);
}

int main(void* initrfsPtr) {
	if(!initrfsPtr) {
		kprintf("Init called without initrfsptr!\nTerminating...\n");
		return 0;
	}

	createDriver(initrfsPtr);

	state = 5;
	execpn("drivers/fifo");
	while(state); //wait for backcall

	struct driver_data* dd = palloc();

	strcpy(dd->data, "testfile");

	FUTURE fut = fCall("fifo", CALL_CREATE, dd);
	while(rpc_check_future(fut));

	for(int i = 0; i < 100; i++) {
		strcpy(dd->data, "INPUT");
		dd->length = strlen(dd->data);
		fWrite("testfile", dd);
	}

	while(rpc_check_future(0));

	while(1) {
		dd->length = 25;
		fut = fRead("testfile", dd);
		while(rpc_check_future(fut));
		kprintf("Read: %s\n", dd->data);
	}

	return 0;
}
