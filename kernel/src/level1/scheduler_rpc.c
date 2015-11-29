#include "level1/scheduler.h"

struct rpc* init_rpc(struct thread* t, uint32_t rpcID, PHYSICAL data) {
	struct rpc* r = calloc(1, sizeof(struct rpc));

	r->next = 0;
	r->state = RPC_STATE_AWAITING;

	if(t->active_rpc == 0) {
		t->active_rpc = r;
	}
	else
	{
		struct rpc* tr = t->active_rpc;
		do {
			if(tr->next == 0) {
				tr->next = r;
			}
			else
			{
				tr = tr->next;
			}
		} while(tr->next != r);
	}

	r->data = data;
	r->mapped = 0;
	r->rpcID = rpcID;

	return r;
}

void return_rpc(int resultCode) {
	if(get_current_thread()->active_rpc->mapped != 0) vmm_unmap(get_current_thread()->active_rpc->mapped);

	get_current_thread()->active_rpc->state = RPC_STATE_RETURNED;
	get_current_thread()->active_rpc->resultCode = resultCode;
}

void* rpc_map() {
	if(get_current_thread()->active_rpc->data != 0) {
		void* map = vmm_alloc_user((void*)0);
		vmm_free(map);
		vmm_map_address(map, get_current_thread()->active_rpc->data, PT_PUBLIC | PT_WRITE);

		get_current_thread()->active_rpc->mapped = map;

		return map;
	}
	return (void*)0;
}
