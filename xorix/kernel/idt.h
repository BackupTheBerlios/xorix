
/*
kernel/idt.h @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>

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

#ifndef _IDT_H
#define _IDT_H

#define IDT_ENTRIES	256
#define IDT_LIMIT	2047

#define V_IRQ(irq)	(0x20 + (irq))
#define V_HALT_IPI	0x3b
#define V_FLUSH_TLB_IPI	0x3c
#define V_APIC_TIMER	0x3d
#define V_APIC_ERROR	0x3e
#define V_SPURIOUS_INT	0x3f
#define V_SYSCALL	0x80

#define INT_USER	1
#define INT_INTS	2

#ifndef ASM

struct idt_deskriptor
{
	uint16_t offset_0_15;
	uint16_t selektor;
	uint8_t reserved;
	uint8_t type;
	uint16_t offset_16_31;
};

extern struct idt_deskriptor idt[];

void set_interrupt(uint8_t vector, void *handler, int flags);
void idt_init();

#endif

#endif


