#ifndef RPC_H
#define RPC_H

#include "stdint.h"

typedef uint32_t FUTURE;

void 	rpc_return(int returnCode);
void* 	rpc_map(uint32_t* rpcID, uint32_t* rpcARG0);
int 	rpc_register_handler(int(*fptr)(int, void*));
int		rpc_check_future(FUTURE fut);
void    _start();

#endif
