#include <stdio.h>
#include <drivers/vga.h>
#include <stdarg.h>

void putc(char c)
{
	vga_putc(c);
}

void puts(const char *str)
{
    while (*str != '\0'){
        putc(*str++);
    }
}

void puti(unsigned int i, int base)
{
	static char table[] = "0123456789ABCDEF";
	
	do
	{
		putc(table[i % base]);
		i /= base;
	} while(i != 0);
}

void printf(const char *format, ...)
{
	char* pointer = format;
	unsigned int i;
	char* s;
	
	va_list arg;
	va_start(arg, format);
	
	while(*pointer != 0)
	{
		while(*pointer != '%' && *pointer != 0)
		{
			putc(*pointer);
			pointer ++;
		}
		
		if(*pointer == 0) break;
		
		pointer ++;
		
		switch(*pointer)
		{
			case 'c':
				i = va_arg(arg, char);
				putc(i);
				break;
			case 'd':
				i = va_arg(arg, int);
				if(i < 0) putc('-');
				puti(i, 10);
				break;
			case 'o':
				i = va_arg(arg, int);
				puti(i, 8);
				break;
			case 's':
				s = va_arg(arg, char*);
				puts(s);
				break;
			case 'x':
				i = va_arg(arg, unsigned int);
				puti(i, 16);
				break;
		}
		
		pointer ++;
	}
	
	va_end(arg);
}