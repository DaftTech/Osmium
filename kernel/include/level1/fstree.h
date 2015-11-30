#ifndef FSTREE_H_
#define FSTREE_H_

#define FST_SUBNODE 0
#define FST_DRIVER  1

#define DRIVER_COUNT 1024

#include "scheduler.h"

struct driver {
	struct thread* driverThread;
	int rpc_create;
	int rpc_remove;
	int rpc_read;
	int rpc_write;
};

struct fs_node {
	char name[128];

	void* sub;

	uint32_t subtype;
	uint32_t resourceID;

	struct fs_node* next;
};

void 	fstree_init();
int 	fstree_register_driver(int rpc_create, int rpc_remove, int rpc_read, int rpc_write);
int 	fstree_register_path(char* path, int driverID, int resourceID);

struct fs_node* fstree_find_path(const char* path);
struct fs_node* fstree_make_path(const char* path, uint32_t subtype, uint32_t resourceID, void* sub);

#endif
