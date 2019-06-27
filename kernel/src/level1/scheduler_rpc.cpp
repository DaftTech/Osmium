#include "level1/scheduler.h"
#include "level0/catofdeath.h"

void remoteCall(Module* t, uint32_t rpcID, void* data, size_t size) {
  RPC* r = new RPC();

  r->next = 0;
  r->state = RPC_STATE_AWAITING;

  r->cpuState.cs = 0x18 | 0x03;
  r->cpuState.ss = 0x20 | 0x03;
  r->cpuState.esp = t->environment->currentNewStackBottom + THREAD_STACK_SIZE - 4;
  r->cpuState.eflags = 0x200;
  r->cpuState.eip = (uint32_t) t->rpc_handler_address;

  PADDR rest_pdir = vmmGetActivePhysical();
  vmmActivate(t->environment->phys_pdir);

  for(ADDRESS addr = t->environment->currentNewStackBottom; addr < t->environment->currentNewStackBottom + THREAD_STACK_SIZE;  addr += 0x1000) {
    vmmAllocateAddress((void*)addr, 0);
  }
  t->environment->currentNewStackBottom -= THREAD_STACK_SIZE;

  if(size != 0) {
    r->rpcData = vmmAllocateInUserspaceCont((size-1)/4096 + 1);
    memcpy(r->rpcData, data, size);
    r->rpcSize = size;
  }
  else {
    r->rpcData = nullptr;
    r->rpcSize = 0;
  }

  vmmActivate(rest_pdir);

  if(t->active_rpc == 0) {
    t->active_rpc = r;
  }
  else
  {
    RPC* tr = t->active_rpc;
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

  r->rpcID = rpcID;


}

void returnRPC(int resultCode) {
  void* ptr = getCurrentThread()->active_rpc;
  getCurrentThread()->active_rpc = getCurrentThread()->active_rpc->next;

  free(ptr);
}
