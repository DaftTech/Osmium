#include "level1/scheduler.h"
#include "level0/console.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/catofdeath.h"

uint32_t schedulingEnabled = 0;

Module* first_module = 0;
Module* root_module = 0;
Module* current_module = 0;

void enableScheduling(void) {
    schedulingEnabled = 1;
}

uint32_t isSchedulingEnabled(void) {
    return schedulingEnabled;
}

Module* getCurrentThread(void) {
    return current_module;
}

CPUState* scheduleException(CPUState* cpu) {
  if (current_module == first_module && current_module->next == 0) {
    //Only one process is running, which just crashed. Stop system.
    setclr(0x04);
    kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_module, cpu->intr, cpu->error);
    showCOD(cpu, "Last thread crashed. Terminating kernel...");

    //will never occur cause COD terminates execution
    return 0;
  } else {
    //Potential security leaks available in following code.
    setclr(0x04);
    kprintf("\n~~~ Terminated thread (PID=%x) due to exception %x:%x \n", current_module, cpu->intr, cpu->error);
    kprintf("\n");
    showDump(cpu);
    setclr(C_DEFAULT);

    return terminateCurrent(cpu);
  }
}

CPUState* terminateCurrent(CPUState* cpu) {
    Module* next = current_module->next;
    Module* prev = current_module->prev;
    Module* old = current_module;

    //vmm_destroy();
    //FIXME: Destroy environment if it has no more threads.

    if (current_module == first_module) {
        first_module = current_module->next;
    }

    if (next != 0) {
        next->prev = prev;
    }

    if (prev != 0) {
        prev->next = next;
    }

    if (next == 0) {
        next = first_module;
    }

    current_module = next;

    free(old);

    if(current_module == 0) {
        showCOD(cpu, "Last thread terminated.");
    }

    vmmActivate(current_module->environment->phys_pdir);
    return &current_module->active_rpc->cpuState;
}


Environment* createEnvironment(PADDR root) {
  Environment* rootEnv = new Environment();
  rootEnv->phys_pdir = root;
  rootEnv->currentNewStackBottom = 0xFFC00000 - THREAD_STACK_SIZE;

  return rootEnv;
}

Module* registerModule(Environment* environment, ADDRESS entry) {
  Module* rModule = new Module();
  rModule->rpc_handler_address = entry;

  rModule->environment = environment;

  rModule->next = first_module;
  rModule->prev = nullptr;

  if (first_module != 0) {
    first_module->prev = rModule;
  }

  if(root_module == 0) {
    root_module = rModule;
  }

  first_module = rModule;

  remoteCall(rModule, 0, nullptr, 0);

  return rModule;
}

CPUState* scheduleToModule(Module* next, CPUState* cpu) {
  current_module = next;
  vmmActivate(next->environment->phys_pdir);

  if(next->active_rpc->state == RPC_STATE_AWAITING) {
    next->active_rpc->state = RPC_STATE_EXECUTING;
  }

  return &(next->active_rpc->cpuState);
}

CPUState* schedule(CPUState* cpu) {
  if (first_module != 0 && schedulingEnabled) {
    if(current_module && current_module->active_rpc && current_module->active_rpc->state == RPC_STATE_EXECUTING) {
      memcpy(&current_module->active_rpc->cpuState, cpu, sizeof(CPUState));
    }

    if(current_module && current_module->active_rpc == 0) {
      current_module = 0;
    }

    Module* next = first_module;

    if(current_module && current_module->next) {
      next = current_module->next;
    }

    while(next != 0) {
    if(next->active_rpc) {
      return scheduleToModule(next, cpu);
    }

    next = next->next;
    }

    remoteCall(root_module, 0x1D7E, nullptr, 0);

    return scheduleToModule(root_module, cpu);
  }

  return cpu;
}
