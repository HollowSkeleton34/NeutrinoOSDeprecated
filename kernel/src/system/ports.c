#include <system/ports.h>
#include <system/sys_kern.h>

// ---------------------------------------------------
//
//  port io - byte (8 bits)
//
// ---------------------------------------------------

uint8_t inportb(uint16_t port)
{
    uint8_t result;
    __asm__ volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}


void outportb(uint16_t port, uint8_t data)
{
    __asm__ volatile ("out %%al, %%dx" : : "a" (data), "d" (port));
}

// ---------------------------------------------------
//
//  port io - word (16 bits)
//
// ---------------------------------------------------

uint16_t inportw(uint16_t port)
{
    uint8_t result;
    __asm__ volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}


void outportw(uint16_t port, uint16_t data)
{
    __asm__ volatile ("out %%ax, %%dx" : : "a" (data), "d" (port));
}

// ---------------------------------------------------
//
//  port io - dword (32 bits)
//
// ---------------------------------------------------

uint32_t inportd(uint16_t port)
{
    uint8_t result;
    __asm__ volatile ("in %%dx, %%eax" : "=a" (result) : "d" (port));
    return result;
}


void outportd(uint16_t port, uint32_t data)
{
    __asm__ volatile ("out %%eax, %%dx" : : "a" (data), "d" (port));
}