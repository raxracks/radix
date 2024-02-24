#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <vga.h>

void putc(char ch) {
	vga_putc(ch);
}

void puts(const char* str) {
	do {
		putc(*str);
	} while(*++str != '\0');
}

void printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);	

	for(int i = 0; fmt[i] != '\0'; i++)
	{
		switch(fmt[i])
		{
		case '%':
			i++;

			char buf[512];

			size_t width = 0;
			char padding_char = fmt[i];
			if(isdigit(fmt[i]) || fmt[i] == ' ') {
				i++;
				int pos = 0;
				while(isdigit(fmt[i])) {
					buf[pos++] = fmt[i++];
				}
				buf[pos] = '\0';
				width = atoi(buf);
			}

			const char* out = 0;

			if(fmt[i] == 's')
				out = va_arg(args, const char*);
			if(fmt[i] == 'd')
				out = itoa(va_arg(args, int), buf, 10);
			if(fmt[i] == 'u')
				out = ultoa(va_arg(args, uint32_t), buf, 10);
			if(fmt[i] == 'x')
				out = ultoa(va_arg(args, uint32_t), buf, 16);
			if(fmt[i] == 'c')
				putc(va_arg(args, int));

			size_t len = strlen(out);
			if(width > len) {
				char padding[256];
				size_t j = 0;
				for(; j < width - len; j++)
					padding[j] = padding_char;
				padding[j] = '\0';
				puts(padding);
			}

			puts(out);
			break;
		default:
			putc(fmt[i]);
			break;
		}
	}

	va_end(args);
}
