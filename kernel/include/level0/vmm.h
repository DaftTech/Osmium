#ifndef VMM_H
#define VMM_H

#include "stdint.h"
#include "multiboot.h"
#include "pmm.h"
#include "console.h"
#include "stddef.h"
#include "stdmem.h"

#define PD_PRESENT (1 << 0)
#define PD_WRITE   (1 << 1)
#define PD_PUBLIC  (1 << 2)
#define PD_WTCACHE (1 << 3)
#define PD_NOCACHE (1 << 4)
#define PD_USED    (1 << 5)
#define PD_USER0   (1 << 9)
#define PD_USER1   (1 << 10)
#define PD_USER2   (1 << 11)

#define PT_PRESENT (1 << 0)
#define PT_WRITE   (1 << 1)
#define PT_PUBLIC  (1 << 2)
#define PT_WTCACHE (1 << 3)
#define PT_NOCACHE (1 << 4)
#define PT_USED    (1 << 5)
#define PT_WRITTEN (1 << 6)
#define PT_USER0   (1 << 9)
#define PT_USER1   (1 << 10)
#define PT_USER2   (1 << 11)

#define PT_ALLOCATED PT_USER0

#define KERNEL_TABLES 64 		//256MB hard kernel space
#define KERNEL_ALLOC_TABLES 64	//256MB allocatable kernel space
#define KERNEL_COMBINED_TABLES (KERNEL_TABLES + KERNEL_ALLOC_TABLES)

#define USERSPACE_BOTTOM (KERNEL_COMBINED_TABLES * 0x400000)
#define KERNELSPACE_ALLOC_BOTTOM (USERSPACE_BOTTOM - (KERNEL_ALLOC_TABLES * 0x400000))

#define ACTIVE_PAGETABLES (void*)0xFFC00000

PADDR vmm_init(void);
PADDR vmm_create(void);
void  vmm_destroy(void);

void vmm_activate_pagedir(uint32_t context);

void vmm_free(void* vaddr);
void vmm_unmap(void* vaddr);

void vmm_map_range(void* vaddr, PADDR paddr, uint32_t length, uint32_t flags);
void vmm_map_address(void* vaddr, uint32_t paddr, uint32_t flags);

void* vmm_alloc(uint32_t* retpaddr);
void* vmm_alloc_cont(uint32_t cont);
void* vmm_alloc_user(uint32_t* retpaddr);
void* vmm_alloc_ucont(uint32_t cont);
void* vmm_alloc_addr(void* vaddr, uint32_t* paddr);

uint32_t vmm_resolve(void* vaddr);

uint32_t vmm_get_current_physical(void);

#endif
