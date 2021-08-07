#include <stdio.h>
#include <drivers/vga.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <system/sys_kern.h>
#include <system/memory.h>
#include <system/disk.h>

#define KERNEL_RELEASE 0
#define KERNEL_VERSION 0
#define KERNEL_SNAPSHOT 1

int main(){
	vga_init_video();
	idt_install();
    isrs_install();
    irq_install();
	//timer_install();
    keyboard_install();

    load_drives();
    memory_init();
	
	printf("Running TMOS Version %d.%d.%d!\n", KERNEL_RELEASE, KERNEL_VERSION, KERNEL_SNAPSHOT);
    __asm__ __volatile__ ("sti");

    return 0;
}