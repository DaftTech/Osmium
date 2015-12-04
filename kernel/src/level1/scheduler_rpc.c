#include "level1/scheduler.h"
#include "level0/catofdeath.h"

struct rpc_future* init_rpc(struct thread* t, uint32_t rpcID, uint32_t rpcARG0, PHYSICAL data) {
	if(t == get_current_thread() && get_current_thread()->active_rpc != 0 && get_current_thread()->active_rpc->state != RPC_STATE_AWAITING) {
		//kprintf("[DEADLOCK-CAUTION] THREAD-RPC caused SELF-RPC.\n");
	}

	struct rpc_future* future = calloc(1, sizeof(struct rpc_future));
	struct rpc* r = calloc(1, sizeof(struct rpc));

	if(get_current_thread()->active_rpc != 0 && get_current_thread()->active_rpc->state == RPC_STATE_EXECUTING) {
		future->next = get_current_thread()->active_rpc->runningFutures;
		get_current_thread()->active_rpc->runningFutures = future;
	}
	else
	{
		future->next = get_current_thread()->runningFutures;
		get_current_thread()->runningFutures = future;
	}

	r->next = 0;
	r->state = RPC_STATE_AWAITING;

	int dbgN = 0;

	if(t->active_rpc == 0) {
		t->active_rpc = r;
	}
	else
	{
		struct rpc* tr = t->active_rpc;
		do {
			dbgN++;
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
	r->rpcARG0 = rpcARG0;

	future->returnCode = 0;
	future->state = FSTATE_RUNNING;

	r->fullfills = future;

	return future;
}

void return_rpc(int resultCode) {
	if(get_current_thread()->active_rpc->mapped != 0) vmm_unmap(get_current_thread()->active_rpc->mapped);

	get_current_thread()->active_rpc->state = RPC_STATE_RETURNED;
	get_current_thread()->active_rpc->returnCode = resultCode;
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
