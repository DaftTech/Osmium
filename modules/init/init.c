#include "process.h"
#include "console.h"

int main(void* initrfsPtr) {
	kprintf("%s", "Hello World\n");
	while(1);

	return initrfsPtr;
}
