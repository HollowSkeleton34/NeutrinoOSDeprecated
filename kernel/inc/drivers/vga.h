#ifndef __DRIVER_VGA_TEXT
#define __DRIVER_VGA_TEXT

#include <stdint.h>

// Define all the usable colors
#define COLOR_BLACK 0     // Black
#define COLOR_BLUE 1     // Blue
#define COLOR_GREEN 2     // Green
#define COLOR_CYAN 3     // Cyan
#define COLOR_RED 4     // Red
#define COLOR_PURPLE 5     // Purple
#define COLOR_BROWN 6     // Brown
#define COLOR_GRAY 7     // Gray
#define COLOR_DARK_GRAY 8     // Dark Gray
#define COLOR_LIGHT_BLUE 9     // Light Blue
#define COLOR_LIGHT_GREEN 10    // Light Green
#define COLOR_LIGHT_CYAN 11    // Light Cyan
#define COLOR_LIGHT_RED 12    // Light Red
#define COLOR_LIGHT_PURPLE 13    // Light Purple
#define COLOR_YELLOW 14    // Yellow
#define COLOR_WHITE 15    // White

void vga_move_csr(void);
void vga_scroll();
void vga_cls();
void vga_putc(unsigned char c);
void vga_settextcolor(unsigned char forecolor, unsigned char backcolor);
void vga_init_video(void);

#endif // DRIVER_VGA_TEXT