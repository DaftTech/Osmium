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

	kprintf("REGISTERING %s as %d in %x\n", path, resourceID, registeredDrivers[driverID]);

	fstree_make_path(path, FST_DRIVER, resourceID, registeredDrivers[driverID]);
	return 0;
}

struct driver* fstree_driver_for_name(char* name) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] != 0) {
			if(!strcmp(name, registeredDrivers[i]->driverName)) return registeredDrivers[i];
		}
	}
	return 0;
}

int fstree_register_driver(int rpc_modify, int rpc_info, int rpc_read, int rpc_write, char* name) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] == 0) {
			registeredDrivers[i] = calloc(1, sizeof(struct driver));

			kprintf("Set write RPCID=%d\n", rpc_write);

			registeredDrivers[i]->rpc_modify = rpc_modify;
			registeredDrivers[i]->rpc_info = rpc_info;
			registeredDrivers[i]->rpc_read   = rpc_read;
			registeredDrivers[i]->rpc_write  = rpc_write;

			strcpy(registeredDrivers[i]->driverName, name);
			registeredDrivers[i]->driverThread = get_current_thread();
			kprintf("Set driver %x thread to %x\n", registeredDrivers[i], get_current_thread());
			return i;
		}
	}
	return -1;
}
