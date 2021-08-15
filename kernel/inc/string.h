#include <stdint.h>

typedef unsigned int size_t;

void *memcpy(void *dest, const void *src, size_t num);
int memcmp(const void* ptr1, const void* ptr2, size_t num);
void *memset(void *dest, char val, size_t count);
void* smemset(void* dest, void* value, size_t count, size_t size);
size_t strlen(const char *str);

#define NULL