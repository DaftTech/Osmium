#include "process.h"
#include "console.h"
#include "rpc.h"
#include "driver.h"

int dCreate(int arg0, void* data) {

	return 0;
}

int dRemove(int arg0, void* data) {

	return 0;
}

int dRead(int arg0, void* data) {
	struct driver_data* drvData = data;

	return 0;
}

int dWrite(int arg0, void* data) {
	struct driver_data* drvData = data;

	kprintf("%s\n", drvData->data);
	return 0;
}

int main(void* initrfsPtr) {
	int dCreateID = rpc_register_handler(&dCreate);
	int dRemoveID = rpc_register_handler(&dRemove);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dCreateID, dRemoveID, dReadID, dWriteID);

	kprintf("Registered driver %d (%d, %d, %d, %d)\n", driverID, dCreateID, dRemoveID, dReadID, dWriteID);

	register_path("/test", driverID, 0);

	char* testString = "Ich bin ein TestString!";

	fWrite("/test", 0, strlen(testString), testString);


	while(1);

	return initrfsPtr;
}
