#include <stdio.h>
#include <drivers/vga.h>
#include <drivers/keyboard.h>
#include <drivers/timer.h>
#include <drivers/ata.h>
#include <system/sys_kern.h>
#include <system/memory.h>
#include <system/multithreading.h>
#include <system/tests.h>

#define KERNEL_RELEASE  0
#define KERNEL_VERSION  3
#define KERNEL_COMMIT   43

int main()
{
	//vga_init_video();
    vga_init();
    memory_init();

    printf("Entered Kernel\n");

	idt_install();
    isrs_install();
    irq_install();


	timer_install();
    keyboard_install();

    ata_install();

    system_task_manager = create_task_manager();

    printf("Running NeutrinoOS Version %d.%d.%d!\n", KERNEL_RELEASE, KERNEL_VERSION, KERNEL_COMMIT);

    // This variable is for determining whether or not ESP's position has been saved before the task
    // manager runs
    stack_set = 0;
    //vga_fillrect(640, 512, 640, 512, 0xFFFF0000);
    //vga_fillrect(640, 512, 640, 512, 0x7F0000FF);

    //for(int i = 0; i < 200; i ++)
    //{
       // printf("Test scroll: %u\n", i);
    //}

    //vga_fillrect(640, 512, 640, 512, 0x7FFF0000);
    add_tests(system_task_manager);

    __asm__ __volatile__ ("sti");

    return 0;
}
