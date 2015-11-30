#include "level1/syscall.h"
#include "level0/console.h"
#include "level1/scheduler.h"
#include "level1/fstree.h"

struct cpu_state* syscall(struct cpu_state* in) {
	struct cpu_state* new = in;

	switch(in->eax) {
	case 0x1:
		kprintf("Thread ended with %d...\n", in->ebx);
		new = terminate_current(in);
		break;

	case 0x2:
		kprintf("Told %x that argsptr is %x\n", get_current_thread(), get_current_thread()->argsptr);
		new->eax = (uint32_t)get_current_thread()->argsptr;
		break;

	case 0x100:
		setclr(COLOR(SCLR_BLACK, SCLR_BLUE));
		kputc((char)in->ebx);
		setclr(C_DEFAULT);
		break;

	case 0x101:
		new->eax = (uint32_t)init_rpc(get_current_thread(), 0, 0);
		break;

	case 0x200: //RPC Map
		new->eax = (uint32_t)rpc_map();
		new->ebx = get_current_thread()->active_rpc->rpcID;
		break;

	case 0x201: //RPC Return
		return_rpc(in->ebx);
		new = schedule(in);
		break;

	case 0x202: //RPC Register Handler
		get_current_thread()->rpc_handler_address = in->ebx;
		break;

	case 0x203: //RPC rpc_check_future
		;
		uint32_t ebx = in->ebx;

		struct thread* c = get_current_thread();
		struct rpc_future* bCheck = (c->active_rpc && c->active_rpc->state != RPC_STATE_AWAITING) ? c->active_rpc->blockedBy : c->blockedBy;

		uint32_t anyBlock = 0;

		while(bCheck != 0) {
			anyBlock |= bCheck->state;
			if((uint32_t)bCheck == ebx) {
				anyBlock = bCheck->state;
				break;
			}

			bCheck = bCheck->next;
		}

		new->eax = anyBlock;
		break;


	case 0x300: //Register driver
		new->eax = fstree_register_driver(in->ebx, in->ecx, in->edx, in->edi, in->esi);
		break;

	default:
		kprintf("Terminated thread due to unhandled syscall...\n");
		new = terminate_current(in);
	}

	return new;
}
