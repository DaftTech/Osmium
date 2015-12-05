#include "process.h"
#include "console.h"
#include "memory.h"
#include "stdlib.h"
#include "rpc.h"
#include "driver.h"
#include "stdint.h"

#define FIFO_MAX_FILE_COUNT 65536
#define BUFFER_SIZE MAX_IO_BUFFER

struct file {
	uint32_t writePos;
	uint32_t readPos;
	uint8_t content[BUFFER_SIZE];
};

static struct file* files[FIFO_MAX_FILE_COUNT];
static int ownDriverID = -1;

int dModify(int resourceID, void* data) {
	struct driver_data* drvData = data;
	if(drvData == 0) return 0;

	drvData->result = E_ERROR;
	return 0;
}

int dCall(int callID, void* data) {
	struct driver_data* drvData = data;
	if(data == 0) return 0;


	kprintf("Received call!\n");

	drvData->result = E_ERROR;

	switch(callID) {
	case CALL_CREATE:
		kprintf("Received FIFO Create CALL!\n");
		;
		int fileID = -1;

		for(int i = 0; i < FIFO_MAX_FILE_COUNT; i++) {
			if(!files[i]) {
				fileID = i;
				files[i] = calloc(1, sizeof(struct file));
				break;
			}
		}

		if(fileID == -1) return 0;
		kprintf("Created FIFO %s...\n", drvData->data);
		int r = register_path((char*)drvData->data, ownDriverID, fileID);
		drvData->result = r ? E_ERROR : S_OK;
		break;

	default:
		break;
	}

	return 0;
}

int dRead(int resourceID, void* data) {
	struct driver_data* drvData = data;
	drvData->result = E_ERROR;

	struct file* file = files[resourceID];

	int available = ((file->writePos + BUFFER_SIZE) - file->readPos) % BUFFER_SIZE;
	int toRead = drvData->length;

	if(toRead > BUFFER_SIZE) return 0;
	if(toRead > available) toRead = available;

	int endPos = file->readPos + toRead;

	int readA = toRead;
	int readB = 0;

	if(endPos >= BUFFER_SIZE) {
		readA = BUFFER_SIZE - file->readPos;
		endPos = readB = endPos - BUFFER_SIZE;
	}

	memset(drvData->data, 0, MAX_IO_BUFFER);
	memcpy(drvData->data, (file->content + file->readPos), readA);
	if(readB) memcpy(drvData->data + file->readPos, file->content, readB);

	file->readPos = endPos;
	drvData->bytesDone = toRead;

	drvData->result = S_OK;

	return 0;
}

int dWrite(int resourceID, void* data) {
	struct driver_data* drvData = data;
	drvData->result = E_ERROR;

	struct file* file = files[resourceID];

	int toWrite = drvData->length;
	if(toWrite > BUFFER_SIZE) return 0;
	uint32_t writable = BUFFER_SIZE - (((file->writePos + BUFFER_SIZE) - file->readPos) % BUFFER_SIZE) - 1;
	if(toWrite > writable) {
		drvData->result = E_USER0;
		return 0;
	}

	drvData->result = S_OK;

	int endPos = toWrite + file->writePos;

	int writeA = toWrite;
	int writeB = 0;

	if(endPos >= BUFFER_SIZE) {
		writeA = BUFFER_SIZE - file->writePos;
		endPos = writeB = endPos - BUFFER_SIZE;
	}

	memcpy(file->content + file->writePos, drvData->data, writeA);
	if(writeB) memcpy(file->content, drvData->data + writeA, writeB);

	file->writePos = endPos;

	return 0;
}

void createDriver() {
	for(int i = 0; i < FIFO_MAX_FILE_COUNT; i++) {
		files[i] = 0;
	}

	int dModifyID = rpc_register_handler(&dModify);
	int dCallID = rpc_register_handler(&dCall);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	ownDriverID = register_driver(dModifyID, dCallID, dReadID, dWriteID, "fifo");

	kprintf("FIFO registered driver %d (%d, %d, %d, %d)\n", ownDriverID, dModifyID, dCallID, dReadID, dWriteID);
}

int rmain(void* initrfsPtr) {
	createDriver();

	fCall("initrfs", 0, 0); //TELL init we are finished here

	while(1) {
		yield();
	}

	return 0;
}
