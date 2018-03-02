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

int rmain() {
	kprintf("test\n");

	return 0;
}
