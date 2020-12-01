#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <keyboard.h>
#include <io.h>
#include <string.h>
#include <gdt.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize(void) 
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 
void terminal_setcolor(uint8_t color) 
{
	terminal_color = color;
}
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void update_cursor(int x, int y)
{
	uint16_t pos = y * VGA_WIDTH + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void terminal_putchar(char c) 
{
	if (c == '\t') {
		terminal_column = terminal_column + 4;
	}
	else if (c == '\n') {
		++terminal_row;
		terminal_column = -1;
	}
	else if (c == '\b') {
		--terminal_column;
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);   
		--terminal_column;
	}
	else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
	}
	

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		++terminal_row;
	}
	if (terminal_row == VGA_HEIGHT)
		terminal_row = 0;
	update_cursor(terminal_column, terminal_row);
}
 
void terminal_write(const char* data, size_t size) 
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) 
{
	terminal_write(data, strlen(data));
}
void keyword_function(const char* keyword, uint8_t size)
{
	char taci[50];
	taci[0]='t';
	taci[1]='a';
	for (size_t i = 0; i < size; i++)
                {       
                      terminal_putchar(keyword[i]);
		}
	if(size != 0) if(keyword[0] == taci[0] && keyword[1] == taci[1]) terminal_write(keyword, strlen(keyword));
}
void terminal_readstring(char c)
{
	static uint8_t length = 0;
	static char keyword[50];
	keyword[0] = c;
	if(c == ' ' || c == '\t' || c == '\n'){
		keyword_function(keyword, length);
		for (size_t i = 0; i < length; i++)
		{	
			//terminal_putchar(keyword[i]);
			keyword[i] = ' ';
		}
			length = 0;
	}
	else{
		keyword[length] = c;
		//terminal_putchar(keyword[length]);
		length++;
	}
}
void xprint(unsigned long long x, int biti)
{
	for (int i = biti - 1; i >= 0; i--)
		terminal_putchar('0' + ((x >> i) & 1));
	terminal_putchar('\n');
}


#define xprint(X) xprint(X, 8)

extern struct gdt_entry gdt_entries[3];

void kernel_main(void) 
{
	/* Initialize terminal interface */
	terminal_initialize();
	keyboard_init();
	// init_gdt();
	// idt_init();
	for(;;)
	{
		char key;
		key = keyboard_showKey();
		if(key != 0x00)  
		{
			terminal_putchar(key);
			//terminal_readstring(key);
			//terminal_putchar(1);
			//terminal_writestring("-");
			//xprint(key);
			//terminal_writestring("\n");
		}
	}
}

__attribute__((interrupt))
void keyboard_interrupt(struct interrupt_frame *f)
{
	(void) f;

	terminal_writestring("Am primit un interrupt de la tastatura\nCe tare!\n");
}
