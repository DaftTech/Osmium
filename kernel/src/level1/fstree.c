#include "level1/fstree.h"
#include "stdmem.h"
#include "string.h"

static struct driver* registeredDrivers[DRIVER_COUNT];
static struct fs_node fsRoot;

void fstree_init() {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		registeredDrivers[i] = (void*)0;
	}

	memset(fsRoot.name, 0, 128);
	fsRoot.next = (void*)0;
	fsRoot.sub = (void*)0;
	fsRoot.subtype = FST_SUBNODE;
}

struct fs_node* fstree_find_path(const char* path) {
	char* cloned = strclone(path);

	struct fs_node* current = &fsRoot;

	char* tok;
	while((tok = strtok(cloned, "/"))) {
		cloned = 0;
		if(current->subtype == FST_SUBNODE) {
			current = current->sub;
		}
		else
		{
			return (void*)0;
		}

		while(current != 0) {
			if(!strcmp(current->name, tok)) {
				break;
			}
			current = current->next;
		}
		if(current != 0) continue;
		return (void*)0;
	}
	return current;
}

struct fs_node* fstree_make_path(const char* path, uint32_t subtype, uint32_t resourceID, void* sub) {
	char* cloned = strclone(path);

	struct fs_node* current = &fsRoot;
	struct fs_node* parent = 0;

	char* tok;
	while((tok = strtok(cloned, "/"))) {
		cloned = 0;
		if(current->subtype == FST_SUBNODE) {
			parent = current;
			current = current->sub;
		}
		else
		{
			return (void*)0;
		}

		while(current != 0) {
			if(!strcmp(current->name, tok)) {
				break;
			}
			current = current->next;
		}
		if(current != 0) continue;

		current = calloc(1, sizeof(struct fs_node));
		strcpy(current->name, tok);
		current->next = parent->sub;
		parent->sub = current;
	}

	current->subtype = subtype;
	current->sub = sub;
	current->resourceID = resourceID;

	return current;
}

int fstree_register_path(char* path, int driverID, int resourceID) {
	void* ep = fstree_find_path(path);
	if(ep) return -1;
	if(driverID < 0 || driverID >= DRIVER_COUNT) return -2;

	fstree_make_path(path, FST_DRIVER, resourceID, registeredDrivers[driverID]);
	return 0;
}

int fstree_register_driver(int rpc_create, int rpc_remove, int rpc_read, int rpc_write) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] == 0) {
			registeredDrivers[i] = calloc(1, sizeof(struct driver));

			kprintf("Set write RPCID=%d\n", rpc_write);

			registeredDrivers[i]->rpc_create = rpc_create;
			registeredDrivers[i]->rpc_remove = rpc_remove;
			registeredDrivers[i]->rpc_read   = rpc_read;
			registeredDrivers[i]->rpc_write  = rpc_write;

			registeredDrivers[i]->driverThread = get_current_thread();
			return i;
		}
	}
	return -1;
}
