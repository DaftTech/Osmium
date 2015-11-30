#ifndef RPC_H
#define RPC_H

#include "stdint.h"

typedef uint32_t FUTURE;

void 	rpc_return(int returnCode);
void* 	rpc_map(uint32_t* rpcID);
void 	rpc_init();
int 	rpc_register_handler(int(*fptr)(void*));
int		rpc_check_future(FUTURE fut);
void 	rpc_handler();

#endif
