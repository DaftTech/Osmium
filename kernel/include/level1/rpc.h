#ifndef RPC_H_
#define RPC_H_

#define DRIVER_COUNT 1024

#include "scheduler.h"

struct driver {
	struct thread* driverThread;
	char driverName[128];
	int rpc_call;
};

void 	       fstree_init();
int 	       fstree_register_driver(int rpc_call, char* name);
struct driver* fstree_driver_for_name(char* name);

#endif
