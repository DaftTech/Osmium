#ifndef _PORTS_H_
#define _PORTS_H_

#include "stdint.h"

uint8_t  inb(uint16_t _port);
uint16_t inw(uint16_t _port);
uint32_t inl(uint16_t _port);

void outb(uint16_t _port, uint8_t _data);
void outw(uint16_t _port, uint16_t _data);
void outl(uint16_t _port, uint32_t _data);
void outb_wait(uint16_t _port, uint8_t _data);

#endif
