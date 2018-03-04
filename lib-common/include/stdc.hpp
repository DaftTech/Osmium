/*
 * new.h
 *
 *  Created on: 04.03.2018
 *      Author: fabia
 */

#ifndef LIB_COMMON_INCLUDE_STDC_HPP_
#define LIB_COMMON_INCLUDE_STDC_HPP_

#include "stdlib.h"

inline void* operator new(size_t size) {
	return calloc(1, size);
}

inline void operator delete(void* ptr) {
	free(ptr);
}

#endif /* LIB_COMMON_INCLUDE_STDC_HPP_ */
