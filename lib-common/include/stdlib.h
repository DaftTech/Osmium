/*
 * stdlib.h
 *
 *  Created on: 03.03.2018
 *      Author: fabia
 */

#ifndef LIB_COMMON_INCLUDE_STDLIB_H_
#define LIB_COMMON_INCLUDE_STDLIB_H_

extern void* malloc(size_t size);
extern void* calloc(size_t num, size_t size);
extern void* realloc(void* ptr, size_t size);
extern void free(void* ptr);

#endif /* LIB_COMMON_INCLUDE_STDLIB_H_ */
