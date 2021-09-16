#ifndef __ALGORITHMS__
#define __ALGORITHMS__

#include <system/multithreading.h>

unsigned int task_manager_binary_search(task_manager* manager, unsigned int start, unsigned int end, unsigned int pid);
unsigned int binary_search(unsigned int arr[], unsigned int start, unsigned int end, unsigned int val);

#endif
