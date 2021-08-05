#include <wstring.h>

// copying ----------------------------------------------------------

void *memcpyw(void *dest, const void *src, size_t num)
{
    const short *sp = (const short *)src;
    short *dp = (short *)dest;
    for(; num != 0; num--) *dp++ = *sp++;
    return dest;
}

// comparison -------------------------------------------------------

int memcmpw(const void* ptr1, const void* ptr2, size_t num)
{
    short *t1 = (short *)ptr1;
    short *t2 = (short *)ptr2;
    for(; num != 0; num--) 
    {
        if(*t1 != *t2) return *t1 - *t2;
        t1 ++;
        t2 ++;
    }
    return 0;
}

// other-------------------------------------------------------------

void *memsetw(void *dest, short val, size_t count)
{
    short *temp = (short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

size_t strlenw(const short *str)
{
    size_t retval;
    for(retval = 0; *str != L'\0'; str++) retval++;
    return retval;
}