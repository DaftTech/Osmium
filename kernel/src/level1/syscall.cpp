#include "level1/syscall.h"

#include "../../../lib-common/include/cstring.h"
#include "level0/console.h"
#include "level0/catofdeath.h"
#include "level0/ports.h"
#include "level1/scheduler.h"
#include "level1/events.hpp"
#include "level1/elf.h"

CPUState* syscall(CPUState* in) {
  CPUState* newCpu = in;

  switch(in->eax) {
  case 0x1: //exit EBX=return code
  {
    newCpu = terminateCurrent(in);
    break;
  }

  case 0x3: //yield
  {
    newCpu = schedule(in);
    break;
  }

  case 0x4: //throwEventByName
  {
    char* name = (char*)in->ebx;

    Event* event = getEventByName(name);

    if(event != nullptr) {
      event->callListeners(nullptr, 0);
    }

    break;
  }

  case 0x5: //registerForEventByName
  {
    char* name = (char*)in->ebx;

    Event* event = getEventByName(name);

    if(event != nullptr) {
      event->listeners->add(getCurrentThread());
    }

    in->ebx = (uint32_t)event;

    break;
  }

        case 0x100: //FIXME: kputc
  {
    setclr(COLOR(SCLR_BLACK, SCLR_LCYAN));
    kputc((char)in->ebx);
    setclr(C_DEFAULT);
    break;
  }

  case 0x200: //RPC Map
  {
    newCpu->ebx = getCurrentThread()->active_rpc->rpcID;
    newCpu->ecx = (uint32_t)getCurrentThread()->active_rpc->rpcData;
    newCpu->edx = getCurrentThread()->active_rpc->rpcSize;
    //TODO: map data

    break;
  }

  case 0x201: //RPC Return EBX=return code
  {
    returnRPC(in->ebx);
    newCpu = schedule(in);
    break;
  }

  case 0x400: //VMM ucont alloc EBX=pages
  {
    newCpu->eax = (uint32_t) vmmAllocateInUserspaceCont(in->ebx);
    break;
  }

  case 0x401: //VMM free EBX=address
  {
    if(in->ebx < USERSPACE_BOTTOM) showCOD(in, "Userspace program tried to free kernel memory.");
    vmmFree((void*)in->ebx);
    break;
  }

  case 0x501: //THREAD createNewContext EBX=data source* ECX=data size EDX=elf source* ESI=elf size
  {
    newCpu->eax = 0;
    Environment* newEnv = createEnvironment(vmmCreate());

    if(in->edx == 0) break;
    if(in->esi == 0) break;

    if(in->ebx == 0) in->ecx = 0;

    void* dataKernel = in->ecx ? malloc(in->ecx) : 0;
    void* elfKernel = malloc(in->esi);

    if(in->ecx) memcpy(dataKernel, (void*)in->ebx, in->ecx);
    memcpy(elfKernel, (void*)in->edx, in->esi);

    Environment* oldEnv = getCurrentThread()->environment;
    vmmActivate(newEnv->phys_pdir);

    ADDRESS entryPoint = unpackELF(elfKernel);
    if(entryPoint == 0) goto noLoad;

    {
      void* dataUserspace = 0;
      if(in->ecx) {
        dataUserspace = vmmAllocateInUserspaceCont(in->ecx / 0x1000);
        memcpy(dataUserspace, dataKernel, in->ecx);
      }

      Module* t = registerModule(newEnv, entryPoint);

      newCpu->eax = (uint32_t)t;
    }

    noLoad:
    free(dataKernel);
    free(elfKernel);
    vmmActivate(oldEnv->phys_pdir);
    break;
  }

  case 0x600: //Register IRQ-RPC EBX=irqID ECX=rpcID
  {
    newCpu->eax = registerIRQRPC(in->ebx, in->ecx);
    break;
  }

  default:
  {
    kprintf("Terminated thread due to unhandled syscall...\n");
    newCpu = scheduleException(in);
  }
  }

  return newCpu;
}
