
/*
kernel/smpboot.S @ Xorix Operating System
Copyright (C) 2001-2002 Ingmar Friedrichsen <ingmar@xorix.org>

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
#include "idt.h"
#include "smpboot.h"

.data
.code16

.globl smpboot_start
.globl smpboot_end

/*
 *	SMP-Boot-Page fuer den Real Mode...
 */

smpboot_start:
r_base = .
	cli

	movw %cs,%ax
	movw %ax,%ds

	lgdt gdtr - r_base

	movw $1,%ax
	lmsw %ax

	ljmpl $8,$pmode

gdtr:
	.word GDT_LIMIT
	.long gdt
smpboot_end:

/*
 *	SMP Protected Mode Initialiesierung...
 */

.text
.code32
pmode:
	movw $16,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs
	movw %ax,%gs
	movw %ax,%ss

	lidt idtr

	movl stack_for_ap,%esp
	sti

	call ap_setup

0:	jmp 0b

.data

idtr:
	.word IDT_LIMIT
	.long idt

.comm stack_for_ap,4
