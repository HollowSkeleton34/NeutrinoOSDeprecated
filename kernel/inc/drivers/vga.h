#ifndef __DRIVER_VGA_TEXT
#define __DRIVER_VGA_TEXT

#include <stdint.h>

typedef struct _color888
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color888;

void vga_init();
void vga_putc(char c);
void vga_setcolor(color888 foreground, color888 background);

#endif // DRIVER_VGA_TEXT