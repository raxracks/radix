#include <vga.h>
#include <stdio.h>

struct vga_terminal vga_term = { 0 };
uint16_t* VGA = (uint16_t*)0xb8000;

void vga_putc(char ch) {
	if(ch == '\n') {
		vga_term.x = 0;
		vga_term.y++;
	} else {
		int index = vga_term.y * VGA_WIDTH + vga_term.x;

		VGA[index] = ch | (vga_term.fg | vga_term.bg << 4) << 8;

		vga_term.x++;
		if(vga_term.x > VGA_WIDTH) {
			vga_term.x = 0;
			vga_term.y++;
		}
	}
}

void vga_clear() {
	vga_term.x = 0;
	vga_term.y = 0;
	for(int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		putc(' ');
}

void init_vga(void) {
	vga_term.fg = 7;
	vga_term.bg = 0;

	vga_clear();

	vga_term.x = 0;
	vga_term.y = 0;
}
