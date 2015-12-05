#include "level0/vmm.h"

extern const void kernel_end;

PADDR kernel_pagetables[KERNEL_COMBINED_TABLES];

uint32_t* active_pagetables = ACTIVE_PAGETABLES;

PADDR vmm_create() {
	PADDR phys_pagedir = 0;
	uint32_t* pagedir_ptr = vmm_alloc(&phys_pagedir);

	pagedir_ptr[1023] = phys_pagedir | PD_PRESENT | PD_WRITE;
	uint32_t paddr, i, i2;

	for (i = 0; i < 1023; i++) {
		if (i < KERNEL_COMBINED_TABLES) {
			pagedir_ptr[i] = kernel_pagetables[i] | PD_PRESENT | PD_WRITE;
		} else {
			PADDR phys = 0;
			uint32_t* c = vmm_alloc(&phys);
			pagedir_ptr[i] = phys  | PD_PRESENT | PD_WRITE | PD_PUBLIC;
			for(i2 = 0; i2 < 1024; i2++) {
				c[i2] = PD_WRITE | PD_PUBLIC;
			}
			vmm_unmap(c);
		}
	}
	return phys_pagedir;
}

PADDR vmm_get_current_physical(void) {
	return (active_pagetables[0xFFFFF] & 0xFFFFF000);
}

PADDR vmm_resolve(void* vaddr) {
	return active_pagetables[(uint32_t)vaddr >> 12] & 0xFFFFF000;
}

void vmm_map_range(void* ivaddr, PADDR paddr, uint32_t length, uint32_t flags) {
	ADDRESS vaddr = (ADDRESS)ivaddr;

	if (vaddr & 0xFFF)
		return;
	if (paddr & 0xFFF)
		return;

	for (uint32_t i = 0; i < length; i += 0x1000) {
		vmm_map_address((void*) (vaddr + i), paddr + i, flags);
	}
}

void vmm_map_address(void* ivaddr, PADDR paddr, uint32_t flags) {
	ADDRESS vaddr = (ADDRESS)ivaddr;

	active_pagetables[vaddr >> 12] = (paddr & 0xFFFFF000) | PT_PRESENT
			| PT_WRITE | (flags & 0xFFF)
			| (active_pagetables[vaddr >> 12] & (PT_PUBLIC | PT_ALLOCATED));

	asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
}

void vmm_destroy(void) {
	for (uint32_t i = USERSPACE_BOTTOM; i < 0xFFFFF000; i += 0x1000) {
		vmm_free((void*)i);
	}
}

void vmm_free(void* ivaddr) {
	ADDRESS vaddr = (ADDRESS) ivaddr;

	if ((active_pagetables[vaddr >> 12] & (PT_ALLOCATED | PT_PRESENT))
			== (PT_ALLOCATED | PT_PRESENT)) {
		pmm_free((void*) (active_pagetables[vaddr >> 12] & 0xFFFFF000));

		active_pagetables[vaddr >> 12] = PT_ALLOCATED
				| (active_pagetables[vaddr >> 12] & PT_PUBLIC);
		asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
	}
}

void vmm_unmap(void* p_vaddr) { //USE ONLY IF YOU KNOW WHAT YOU DO. POTENTIAL MEMORY LEAK!
	uint32_t vaddr = (uint32_t) p_vaddr;

	if ((active_pagetables[vaddr >> 12] & (PT_ALLOCATED | PT_PRESENT))
			== (PT_ALLOCATED | PT_PRESENT)) {
		active_pagetables[vaddr >> 12] = (active_pagetables[vaddr >> 12] & PT_PUBLIC);
		asm volatile("invlpg %0" : : "m" (*(char*)vaddr));
	}
}

void* vmm_alloc_addr(void* ivaddr, uint32_t* retpaddr) {
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

	uint32_t paddr = (uint32_t) pmm_alloc();
	vmm_map_address(ivaddr, paddr, PT_PUBLIC | PT_ALLOCATED);

	if (retpaddr != 0)
		*retpaddr = paddr;

	return ivaddr;
}

static void* vmm_alloc_in_range(uint32_t low, uint32_t high, uint32_t* retpaddr,
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
		vmm_alloc_addr(vaddr + off * 0x1000, off == 0 ? retpaddr : 0);
		off++;
	}

	return vaddr;
}

void* vmm_alloc_ucont(uint32_t cont) {
	return vmm_alloc_in_range(USERSPACE_BOTTOM, 0xFFC00000, 0, cont);
}

void* vmm_alloc_user(PADDR* retpaddr) {
	return vmm_alloc_in_range(USERSPACE_BOTTOM, 0xFFC00000, retpaddr, 1);
}

void* vmm_alloc_cont(uint32_t cont) {
	return vmm_alloc_in_range(KERNELSPACE_ALLOC_BOTTOM, USERSPACE_BOTTOM, 0,
			cont);
}

void* vmm_alloc(PADDR* retpaddr) {
	return vmm_alloc_in_range(KERNELSPACE_ALLOC_BOTTOM, USERSPACE_BOTTOM,
			retpaddr, 1);
}

void vmm_activate_pagedir(PADDR pdpaddr) {
	asm volatile("mov %0, %%cr3" : : "r" (pdpaddr));
}

PADDR vmm_init(void) {
	//Kernelspace init

	for (uint32_t i = 0; i < KERNEL_COMBINED_TABLES; i++) {
		kernel_pagetables[i] = (PADDR) pmm_alloc() | PD_PRESENT | PD_WRITE;
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
	uint32_t* pagedir_ptr = pmm_alloc();
	PADDR phys_pagedir = (PADDR) pagedir_ptr;

	pagedir_ptr[1023] = phys_pagedir | PD_PRESENT | PD_WRITE;
	uint32_t paddr, i, i2;

	for (i = 0; i < 1023; i++) {
		if (i < KERNEL_COMBINED_TABLES) {
			pagedir_ptr[i] = kernel_pagetables[i] | PD_PRESENT | PD_WRITE;
		} else {
			pagedir_ptr[i] = (PADDR) pmm_alloc() | PD_PRESENT | PD_WRITE | PD_PUBLIC;
			for(i2 = 0; i2 < 1024; i2++) {
				((uint32_t*)pagedir_ptr[i])[i2] = PD_WRITE | PD_PUBLIC;
			}
		}
	}

	//END_CREATE

	vmm_activate_pagedir(phys_pagedir);

	uint32_t cr0;

	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= (1 << 31);
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	return phys_pagedir;
}
