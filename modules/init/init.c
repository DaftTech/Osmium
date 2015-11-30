#include "process.h"
#include "console.h"

int dOpen(void* data) {

}

int dClose(void* data) {

}

int dRemove(void* data) {

}

int dRead(void* data) {

}

int dWrite(void* data) {

}

int main(void* initrfsPtr) {
	int dOpenID = rpc_register_handler(&dOpen);
	int dCloseID = rpc_register_handler(&dClose);
	int dRemoveID = rpc_register_handler(&dRemove);
	int dReadID = rpc_register_handler(&dRead);
	int dWriteID = rpc_register_handler(&dWrite);

	int driverID = register_driver(dOpenID, dCloseID, dRemoveID, dReadID, dWriteID);

	kprintf("Registered driver %d (%d, %d, %d, %d, %d)\n", driverID, dOpenID, dCloseID, dRemoveID, dReadID, dWriteID);


	while(1);

	return initrfsPtr;
}
