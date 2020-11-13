#include <stdint.h>

#include <io.h>
#include <keyboard.h>


void keyboard_init()
{
	/* TODO: verifica ACK */

	outb(0x60, 0xED); // LED
	outb(0x60, 0);
	keyboard_poll();

	outb(0x60, 0xF0); // Code set
	outb(0x60, 1);
	keyboard_poll();

	outb(0x60, 0xF3); // typemetric
	outb(0x60, 0);
	keyboard_poll();
	
	outb(0x60, 0xF4);
	outb(0x60, 0);
	keyboard_poll();
}

uint8_t keyboard_poke()
{
	return inb(0x60);
}

uint8_t keyboard_poll()
{
	if (inb(0x64) & 1 == 1)
		return inb(0x60);
	return -1;
	
}