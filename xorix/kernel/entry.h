
/*
kernel/entry.h @ Xorix Operating System
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

#ifndef _ENTRY_H
#define _ENTRY_H

#ifdef ASM

#define SAVE_ALL \
	pushl %eax; \
	pushl %ebp; \
	pushw %gs; \
	pushw %fs; \
	pushw %es; \
	pushw %ds; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movw $KERNEL_DATA_SELEKTOR,%dx; \
	movw %dx,%ds; \
	movw %dx,%es;

#define SAVE_ALL_ERROR \
	xchgl %eax,(%esp); \
	pushl %ebp; \
	pushw %gs; \
	pushw %fs; \
	pushw %es; \
	pushw %ds; \
	pushl %edi; \
	pushl %esi; \
	pushl %edx; \
	pushl %ecx; \
	pushl %ebx; \
	movw $KERNEL_DATA_SELEKTOR,%dx; \
	movw %dx,%ds; \
	movw %dx,%es;

#define RESTORE_ALL \
	popl %ebx; \
	popl %ecx; \
	popl %edx; \
	popl %esi; \
	popl %edi; \
	popw %ds; \
	popw %es; \
	popw %fs; \
	popw %gs; \
	popl %ebp; \
	popl %eax; \
	iret

#endif

#ifndef ASM

struct registers
{
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint16_t ds;
	uint16_t es;
	uint16_t fs;
	uint16_t gs;
	uint32_t ebp;
	uint32_t eax;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint16_t ss;
};

#endif

#define REGISTERS_EBX		0
#define REGISTERS_ECX		4
#define REGISTERS_EDX		8
#define REGISTERS_ESI		12
#define REGISTERS_EDI		16
#define REGISTERS_DS		20
#define REGISTERS_ES		22
#define REGISTERS_FS		24
#define REGISTERS_GS		26
#define REGISTERS_EBP		28
#define REGISTERS_EAX		32
#define REGISTERS_EIP		36
#define REGISTERS_CS		40
#define REGISTERS_EFLAGS	44
#define REGISTERS_ESP		48
#define REGISTERS_SS		52

#ifndef ASM

#define jiffies() jiffies
extern volatile uint32_t jiffies;

extern volatile int wait_halt;
extern volatile int wait_flush_tlb;

void timer();
void halt_ipi();
void flush_tlb_ipi();
void apic_timer();
void apic_error();
void spurious_int();
void dummy_int();
void boot_syscall();

void context_switch(void *task);

#endif

#endif
