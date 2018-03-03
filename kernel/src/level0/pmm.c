#include "level0/pmm.h"

#define BITMAP_SIZE 32768

uint32_t allocatable[BITMAP_SIZE];
uint32_t upper_limit = 0;

void* pmmAllocate() {
	int i;
	for (i = 0; i < BITMAP_SIZE; i++) {
		if (allocatable[i]) {
			int s;
			for (s = 0; s < 32; s++) {
				if (allocatable[i] & (1 << s)) {
					allocatable[i] &= ~(1 << s);
					return (uint8_t*) ((i * 32 + s) * 4096);
				}
			}
		}
	}
	return 0;
}

void pmmPrintStats() {
	kprintf("Running Osmium with %dMB/%dMB (%dkB/%dkB) available memory!\n",
			pmmGetFreeSpace(1024 * 1024), upper_limit / (1024 * 1024),
			pmmGetFreeSpace(1024), upper_limit / 1024);
}

uint32_t pmmGetFreeSpace(uint32_t div) {
	uint32_t free = 0;

	for (uint32_t i = 0; i < BITMAP_SIZE; i++) {
		for (uint32_t s = 0; s < 32; s++) {
			if (allocatable[i] & (1 << s))
				free += 0x1000;
		}
	}

	return free / div;
}

void pmmFree(void* addr) {
	uintptr_t ptr = ((uintptr_t) addr) / 4096;

	uintptr_t s = ptr % 32;
	uintptr_t i = ptr / 32;

	allocatable[i] |= (1 << s);
}

void pmmMarkUsed(void* addr) {
	uintptr_t ptr = ((uintptr_t) addr) / 4096;

	uintptr_t s = ptr % 32;
	uintptr_t i = ptr / 32;

	allocatable[i] &= ~(1 << s);
}

void pmmInit(struct multiboot_info* mb_info) {
	struct multiboot_mmap* mmap = mb_info->mi_mmap_addr;
	struct multiboot_mmap* mmap_end = (void*) ((uintptr_t) mb_info->mi_mmap_addr
			+ mb_info->mi_mmap_length);

	memset(allocatable, 0, sizeof(allocatable));

	kprintf("BITMAP: %x to %x ", allocatable, &(allocatable[BITMAP_SIZE-1]));

	while (mmap < mmap_end) {
		if (mmap->mm_type == 1) {
			uintptr_t addr = mmap->mm_base_addr;
			uintptr_t end_addr = addr + mmap->mm_length;

			while (addr < end_addr) {
				pmmFree((void*) addr);
				if (addr > upper_limit)
					upper_limit = addr;
				addr += 0x1000;
			}
		}
		mmap++;
	}

	extern const void* kernel_start;
	extern const void* kernel_end;

	uintptr_t addr = (uintptr_t) &kernel_start;
	while (addr < (uintptr_t) &kernel_end) {
		pmmMarkUsed((void*) addr);
		addr += 0x1000;
	}

	struct multiboot_module* modules = mb_info->mi_mods_addr;

	pmmMarkUsed(mb_info);
	pmmMarkUsed(modules);

	uint32_t i;
	for (i = 0; i < mb_info->mi_mods_count; i++) {
		addr = (uintptr_t) modules[i].start;
		while (addr < (uintptr_t) modules[i].end) {
			pmmMarkUsed((void*) addr);
			addr += 0x1000;
		}
	}
}
