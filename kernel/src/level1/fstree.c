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
	fsRoot.sub.driver = (void*)0;
	fsRoot.subtype = FST_SUBNODE;
}

struct fs_node* fstree_find_path(const char* path) {
	char* cloned = strclone(path);

	struct fs_node* current = &fsRoot;

	char* tok;
	while((tok = strtok(cloned, "/"))) {
		cloned = 0;
		if(current->subtype == FST_SUBNODE) {
			current = current->sub.node;
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

struct fs_node* fstree_make_path(const char* path, uint32_t subtype, void* sub) {
	char* cloned = strclone(path);

	struct fs_node* current = &fsRoot;
	struct fs_node* parent = 0;

	char* tok;
	while((tok = strtok(cloned, "/"))) {
		cloned = 0;
		if(current->subtype == FST_SUBNODE) {
			parent = current;
			current = current->sub.node;
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
		current->next = parent->sub.node;
		parent->sub.node = current;
	}
	return current;
}

int fstree_register_driver(int rpc_open, int rpc_close, int rpc_remove, int rpc_read, int rpc_write) {
	for(int i = 0; i < DRIVER_COUNT; i++) {
		if(registeredDrivers[i] == 0) {
			registeredDrivers[i] = calloc(1, sizeof(struct driver));

			registeredDrivers[i]->rpc_open = rpc_open;
			registeredDrivers[i]->rpc_close = rpc_close;
			registeredDrivers[i]->rpc_remove = rpc_remove;
			registeredDrivers[i]->rpc_read = rpc_read;
			registeredDrivers[i]->rpc_write = rpc_write;

			registeredDrivers[i]->driverThread = get_current_thread();
			return i;
		}
	}
	return -1;
}
