#include "level0/console.h"

#include "multiboot.h"

void clevel_entry(struct multiboot_info* mb_info) {
	clrscr();

	kprintf("LEVEL0 ENTRY\n");
}
