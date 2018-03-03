#include "process.h"
#include "console.h"
#include "memory.h"
#include "rpc.h"
#include "driver.h"
#include "stdint.h"
#include "string.h"

#define INITRFS_MAX_FILE_COUNT 65536

struct file {
	uint32_t size;
	uint8_t content;
};

int processEvent(int arg0, void* argPtr) {
	switch(arg0) {
	case 0x00: //Module Initialized
		kprintf("initialized");
		break;

	case 0x1D7E: //IDLE
		break;
	}
}
