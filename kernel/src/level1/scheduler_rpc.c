#include "level1/scheduler.h"
#include "level0/catofdeath.h"

struct rpc_future* init_rpc(struct module* t, uint32_t rpcID, uint32_t rpcARG0, PADDR data, struct module* calling) {
	kprintf("init_rpc(%d) for %x\n", rpcID, t);

	struct rpc* r = calloc(1, sizeof(struct rpc));

	r->next = 0;
	r->state = RPC_STATE_AWAITING;

	r->cpuState.cs = 0x18 | 0x03;
	r->cpuState.ss = 0x20 | 0x03;
	r->cpuState.esp = t->environment->currentNewStackBottom + THREAD_STACK_SIZE - 4;
	r->cpuState.eflags = 0x200;
	r->cpuState.eip = (uint32_t) t->rpc_handler_address;

    PADDR rest_pdir = vmm_get_current_physical();
    vmm_activate_pagedir(t->environment->phys_pdir);

    for(ADDRESS addr = t->environment->currentNewStackBottom;
    		addr < t->environment->currentNewStackBottom + THREAD_STACK_SIZE;
    		addr += 0x1000) {
        vmm_alloc_addr((void*)addr, 0);
    }
    t->environment->currentNewStackBottom -= THREAD_STACK_SIZE;

    vmm_activate_pagedir(rest_pdir);


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
	r->creatorModule = calling;
	r->fullfills = 0;

	if(calling) {
		struct rpc_future* future = calloc(1, sizeof(struct rpc_future));

		if(get_current_thread()->active_rpc != 0 && get_current_thread()->active_rpc->state == RPC_STATE_EXECUTING) {
			future->next = get_current_thread()->active_rpc->runningFutures;
			get_current_thread()->active_rpc->runningFutures = future;
		}
		else
		{
			future->next = get_current_thread()->runningFutures;
			get_current_thread()->runningFutures = future;
		}

		future->returnCode = 0;
		future->state = FSTATE_RUNNING;

		r->fullfills = future;
	}

	return r->fullfills;
}

void return_rpc(int resultCode) {
	struct module* creator = get_current_thread()->active_rpc->creatorModule;

	if(get_current_thread()->active_rpc->mapped != 0) vmm_unmap(get_current_thread()->active_rpc->mapped);

	get_current_thread()->active_rpc->state = RPC_STATE_RETURNED;
	get_current_thread()->active_rpc->returnCode = resultCode;

	while(get_current_thread()->active_rpc->runningFutures) {
		void* ptr = get_current_thread()->active_rpc->runningFutures;
		get_current_thread()->active_rpc->runningFutures = get_current_thread()->active_rpc->runningFutures->next;
		free(ptr);
	}

	if(get_current_thread()->active_rpc->fullfills) {
		get_current_thread()->active_rpc->fullfills->state = FSTATE_RETURNED;
		get_current_thread()->active_rpc->fullfills->returnCode = get_current_thread()->active_rpc->returnCode;
	}

	void* ptr = get_current_thread()->active_rpc;
	get_current_thread()->active_rpc = get_current_thread()->active_rpc->next;

	kprintf("set active_rpc = %x\n", get_current_thread()->active_rpc);

	free(ptr);
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
