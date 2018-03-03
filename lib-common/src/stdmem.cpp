#include "stdmem.h"

void* memset(void* buf, int c, size_t n) {
	unsigned char* p = buf;

	while (n--) {
		*p++ = c;
	}

	return buf;
}

void* memcpy(void* dest, const void* src, size_t n) {
	unsigned char* d = dest;
	const unsigned char* s = src;

	if (n == 0)
		return 0;

	while (n--) {
		*(d++) = *(s++);
	}

	return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
	if (ptr1 == 0)
		return -1;
	if (ptr2 == 0)
		return 1;
	if (num == 0)
		return 0;

	const uint8_t* pa = ptr1;
	const uint8_t* pb = ptr2;

	for (uint32_t i = 0; i < num; i++) {
		if (pa[i] != pb[i]) {
			return pa[i] - pb[i];
		}
	}
	return 0;
}
