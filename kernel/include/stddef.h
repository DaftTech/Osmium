#ifndef _STDDEF_H_
#define _STDDEF_H_
#include "cdefs.h"
#include "stdint.h"

#ifndef __cplusplus
	#define NULL (void*) 0
#else
	#define NULL 0
#endif

typedef uint32_t size_t;

typedef uint32_t PADDR;
typedef uint32_t ADDRESS;

#endif
