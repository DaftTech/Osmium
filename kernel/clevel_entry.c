#include "level0/console.h"
#include "level0/gdt.h"
#include "level0/pmm.h"
#include "level0/vmm.h"
#include "level0/idt.h"

#include "multiboot.h"

void clevel_entry(struct multiboot_info* mb_info) {
	clrscr();
	setclr(C_DEFAULT);
	kprintf("LEVEL0 ENTRY\n");

	kprintf("GDT INIT ");
	init_gdt();
	setclr(C_SUCCESS);
	kprintf("DONE!\n");
	setclr(C_DEFAULT);

	kprintf("PMM INIT ");
	pmm_init(mb_info);
	setclr(C_SUCCESS);
	kprintf("DONE!\n");

	kprintf("IDT INIT ");
	init_idt();
	setclr(C_SUCCESS);
	kprintf("DONE!\n");
	setclr(C_DEFAULT);


	setclr(COLOR(SCLR_BLACK, SCLR_YELLOW));
	pmm_print_stats();
	kprintf("\n");

	setclr(C_DEFAULT);
	kprintf("VMM/PAGING INIT ");
	PHYSICAL root = vmm_init();
	setclr(C_SUCCESS);
	kprintf("DONE! (root=%x)\n", root);

	setclr(COLOR(SCLR_BLACK, SCLR_YELLOW));
	pmm_print_stats();
	setclr(C_DEFAULT);

	kprintf("VMM ALLOC TEST ");
	void* pointer = vmm_alloc(0);
	setclr(C_SUCCESS);
	kprintf("DONE!\n");


}
