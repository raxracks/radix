#ifndef __VGA_H__
#define __VGA_H__
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

struct vga_terminal {
	uint8_t bg;
	uint8_t fg;
	uint8_t x;
	uint8_t y;
};

extern struct vga_terminal vga_term;

void init_vga(void);
void vga_putc(char ch);

#endif
