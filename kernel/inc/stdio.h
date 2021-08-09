#ifndef __CSTDIO
#define __CSTDIO

#define panic(...) {printf("***KERNEL PANIC*** in %s at line %d in function: %s\n", __FILE__, __LINE__, __func__); printf(__VA_ARGS__); for(;;);}

void putc(char c);
void puts(const char *str);
void puti(unsigned int i, int base);
int printf(const char *format, ...);

#endif