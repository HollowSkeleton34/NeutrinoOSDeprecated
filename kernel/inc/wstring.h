
#ifndef __WSTRING
#define __WSTRING

typedef unsigned int size_t;

void *memcpyw(void *dest, const void *src, size_t num);
int memcmpw(const void* ptr1, const void* ptr2, size_t num);
void *memsetw(void *dest, short val, size_t count);
size_t strlenw(const short *str);

#endif