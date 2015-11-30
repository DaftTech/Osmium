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

void*        memset(void* buf, int c, uint32_t n);
void*        memcpy(void* dest, const void* src, uint32_t n);
int          memcmp(const void* ptr1, const void* ptr2, uint32_t num);

#endif /* LIB_OSMIUM_INCLUDE_MEMORY_H_ */
