
/*
kernel/idt.c @ Xorix Operating System
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

#include <xorix.h>

#include "debug.h"
#include "entry.h"
#include "cpu.h"
#include "gdt.h"
#include "idt.h"

struct idt_deskriptor idt[IDT_ENTRIES];

/*
 *	set_interrupt() installiert einen Interrupt-Handler in der IDT...
 */

void set_interrupt(uint8_t vector, void *handler, int flags)
{
	struct idt_deskriptor *deskriptor;
	uint8_t type;

	if(flags & INT_INTS) type = 0x8f;
	else type = 0x8e;
	if(flags & INT_USER) type |= 0x60;

	deskriptor = &idt[vector];
	deskriptor->offset_0_15 = ((uint32_t) handler) & 0xFFFF;
	deskriptor->selektor = KERNEL_CODE_SELEKTOR;
	deskriptor->reserved = 0;
	deskriptor->type = type;
	deskriptor->offset_16_31 = (((uint32_t) handler) >> 16) & 0xFFFF;

	return;
}

#include "console.h"
void xdummy_handler()
{
	//asm("mov $0x10,%ax; mov %ax,%ds;");
	kernel_panic("!!!...!!!");
}

void e0() {kernel_panic("#DE");}
void e1() {kernel_panic("#DB");}
void e2() {kernel_panic("#2");}
void e3() {kernel_panic("#BP");}
void e4() {kernel_panic("#OF");}
void e5() {kernel_panic("#BR");}
void e6() {kernel_panic("#UD");}
void e7() {kernel_panic("#NM");}
void e8() {kernel_panic("#DF");}
void e9() {kernel_panic("#9");}
void e10() {kernel_panic("#TS");}
void e11() {kernel_panic("#NP");}
void e12() {kernel_panic("#SS");}
void e13() {kernel_panic("#GP");}
void e14() {kernel_panic("#PF");}
void e15() {kernel_panic("#15");}
void e16() {kernel_panic("#MF");}
void e17() {kernel_panic("#AC");}
void e18() {kernel_panic("#MC");}
void e19() {kernel_panic("#XF");}

void atxxx()
{
wprintk(L"#");
while(1);
return;
}

void idt_init()
{
	struct
	{
		uint16_t dummy;
		uint16_t limit;
		uint32_t base;
	} idtr;
	int i;

	// IDT initialisieren...

	for(i = 0; i < 256; i++)
	{
		set_interrupt(i, dummy_int, INT_INTS);
	}

	// Boot System-Calls...

	set_interrupt(V_SYSCALL, boot_syscall, INT_INTS);

	// Timer-Interrupt...

	set_interrupt(V_IRQ(0), timer, 0);

	// APIC und SMP Interrupts...

	set_interrupt(V_HALT_IPI, halt_ipi, 0);
	set_interrupt(V_FLUSH_TLB_IPI, flush_tlb_ipi, 0);
	set_interrupt(V_APIC_TIMER, apic_timer, 0);
	set_interrupt(V_APIC_ERROR, apic_error, 0);
	set_interrupt(V_SPURIOUS_INT, spurious_int, 0);

	// Exception Debug...

	set_interrupt(0, e0, 0);
	set_interrupt(1, e1, 0);
	set_interrupt(2, e2, 0);
	set_interrupt(3, e3, 0);
	set_interrupt(4, e4, 0);
	set_interrupt(5, e5, 0);
	set_interrupt(6, e6, 0);
	set_interrupt(7, e7, 0);
	set_interrupt(8, e8, 0);
	set_interrupt(9, e9, 0);
	set_interrupt(10, e10, 0);
	set_interrupt(11, e11, 0);
	set_interrupt(12, e12, 0);
	set_interrupt(13, e13, 0);
	set_interrupt(14, e14, 0);
	set_interrupt(15, e15, 0);
	set_interrupt(16, e16, 0);
	set_interrupt(17, e17, 0);
	set_interrupt(18, e18, 0);
	set_interrupt(19, e19, 0);

	// IDT laden...

	idtr.limit = IDT_LIMIT;
	idtr.base = (uint32_t) idt;
	asm volatile("lidt (%%eax)"::"a" (&idtr.limit));

	// sti

	STI();

	return;
}


