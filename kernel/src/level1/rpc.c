#include "../../include/level1/rpc.h"

#include "stdmem.h"
#include "string.h"

static struct driver* registeredDrivers[DRIVER_COUNT];
static int initialized = 0;

void fstree_init() {
	if(initialized) return;

	for(int i = 0; i < DRIVER_COUNT; i++) {
		registeredDrivers[i] = (void*)0;
	}

	initialized = 1;
}

struct driver* fstree_driver_for_name(char* name) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] != 0) {
			if(!strcmp(name, registeredDrivers[i]->driverName)) return registeredDrivers[i];
		}
	}
	return 0;
}

int fstree_register_driver(int rpc_call, char* name) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] == 0) {
			registeredDrivers[i] = calloc(1, sizeof(struct driver));

			registeredDrivers[i]->rpc_call = rpc_call;

			strcpy(registeredDrivers[i]->driverName, name);
			registeredDrivers[i]->driverThread = get_current_thread();
			return i;
		}
	}
	return -1;
}
