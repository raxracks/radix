extern kernel_main

MBALIGN		equ 1 << 0
MEMINFO		equ 1 << 1
VIDINFO		equ 1 << 2
MBFLAGS		equ MBALIGN | MEMINFO | VIDINFO
MAGIC		equ 0x1BADB002
CHECKSUM 	equ -(MAGIC + MBFLAGS)

section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM
	dd 0, 0, 0, 0, 0
	dd 0
	dd 320, 200, 8

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	push ebx
	push eax
	
	call kernel_main
	cli
.hang:
	jmp .hang
.end:
