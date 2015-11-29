#include "level0/console.h"
#include "level0/gdt.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/idt.h"

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
	kprintf("DONE! (root=%x)\n", root);
	setclr(C_DEFAULT);


}
