#include <stdint.h>

#ifndef __CPORTS
#define __CPORTS

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t data);
uint16_t inportw(uint16_t port);
void outportw(uint16_t port, uint16_t data);
uint32_t inportd(uint16_t port);
void outportd(uint16_t port, uint32_t data);

#endif