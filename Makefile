KERNEL=obj/radix.bin
ISO=radix.iso

CC=gcc
LD=ld
CFLAGS=-ffreestanding -fno-stack-protector -fno-stack-check -O3 -Wall -Wextra -Werror -m32 -Isrc
LDFLAGS=-T linker.ld -m elf_i386
NASMFLAGS=-f elf32

CFILES=$(shell cd src && find -L * -type f -name '*.c')
ASMFILES=$(shell cd src && find -L * -type f -name '*.asm')
OBJ=$(addprefix obj/,$(CFILES:.c=.c.o) $(ASMFILES:.asm=.asm.o))

$(ISO): $(KERNEL) isolinux.cfg
	cp isolinux.cfg cdroot/isolinux/
	cp $(KERNEL) cdroot/
	mkisofs -o $(ISO) -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table cdroot/

$(KERNEL): makedir linker.ld $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $@

obj/%.c.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
obj/%.asm.o: src/%.asm
	nasm $(NASMFLAGS) $< -o $@

makedir:
	mkdir -p obj

run: $(ISO)
	qemu-system-i386 -boot d -cdrom $(ISO) -m 1G -enable-kvm -cpu host -serial stdio

clean:
	rm -rf $(OBJ) $(KERNEL)
