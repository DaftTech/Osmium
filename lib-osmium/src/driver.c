#include "driver.h"
#include "syscall.h"


int register_driver(int dOpenID, int dCloseID, int dRemoveID, int dReadID, int dWriteID) {
	struct regstate state = {
			.eax = 0x300,
			.ebx = dOpenID,
			.ecx = dCloseID, .edx = dRemoveID,
			.esi = dReadID,  .edi = dWriteID };

	syscall(&state);

	return state.eax;
}
