#include "level0/console.h"
#include "level0/gdt.h"

#include "multiboot.h"

void clevel_entry(struct multiboot_info* mb_info) {
	clrscr();
	setclr(C_DEFAULT);
	kprintf("LEVEL0 ENTRY\n");

	kprintf("GDT INIT ");
	init_gdt();
	setclr(C_SUCCESS);
	kprintf("DONE!");
	setclr(C_DEFAULT);
}
