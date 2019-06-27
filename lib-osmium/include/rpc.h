#ifndef RPC_H
#define RPC_H

#include "stdint.h"

typedef uint32_t FUTURE;

void 	rpc_return(int returnCode);
void 	rpc_map(uint32_t* rpcID, void** rpcDat, uint32_t* rpcSize);
int 	rpc_register_handler(int(*fptr)(int));
int		rpc_check_future(FUTURE fut);

#endif
