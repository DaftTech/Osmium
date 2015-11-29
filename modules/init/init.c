#include "process.h"

int main(void* initrfsPtr) {
	while(1) tempputs((char*)initrfsPtr);

	return initrfsPtr;
}
