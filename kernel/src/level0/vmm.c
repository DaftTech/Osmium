#include "level0/vmm.h"

extern const void kernel_end;

PADDR kernel_pagetables[KERNEL_COMBINED_TABLES];

uint32_t* active_pagetables = ACTIVE_PAGETABLES;

PADDR vmmCreate() {
	PADDR phys_pagedir = 0;
	uint32_t* pagedir_ptr = vmmAllocate(&phys_pagedir);

	pagedir_ptr[1023] = phys_pagedir | PD_PRESENT | PD_WRITE;
	uint32_t paddr, i, i2;

	for (i = 0; i < 1023; i++) {
		if (i < KERNEL_COMBINED_TABLES) {
			pagedir_ptr[i] = kernel_pagetables[i] | PD_PRESENT | PD_WRITE;
		} else {
			PADDR phys = 0;
			uint32_t* c = vmmAllocate(&phys);
			pagedir_ptr[i] = phys  | PD_PRESENT | PD_WRITE | PD_PUBLIC;
			for(i2 = 0; i2 < 1024; i2++) {
				c[i2] = PD_WRITE | PD_PUBLIC;
			}
			vmmUnmap(c);
		}
	}
	return phys_pagedir;
}

PADDR vmmGetActivePhysical(void) {
	return (active_pagetables[0xFFFFF] & 0xFFFFF000);
}

PADDR vmmResolve(void* vaddr) {
	return active_pagetables[(uint32_t)vaddr >> 12] & 0xFFFFF000;
}

void vmmMapRange(void* ivaddr, PADDR paddr, uint32_t length, uint32_t flags) {
	ADDRESS vaddr = (ADDRESS)ivaddr;

	if (vaddr & 0xFFF)
		return;
	if (paddr & 0xFFF)
		return;

	for (uint32_t i = 0; i < length; i += 0x1000) {
		vmmMapAddress((void*) (vaddr + i), paddr + i, flags);
	}
}

void vmmMapAddress(void* ivaddr, PADDR paddr, uint32_t flags) {
	ADDRESS vaddr = (ADDRESS)ivaddr;

	active_pagetables[vaddr >> 12] = (paddr & 0xFFFFF000) | PT_PRESENT
			| PT_WRITE | (flags & 0xFFF)
			| (active_pagetables[vaddr >> 12] & (PT_PUBLIC | PT_ALLOCATED));

	asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
}

void vmmTruncate(void) {
	for (uint32_t i = USERSPACE_BOTTOM; i < 0xFFFFF000; i += 0x1000) {
		vmmFree((void*)i);
	}
}

void vmmFree(void* ivaddr) {
	ADDRESS vaddr = (ADDRESS) ivaddr;

	if ((active_pagetables[vaddr >> 12] & (PT_ALLOCATED | PT_PRESENT))
			== (PT_ALLOCATED | PT_PRESENT)) {
		pmmFree((void*) (active_pagetables[vaddr >> 12] & 0xFFFFF000));

		active_pagetables[vaddr >> 12] = PT_ALLOCATED
				| (active_pagetables[vaddr >> 12] & PT_PUBLIC);
		asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
	}
}

void vmmUnmap(void* p_vaddr) { //USE ONLY IF YOU KNOW WHAT YOU DO. POTENTIAL MEMORY LEAK!
	uint32_t vaddr = (uint32_t) p_vaddr;

	if ((active_pagetables[vaddr >> 12] & (PT_ALLOCATED | PT_PRESENT))
			== (PT_ALLOCATED | PT_PRESENT)) {
		active_pagetables[vaddr >> 12] = (active_pagetables[vaddr >> 12] & PT_PUBLIC);
		asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
	}
}

void* vmmAllocateAddress(void* ivaddr, uint32_t* retpaddr) {
	ADDRESS vaddr = (ADDRESS)ivaddr;

	if (vaddr == 0) {
		kprintf(
				"Denied vmm_alloc_addr at %x (Flags: %x) ... this is a potential mm-fault \n",
				vaddr, 0);
		return 0;
	}

	if ((active_pagetables[vaddr >> 12] & PT_PRESENT) == PT_PRESENT) {
		kprintf(
				"Denied vmm_alloc_addr at %x (Flags: %x) ... this is a potential mm-fault \n",
				vaddr, active_pagetables[vaddr >> 12]);
		return 0;
	}

	uint32_t paddr = (uint32_t) pmmAllocate();
	vmmMapAddress(ivaddr, paddr, PT_PUBLIC | PT_ALLOCATED);

	if (retpaddr != 0)
		*retpaddr = paddr;

	return ivaddr;
}

static void* vmmAllocateInRange(uint32_t low, uint32_t high, uint32_t* retpaddr,
		uint32_t cont) {
	void* vaddr = 0;
	uint32_t i = 0;
	uint32_t c = 0;

	for (i = (low & 0xFFFFF000); i < high; i += 0x1000) {
		if ((active_pagetables[i >> 12] & (PT_PRESENT)) != PT_PRESENT) {
			if (c == 0)
				vaddr = (void*) i;
			c++;
			if (c >= cont)
				break;
		} else {
			c = 0;
		}
	}

	uint32_t off = 0;

	while (c--) {
		vmmAllocateAddress(vaddr + off * 0x1000, off == 0 ? retpaddr : 0);
		off++;
	}

	return vaddr;
}

void* vmmAllocateInUserspaceCont(uint32_t cont) {
	return vmmAllocateInRange(USERSPACE_BOTTOM, 0xFFC00000, 0, cont);
}

void* vmmAllocateInUserspace(PADDR* retpaddr) {
	return vmmAllocateInRange(USERSPACE_BOTTOM, 0xFFC00000, retpaddr, 1);
}

void* vmmAllocateCont(uint32_t cont) {
	return vmmAllocateInRange(KERNELSPACE_ALLOC_BOTTOM, USERSPACE_BOTTOM, 0,
			cont);
}

void* vmmAllocate(PADDR* retpaddr) {
	return vmmAllocateInRange(KERNELSPACE_ALLOC_BOTTOM, USERSPACE_BOTTOM,
			retpaddr, 1);
}

void vmmActivate(PADDR pdpaddr) {
	asm volatile("mov %0, %%cr3" : : "r" (pdpaddr));
}

PADDR vmmInit(void) {
	//Kernelspace init

	for (uint32_t i = 0; i < KERNEL_COMBINED_TABLES; i++) {
		kernel_pagetables[i] = (PADDR) pmmAllocate() | PD_PRESENT | PD_WRITE;
		uint32_t* kptAccess = (void*) (kernel_pagetables[i] & 0xFFFFF000);

		for (uint32_t i2 = 0; i2 < 1023; i2++) {
			PADDR addr = (i << 22) + (i2 << 12);
			PADDR written_addr = 0;
			uint32_t flags = PT_WRITE;

			if (addr < (uint32_t) &kernel_end && addr != 0) {
				written_addr = addr;
				flags |= PT_PRESENT;
			}

			kptAccess[i2] = written_addr | flags;
		}
	}

	//VMM_CREATE
	uint32_t* pagedir_ptr = pmmAllocate();
	PADDR phys_pagedir = (PADDR) pagedir_ptr;

	pagedir_ptr[1023] = phys_pagedir | PD_PRESENT | PD_WRITE;
	uint32_t paddr, i, i2;

	for (i = 0; i < 1023; i++) {
		if (i < KERNEL_COMBINED_TABLES) {
			pagedir_ptr[i] = kernel_pagetables[i] | PD_PRESENT | PD_WRITE;
		} else {
			pagedir_ptr[i] = (PADDR) pmmAllocate() | PD_PRESENT | PD_WRITE | PD_PUBLIC;
			for(i2 = 0; i2 < 1024; i2++) {
				((uint32_t*)pagedir_ptr[i])[i2] = PD_WRITE | PD_PUBLIC;
			}
		}
	}

	//END_CREATE

	vmmActivate(phys_pagedir);

	uint32_t cr0;

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= (1 << 31);
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	return phys_pagedir;
}
