
/*
kernel/multiboot.S @ Xorix Operating System
Copyright (C) 2002 Ingmar Friedrichsen <ingmar@xorix.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define ASM 1
#include "gdt.h"
#include "multiboot.h"

.globl _start,multiboot_info

/*
 *	_start
 */

_start:
		// Mutiboot testen...

		cmp $MULTIBOOT_BOOTLOADER_MAGIC,%eax
0:		jne 0b
		movl %ebx,(multiboot_info)

		// Stack initialisieren...

		mov $kernel_stack+4096,%esp

		// 1. und 2. Interrupt-Controller initialisieren.
		// (IRQ0 - IRQ15 zu INT 20 - INT 2F)

		movb $0x11,%al
		outb %al,$0x20
		outb %al,$0xA0

		movb $0x20,%al
		outb %al,$0x21
		movb $0x28,%al
		outb %al,$0xA1

		movb $0x04,%al
		outb %al,$0x21
		movb $0x02,%al
		outb %al,$0xA1

		movb $0x01,%al
		outb %al,$0x21
		outb %al,$0xA1

		movb $0x00,%al
		outb %al,$0x21
		outb %al,$0xA1

		// Timer initialisieren.

		movb $0x36,%al
		outb %al,$0x43
		movb $0x9B,%al
		outb %al,$0x40
		movb $0x2E,%al
		outb %al,$0x40

		// setup() aufrufen...

		call setup

0:		jmp 0b

/*
 *	Multiboot header
 */

		.align 4

multiboot_header:
		.long MULTIBOOT_HEADER_MAGIC
		.long MULTIBOOT_HEADER_FLAGS
		.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

/*
 *	Multiboot-Information-Pointer
 */

.comm multiboot_info,4

/*
 *	BSP-TSS
 */

.comm bsp_tss,4096,4096

/*
 *	Kernel Stack
 */

.comm kernel_stack,4096,4096

/*
 *	IDLE Stack
 */

.comm idle_stack,4096,4096
