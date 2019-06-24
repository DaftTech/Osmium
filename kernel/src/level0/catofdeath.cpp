#include "level0/catofdeath.h"

static uint32_t icod = 0;

uint32_t in_cod() {
    return icod;
}

void showCOD(CPUState* cpu, char* fstr) {
    icod = 1;
#ifdef SCREEN_COD
    clrscr();

    kprintf ("        (`. \n");
    kprintf ("         ) ) \n");
    kprintf ("        ( ( \n");
    kprintf ("         \\ \\ \n");
    kprintf ("          \\ \\ \n");
    kprintf ("        .-'  `-. \n");
    kprintf ("       /        `. \n");
    kprintf ("      (      )    `-._ ,    _ \n");
    kprintf ("       )   ,'         (.\\--'( \n");
    kprintf ("       \\  (         ) /      \\ \n");
    kprintf ("        \\  \\_(     / (    <6 (6 \n");
    kprintf ("         \\_)))\\   (   `._  .:Y)__ \n");
    kprintf ("          '''  \\   `-._.'`---^_))) \n");
    kprintf ("                `-._ )))       ``` \n");
    kprintf ("                     ```            \n");
#endif

    setclr(0x04);
    kprintf(fstr);
    if(cpu != 0) {
    	kprintf("\n\nException I:%x E:%x, Kernel halt!\n", cpu->intr, cpu->error);
    	showDump(cpu);
    }

    uint32_t cr2 = 0;
    asm volatile("mov %%cr2, %0" : "=r" (cr2));

    switch(cpu->intr) {
        case 0x00:
            kprintf("\n DIVIDE BY ZERO");
            break;
        case 0x0D:
            kprintf("\n GENERAL PROTECTION FAULT");
            break;
        case 0x0E:
            kprintf("\n PAGE FAULT");

            if(cr2 == 0x00) {
                kprintf("\n refNull @ 0x%x", cpu->eip);
            }
            else
            {

            }

            break;
        default:
            kprintf("\n Description needs to be added.");
            break;
    }

    while (1) {
        asm volatile("cli; hlt");
    }
}

void showDump(CPUState* cpu) {
    kprintf("EAX: %x EBX: %x ECX: %x EDX: %x\n", cpu->eax, cpu->ebx, cpu->ecx,
            cpu->edx);
    kprintf("ESI: %x EDI: %x EBP: %x EIP: %x\n", cpu->esi, cpu->edi, cpu->ebp,
            cpu->eip);
    kprintf("CS: %x EFLAGS: %x ESP: %x SS: %x\n", cpu->cs, cpu->eflags,
            cpu->esp, cpu->ss);

    uint32_t cr2 = 0;

    asm volatile("mov %%cr2, %0" : "=r" (cr2));

    kprintf("CR2: %x \n", cr2);

//    uint32_t* stack = (uint32_t*)cpu->esp;

//    for(int i = 0; i < 10; i++) {
//    	kprintf("esp[%d] = %x\n", i, stack[i]);
//    }
}
