#include <system/multithreading.h>

void print_test_a()
{
    for (int i = 0; i < 50; ++i)
    {
        printf("A");
    }

    printf("\n");
}

void print_test_b()
{
    for (int i = 0; i < 50; ++i)
    {
	printf("B");
    }

    printf("\n");
}

void add_tests(task_manager* manager)
{
    //task_create(0x00, 0x00, &print_test_a
}
