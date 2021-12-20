#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdint.h>

/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
}__attribute__((packed));

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_install();
void irq_send_eoi(uint8_t irq);

#define PUSHA() asm volatile("add $0x1c, %esp"); \
		        asm volatile("pusha");

#define POPA()  asm volatile("popa"); \
	            asm volatile("iret");

#endif
