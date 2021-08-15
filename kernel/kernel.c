#include <stdio.h>
#include <drivers/vga.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/ata.h>
#include <system/sys_kern.h>
#include <system/memory.h>

#define KERNEL_RELEASE  0
#define KERNEL_VERSION  2
#define KERNEL_COMMIT   8

int main(){
	//vga_init_video();
    vga_init();
    memory_init();

    printf("Entered Kernel\n");

	idt_install();
    isrs_install();
    irq_install();


	//timer_install();
    keyboard_install();
    ata_install();
	
	printf("Running NeutrinoOS Version %d.%d.%d!\n", KERNEL_RELEASE, KERNEL_VERSION, KERNEL_COMMIT);

    vga_fillrect(640, 512, 640, 512, 0x7FFF0000);

    __asm__ __volatile__ ("sti");

    return 0;
}