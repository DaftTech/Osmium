#include "syscall.h"

void syscall(RegState* state) {
	asm("int $0x30"

			: "=a" (state->eax),
			"=b" (state->ebx),
			"=c" (state->ecx),
			"=d" (state->edx),
			"=S" (state->esi),
			"=D" (state->edi)

			: "a" (state->eax),
			"b" (state->ebx),
			"c" (state->ecx),
			"d" (state->edx),
			"S" (state->esi),
			"D" (state->edi));
}
