#include "level0/console.h"
#include "level0/gdt.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/idt.h"
#include "level0/ports.h"
#include "level0/catofdeath.h"
#include "level1/scheduler.h"
#include "level1/tar.h"
#include "level1/elf.h"

#include "stdlib.h"
#include "multiboot.h"

void clevel_entry(struct multiboot_info* mb_info) {
	clrscr();
	setclr(COLOR(SCLR_BLACK, SCLR_CYAN));
	kprintf("LEVEL0 ENTRY\n");

	setclr(C_DEFAULT);
	kprintf("GDT INIT ");
	init_gdt();
	setclr(C_SUCCESS);
	kprintf("DONE!\n");
	setclr(C_DEFAULT);

	kprintf("IDT INIT ");
	init_idt();
	setclr(C_SUCCESS);
	kprintf("DONE!\n");
	setclr(C_DEFAULT);

	kprintf("PMM INIT ");
	pmm_init(mb_info);
	setclr(C_SUCCESS);
	kprintf("DONE!\n");
	setclr(C_DEFAULT);

	setclr(COLOR(SCLR_BLACK, SCLR_YELLOW));
	pmm_print_stats();
	setclr(C_DEFAULT);

	kprintf("VMM/PAGING INIT ");
	PHYSICAL root = vmm_init();
	setclr(C_SUCCESS);
	kprintf("DONE! (root=%x, getRoot=%x)\n", root, vmm_get_current_physical());
	setclr(C_DEFAULT);

	setclr(COLOR(SCLR_BLACK, SCLR_CYAN));
	kprintf("LEVEL1 ENTRY\n");
	setclr(C_DEFAULT);

	kprintf("Creating environment...\n");

	struct environment* rootEnv = malloc(sizeof(struct environment));
	rootEnv->phys_pdir = root;

	kprintf("Mapping multiboot...\n");

	vmm_map_address(mb_info, (uint32_t) mb_info, 0);
	vmm_map_address(mb_info->mi_mods_addr, (uint32_t) mb_info->mi_mods_addr, 0);

	if (!(mb_info->mi_flags & MULTIBOOT_INFO_HAS_MODS)) {
		show_cod(0, "No multiboot module (initrfs?) available.\n");
	}

	void* initrfs = mb_info->mi_mods_addr[0].start;
	size_t initrfsSize = mb_info->mi_mods_addr[0].end - mb_info->mi_mods_addr[0].start;

	vmm_map_range(initrfs, (uint32_t) initrfs, initrfsSize,	0);

	kprintf("Assuming mbmod[0] is a tarball (%d bytes)... \n", initrfsSize);

	uint32_t* initELF = tar_extract(mb_info->mi_mods_addr[0].start, "init");

	if(initELF == (void*)0) {
		show_cod(0, "initrfs damaged or didn't contain \"/init\".\n");
	}

	kprintf("Unpacking ELF...\n");
	unpack_elf(&initELF[1]);

	kprintf("Copying initrfs BLOB into userspace (for init)...\n");
	void* dest = vmm_alloc_ucont(((initrfsSize - 1) / 0x1000) + 1);
    memcpy(dest, initrfs, initrfsSize);

	kprintf("Creating thread zero...\n");
	struct thread* zero = create_thread(rootEnv, (void*)USERSPACE_BOTTOM);

	kprintf("Push userspace address of initrfs BLOB...\n");
	push(zero, (uint32_t)dest);

	kprintf("Setting PIT interval...\n");
	outb(0x43, 0b00110100);
	outb(0x40, 0x00);
	outb(0x40, 0x08);

	kprintf("Enabling scheduler...\n");
	enableScheduling();
}
