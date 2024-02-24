#include <multiboot.h>
#include <stdio.h>
#include <hcf.h>
#include <vga.h>

#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

void putpixel(unsigned char* addr, int pos_x, int pos_y, unsigned char VGA_COLOR)
{
    unsigned char* location = addr + 320 * pos_y + pos_x;
    *location = VGA_COLOR;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
    /* There's an outb %al, $imm8 encoding, for compile-time constant port numbers that fit in 8b. (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

#define PORT 0x3f8          // COM1
 
static int init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(PORT + 0) != 0xAE) {
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}

int is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}
 
void write_serial(char a) {
   while (is_transmit_empty() == 0);
 
   outb(PORT,a);
}

void serial_puts(const char* s) {
	do {
		write_serial(*s);
	} while(*++s != '\0');
}

void sprintf(const char* fmt, ...) {
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
				write_serial(va_arg(args, int));

			size_t len = strlen(out);
			if(width > len) {
				char padding[256];
				size_t j = 0;
				for(; j < width - len; j++)
					padding[j] = padding_char;
				padding[j] = '\0';
				serial_puts(padding);
			}

			serial_puts(out);
			break;
		default:
			write_serial(fmt[i]);
			break;
		}
	}

	va_end(args);
}

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kernel_main(uint32_t magic, multiboot_info_t* mbi) {
	init_vga();
	init_serial();

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printf("Invalid bootloader magic: %x", magic);
		hcf();
	}

	printf("flags: 0x%x\n", mbi->flags);

	uint32_t mem_total = mbi->mem_upper - mbi->mem_lower;
	printf("mem_lower: %uKB, mem_upper: %uKB, mem_total: %uKB\n", mbi->mem_lower, mbi->mem_upper, mem_total);

	//sprintf("checking for flag");
//if (CHECK_FLAG (mbi->flags, 12))
    //{
	//sprintf("flag present");
      multiboot_uint32_t color;
      unsigned i;
      void *fb = (void *) (unsigned long) mbi->framebuffer_addr;

sprintf("fb_addr: %x, fb_width: %u, fb_height: %u", fb, mbi->framebuffer_width, mbi->framebuffer_height);

      switch (mbi->framebuffer_type)
        {
        case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
          {
		  //sprintf("indexed");
            unsigned best_distance, distance;
            struct multiboot_color *palette;
            
            palette = (struct multiboot_color *) mbi->framebuffer_palette_addr;

            color = 0;
            best_distance = 4*256*256;
            
            for (i = 0; i < mbi->framebuffer_palette_num_colors; i++)
              {
                distance = (0xff - palette[i].blue) * (0xff - palette[i].blue)
                  + palette[i].red * palette[i].red
                  + palette[i].green * palette[i].green;
                if (distance < best_distance)
                  {
                    color = i;
                    best_distance = distance;
                  }
              }
          }
          break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
          color = ((1 << mbi->framebuffer_blue_mask_size) - 1) 
            << mbi->framebuffer_blue_field_position;
          break;

        case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
          color = '\\' | 0x0100;
          break;

        default:
          color = 0xffffffff;
          break;
        }
      for (i = 0; i < mbi->framebuffer_width
             && i < mbi->framebuffer_height; i++)
        {
          switch (mbi->framebuffer_bpp)
            {
            case 8:
              {
                multiboot_uint8_t *pixel = fb + mbi->framebuffer_pitch * i + i;
                *pixel = color;
              }
              break;
            case 15:
            case 16:
              {
                multiboot_uint16_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 2 * i;
                *pixel = color;
              }
              break;
            case 24:
              {
                multiboot_uint32_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 3 * i;
                *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
              }
              break;

            case 32:
              {
                multiboot_uint32_t *pixel
                  = fb + mbi->framebuffer_pitch * i + 4 * i;
                *pixel = color;
              }
              break;
            }
        }
    //}

	hcf();
}
