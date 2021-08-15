#include <string.h>

// copying ----------------------------------------------------------

void *memcpy(void *dest, const void *src, size_t num)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; num != 0; num--) *dp++ = *sp++;
    return dest;
}

// comparison -------------------------------------------------------

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
    char *t1 = (char *)ptr1;
    char *t2 = (char *)ptr2;
    for(; num != 0; num--) 
    {
        if(*t1 != *t2) return *t1 - *t2;
        t1 ++;
        t2 ++;
    }
    return 0;
}

// other-------------------------------------------------------------

void *memset(void *dest, char val, size_t count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

void* smemset(void* dest, void* value, size_t count, size_t size)
{
    char *temp1 = (char *)dest;
    char *temp2 = (char *)value;
    for(; count != 0; count --)
        for(size_t tempsize = 0; tempsize != size; tempsize ++)
            *temp1++ = temp2[tempsize];
    return dest;
}

size_t strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}