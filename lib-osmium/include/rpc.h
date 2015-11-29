#ifndef RPC_H
#define RPC_H

#include "stdint.h"

void rpc_return(int returnCode);
void* rpc_map(uint32_t* rpcID);
void rpc_init();
void rpc_handler();

#endif
