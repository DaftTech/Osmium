#include "level0/idt.h"
#include "level0/console.h"
#include "level0/ports.h"
#include "level0/gdt.h"
#include "level0/catofdeath.h"
#include "level1/scheduler.h"
#include "level1/syscall.h"

#define IDT_ENTRIES 64

extern uint8_t* interrupt_stack;

static uint32_t tss[32] = { 0, (uint32_t) &interrupt_stack, 0x10 };

static uint64_t idt[IDT_ENTRIES];
static struct RPCDestination {
	Module* thread;
	uint32_t rpcID;
} registered[IDT_ENTRIES];

extern "C" void intr_stub_0(void);
extern "C" void intr_stub_1(void);
extern "C" void intr_stub_2(void);
extern "C" void intr_stub_3(void);
extern "C" void intr_stub_4(void);
extern "C" void intr_stub_5(void);
extern "C" void intr_stub_6(void);
extern "C" void intr_stub_7(void);
extern "C" void intr_stub_8(void);
extern "C" void intr_stub_9(void);
extern "C" void intr_stub_10(void);
extern "C" void intr_stub_11(void);
extern "C" void intr_stub_12(void);
extern "C" void intr_stub_13(void);
extern "C" void intr_stub_14(void);
extern "C" void intr_stub_15(void);
extern "C" void intr_stub_16(void);
extern "C" void intr_stub_17(void);
extern "C" void intr_stub_18(void);

extern "C" void intr_stub_32(void);
extern "C" void intr_stub_33(void);
extern "C" void intr_stub_34(void);
extern "C" void intr_stub_35(void);
extern "C" void intr_stub_36(void);
extern "C" void intr_stub_37(void);
extern "C" void intr_stub_38(void);
extern "C" void intr_stub_39(void);
extern "C" void intr_stub_40(void);
extern "C" void intr_stub_41(void);
extern "C" void intr_stub_42(void);
extern "C" void intr_stub_43(void);
extern "C" void intr_stub_44(void);
extern "C" void intr_stub_45(void);
extern "C" void intr_stub_46(void);
extern "C" void intr_stub_47(void);

extern "C" void intr_stub_48(void);

void setIDTEntry(int i, void (*fn)(), uint16_t selector, uint8_t flags) {
	unsigned long int handler = (unsigned long int) fn;
	idt[i] = handler & 0xffffLL;
	idt[i] |= (selector & 0xffffLL) << 16;
	idt[i] |= (flags & 0xffLL) << 40;
	idt[i] |= ((handler >> 16) & 0xffffLL) << 48;
}

int registerIRQRPC(uint32_t irqID, uint32_t rpcID) {
	if(irqID > IDT_ENTRIES) return 1;
	if(!registered[irqID].thread) {
		registered[irqID].thread = getCurrentThread();
		registered[irqID].rpcID = rpcID;
		return 0;
	}
	return 2;
}

void initIDT() {
	struct {
		unsigned short int limit;
		void* pointer;
	}__attribute__((packed)) idtp = { .limit = IDT_ENTRIES * 8 - 1, .pointer =
			idt, };

	for(int i = 0; i < IDT_ENTRIES; i++) {
		idt[i] = 0;
		registered[i].thread = 0;
	}

	outb(0x20, 0x11); // Initialisierungsbefehl fuer den PIC
	outb(0x21, 0x20); // Interruptnummer fuer IRQ 0
	outb(0x21, 0x04); // An IRQ 2 haengt der Slave
	outb(0x21, 0x01); // ICW 4

	outb(0xa0, 0x11); // Initialisierungsbefehl fuer den PIC
	outb(0xa1, 0x28); // Interruptnummer fuer IRQ 8
	outb(0xa1, 0x02); // An IRQ 2 haengt der Slave
	outb(0xa1, 0x01); // ICW 4

	outb(0x20, 0x0);
	outb(0xa0, 0x0);

	setIDTEntry(0,  intr_stub_0, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(1,  intr_stub_1, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(2,  intr_stub_2, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(3,  intr_stub_3, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(4,  intr_stub_4, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(5,  intr_stub_5, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(6,  intr_stub_6, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(7,  intr_stub_7, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(8,  intr_stub_8, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(9,  intr_stub_9, 0x8,  IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(10, intr_stub_10, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(11, intr_stub_11, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(12, intr_stub_12, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(13, intr_stub_13, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(14, intr_stub_14, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(15, intr_stub_15, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(16, intr_stub_16, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(17, intr_stub_17, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(18, intr_stub_18, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

	setIDTEntry(32, intr_stub_32, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(33, intr_stub_33, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(34, intr_stub_34, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(35, intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(36, intr_stub_36, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(37, intr_stub_37, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(38, intr_stub_38, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(39, intr_stub_39, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(40, intr_stub_40, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(41, intr_stub_41, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(42, intr_stub_42, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(43, intr_stub_43, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(44, intr_stub_44, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(45, intr_stub_45, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(46, intr_stub_46, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	setIDTEntry(47, intr_stub_47, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

	setIDTEntry(48, intr_stub_48, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT);

	setGDTEntry(5, (uint32_t) tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

	asm volatile("ltr %%ax" : : "a" (5 << 3));

	asm volatile("lidt %0" : : "m" (idtp));
	asm volatile("sti");
}

extern "C" CPUState* handleInterrupt(CPUState* cpu) {
	CPUState* new_cpu = cpu;

	if (cpu->intr <= 0x1f) {
		if(isSchedulingEnabled()) {
			new_cpu = scheduleException(cpu);
		}
		else
		{
			showCOD(cpu, "Kernel PANIC!");
		}
	} else if (cpu->intr >= 0x20 && cpu->intr <= 0x2f) {
		if (cpu->intr >= 0x28) {
			outb(0xa0, 0x20);
		}

		if (cpu->intr == 0x20) {
			new_cpu = schedule(cpu);
		}
		else
		{
			if(registered[cpu->intr].thread != 0) {
				//TODO: remoteCall(registered[cpu->intr].thread, registered[cpu->intr].rpcID, cpu->intr); //call IRQ RPC
			}
		}

        outb(0x20, 0x20);
	} else if (cpu->intr == 0x30) {
		new_cpu = syscall(new_cpu);
	} else {
		showCOD(cpu, "Unknown Interrupt!");
	}

	/*if(isSchedulingEnabled() && new_cpu == cpu) {
		return schedule(new_cpu);
	}*/
	return new_cpu;
}
