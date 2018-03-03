#include "level1/syscall.h"

#include "level0/console.h"
#include "level0/catofdeath.h"
#include "level0/ports.h"
#include "level1/scheduler.h"
#include "level1/elf.h"
#include "string.h"

struct cpu_state* syscall(struct cpu_state* in) {
	struct cpu_state* new = in;

	switch(in->eax) {
	case 0x1: //exit EBX=return code
		new = terminate_current(in);
		break;

	case 0x3: //yield
		new = schedule(in);
		break;

	case 0x100: //FIXME: kputc
		setclr(COLOR(SCLR_BLACK, SCLR_LCYAN));
		kputc((char)in->ebx);
		setclr(C_DEFAULT);
		break;

	case 0x200: //RPC Map
		new->ebx = get_current_thread()->active_rpc->rpcID;
		new->ecx = get_current_thread()->active_rpc->rpcARG0;
		break;

	case 0x201: //RPC Return EBX=return code
		return_rpc(in->ebx);
		new = schedule(in);
		break;

	case 0x400: //VMM ucont alloc EBX=pages
		new->eax = (uint32_t) vmm_alloc_ucont(in->ebx);
		break;

	case 0x401: //VMM free EBX=address
		if(in->ebx < USERSPACE_BOTTOM) show_cod(in, "Userspace program tried to free kernel memory.");
		vmm_free((void*)in->ebx);
		break;

	case 0x501: //THREAD createNewContext EBX=data source* ECX=data size EDX=elf source* ESI=elf size
		new->eax = 0;
		struct environment* newEnv = create_env(vmm_create());

		if(in->edx == 0) break;
		if(in->esi == 0) break;

		if(in->ebx == 0) in->ecx = 0;

		void* dataKernel = in->ecx ? malloc(in->ecx) : 0;
		void* elfKernel = malloc(in->esi);

		if(in->ecx) memcpy(dataKernel, (void*)in->ebx, in->ecx);
		memcpy(elfKernel, (void*)in->edx, in->esi);

		struct environment* oldEnv = get_current_thread()->environment;
		vmm_activate_pagedir(newEnv->phys_pdir);

		ADDRESS entryPoint = unpack_elf(elfKernel);
		if(entryPoint == 0) goto noLoad;

		void* dataUserspace = 0;
		if(in->ecx) {
			dataUserspace = vmm_alloc_ucont(in->ecx / 0x1000);
			memcpy(dataUserspace, dataKernel, in->ecx);
		}

		struct module* t = register_module(newEnv, entryPoint);

		new->eax = (uint32_t)t;

		noLoad:
		free(dataKernel);
		free(elfKernel);
		vmm_activate_pagedir(oldEnv->phys_pdir);
		break;

	case 0x600: //Register IRQ-RPC EBX=irqID ECX=rpcID
		new->eax = register_irq_rpc(in->ebx, in->ecx);
		break;

	default:
		kprintf("Terminated thread due to unhandled syscall...\n");
		new = schedule_exception(in);
	}

	return new;
}
