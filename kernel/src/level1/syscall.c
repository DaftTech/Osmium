#include "level1/syscall.h"
#include "level0/console.h"
#include "level0/catofdeath.h"
#include "level0/ports.h"
#include "level1/scheduler.h"
#include "level1/fstree.h"
#include "level1/elf.h"

struct cpu_state* syscall(struct cpu_state* in) {
	struct cpu_state* new = in;

	switch(in->eax) {
	case 0x1: //exit EBX=return code
		new = terminate_current(in);
		break;

	case 0x2: //get argsptr
		new->eax = (uint32_t)get_current_thread()->argsptr;
		break;

	case 0x100: //FIXME: kputc
		setclr(COLOR(SCLR_BLACK, SCLR_LCYAN));
		kputc((char)in->ebx);
		setclr(C_DEFAULT);
		break;

	case 0x200: //RPC Map
		new->eax = (uint32_t)rpc_map();
		new->ebx = get_current_thread()->active_rpc->rpcID;
		new->ecx = get_current_thread()->active_rpc->rpcARG0;
		break;

	case 0x201: //RPC Return EBX=return code
		return_rpc(in->ebx);
		new = schedule(in);
		break;

	case 0x202: //RPC Register Handler EBX=handler function*
		get_current_thread()->rpc_handler_address = in->ebx;
		break;

	case 0x203: //RPC rpc_check_future EBX=future
		;
		uint32_t ebx = in->ebx;

		struct thread* c = get_current_thread();
		struct rpc_future* bCheck = (c->active_rpc && c->active_rpc->state != RPC_STATE_AWAITING) ? c->active_rpc->runningFutures : c->runningFutures;

		uint32_t anyFuture = 0;

		while(bCheck != 0) {
			if(ebx == 1337) anyFuture |= bCheck->state;
			if((uint32_t)bCheck == ebx) {
				anyFuture = bCheck->state;
				break;
			}

			bCheck = bCheck->next;
		}

		new->eax = anyFuture;
		break;


	case 0x300: //Register driver EBX=modify ECX=info EDX=read ESI=write
		new->eax = fstree_register_driver(in->ebx, in->ecx, in->edx, in->esi, (char*)in->edi);
		break;

	case 0x301: //Register path EBX=path char* ECX=driverID EDX=resourceID
		new->eax = fstree_register_path((char*)in->ebx, in->ecx, in->edx);
		break;

	case 0x302: //DRVCall modify EBX=path char* ECX=data page*
	{
		char* path = (char*)in->ebx;
		struct fs_node* node = fstree_find_path(path);
		if(node != 0) {
			struct driver* d = node->sub;
			new->eax = (uint32_t)init_rpc(d->driverThread, d->rpc_modify, node->resourceID, vmm_resolve((void*)in->ecx), get_current_thread());
		}
		else
		{
			new->eax = 0;
		}
	}
		break;

	case 0x303: //DRVCall call EBX=name char* ECX=data page* EDX=callID
	{
		char* name = (char*)in->ebx;
		struct driver* d = fstree_driver_for_name(name);
		if(d != 0) {
			new->eax = (uint32_t)init_rpc(d->driverThread, d->rpc_info, in->edx, vmm_resolve((void*)in->ecx), get_current_thread());
		}
		else
		{
			new->eax = 0;
		}
	}
		break;

	case 0x304: //DRVCall write EBX=path char* ECX=data page*
	{
		char* path = (char*)in->ebx;
		struct fs_node* node = fstree_find_path(path);
		if(node != 0 && node->subtype == FST_DRIVER) {
			struct driver* d = node->sub;
			new->eax = (uint32_t)init_rpc(d->driverThread, d->rpc_write, node->resourceID, vmm_resolve((void*)in->ecx), get_current_thread());
		}
		else
		{
			new->eax = 0;
		}
	}
		break;

	case 0x305: //DRVCall read EBX=path char* ECX=data page*
	{
		char* path = (char*)in->ebx;
		struct fs_node* node = fstree_find_path(path);
		if(node != 0 && node->subtype == FST_DRIVER) {
			struct driver* d = node->sub;
			new->eax = (uint32_t)init_rpc(d->driverThread, d->rpc_read, node->resourceID, vmm_resolve((void*)in->ecx), get_current_thread());
		}
		else
		{
			new->eax = 0;
		}
	}
		break;


	case 0x400: //VMM ucont alloc EBX=pages
		new->eax = (uint32_t) vmm_alloc_ucont(in->ebx);
		break;

	case 0x401: //VMM free EBX=address
		if(in->ebx < USERSPACE_BOTTOM) show_cod(in, "Userspace program tried to free kernel memory.");
		vmm_free((void*)in->ebx);
		break;

	case 0x500: //THREAD create EBX=entry_point* ECX=args*
		new->eax = (uint32_t) create_thread(get_current_thread()->environment, (void*)in->ebx);
		setargsptr((struct thread*)new->eax, (void*)in->ecx);
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

		void* entryPoint = unpack_elf(elfKernel);
		if(entryPoint == 0) goto noLoad;

		void* dataUserspace = 0;
		if(in->ecx) {
			dataUserspace = vmm_alloc_ucont(in->ecx / 0x1000);
			memcpy(dataUserspace, dataKernel, in->ecx);
		}

		struct thread* t = create_thread(newEnv, entryPoint);
		setargsptr(t, dataUserspace);

		new->eax = (uint32_t)t;

		noLoad:
		free(dataKernel);
		free(elfKernel);
		vmm_activate_pagedir(oldEnv->phys_pdir);
		break;

	case 0x600: //Register IRQ-RPC EBX=irqID ECX=rpcID
		new->eax = register_irq_rpc(in->ebx, in->ecx);
		break;

		//FIXME: PORT RESTRICTIONS!
	case 0x601: //OUTB EBX=port ECX=value
		outb(in->ebx, in->ecx);
		break;

	case 0x602: //OUTW EBX=port ECX=value
		outw(in->ebx, in->ecx);
		break;

	case 0x603: //OUTL EBX=port ECX=value
		outl(in->ebx, in->ecx);
		break;

	case 0x604: //INB EBX=port
		new->eax = inb(in->ebx);
		break;

	case 0x605: //INW EBX=port
		new->eax = inw(in->ebx);
		break;

	case 0x606: //INL EBX=port
		new->eax = inl(in->ebx);
		break;

	default:
		kprintf("Terminated thread due to unhandled syscall...\n");
		new = schedule_exception(in);
	}

	return new;
}
