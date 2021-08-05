#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <system/system.h>
#include <system/sys_kern.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

static bool shift = false;
static bool ctrl = false;
static bool alt = false;
static bool caps = false;
static char key_char = 0;
static uint32_t key_code = 0;

// US Keyboard Layout
unsigned char kbdus[256] = 
"??1234567890-=\b\tqwertyuiop[]\n?asdfghjkl;'`?\\zxcvbnm,./?*? ??????????????????????????????????????????????????????????????????????"
"??!@#$%^&*()_+\b\tQWERTYUIOP{}\n?ASDFGHJKL:\"~?|ZXCVBNM<>??*? ??????????????????????????????????????????????????????????????????????";

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);

	switch(scancode)
	{
		case(0x2A): shift = true; return;
		case(0x36):	shift = true; return;
		case(0x1D): ctrl = true; return;
		case(0x38): alt = true; return;
		case(0x3A): caps = !caps; return;
		case(0xAA): shift = false; return;
		case(0xB6): shift = false; return;
		case(0x9D): ctrl = false; return;
		case(0xB8): alt = false; return;
	}
	
	key_code = scancode;
	
	if(scancode < 128)
	{
		if(shift)
		{
			if(caps && isalpha(kbdus[scancode]))
				key_code = scancode;
			else
				key_code = scancode + 128;
		}
		else if(caps && isalpha(kbdus[scancode]))
		{
			key_code = scancode + 128;
		}
		else
		{
			key_code = scancode;
		}
		
		key_char = kbdus[key_code];
		putc(key_char);
	}
}

/* Installs the keyboard handler into IRQ1 */
void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}
