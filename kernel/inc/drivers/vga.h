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
void vga_fillrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t c);

#endif // DRIVER_VGA_TEXT