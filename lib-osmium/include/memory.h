/*
 * memory.h
 *
 *  Created on: 30.11.2015
 *      Author: Fabian
 */

#ifndef LIB_OSMIUM_INCLUDE_MEMORY_H_
#define LIB_OSMIUM_INCLUDE_MEMORY_H_

#include "stdint.h"

void*	palloc();
void*	pcalloc(uint32_t pages);
void	pfree(void* page);

#endif /* LIB_OSMIUM_INCLUDE_MEMORY_H_ */
