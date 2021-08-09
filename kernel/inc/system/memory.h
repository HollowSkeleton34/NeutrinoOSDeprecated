
#ifndef __SYSTEM_MEMORY
#define __SYSTEM_MEMORY

#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

void memory_init();
void* malloc(uint64_t size);
void free(void* pointer);

#endif