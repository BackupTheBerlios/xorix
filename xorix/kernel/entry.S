
/*
kernel/entry.S @ Xorix Operating System
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

#define ASM 1
#include "task.h"
#include "gdt.h"
#include "cpu.h"
#include "entry.h"

.globl timer
.globl halt_ipi
.globl flush_tlb_ipi
.globl apic_timer
.globl apic_error
.globl spurious_int
.globl dummy_int
.globl boot_syscall
.globl context_switch

#define DO_THREAD \
	GET_CURRENT_THREAD(%edi) \
	cmpl $0,THREAD_NEED_RESCHEDULE(%edi); \
	jne 0f; \
	subl $COUNTER_TICK,THREAD_COUNTER(%edi); \
	jns 0f; \
	movl $-1,THREAD_COUNTER(%edi); \
	movl $1,THREAD_NEED_RESCHEDULE(%edi); \
	0:

.align 4
timer:
		SAVE_ALL

		lock; incl jiffies

		DO_THREAD

0:		movb $0x20,%al
		outb %al,$0x20
		RESTORE_ALL
		iret

.align 4
halt_ipi:
		cli
		lock; decl wait_halt;
0:		jmp 0b

.align 4
flush_tlb_ipi:
		pushl %eax
		mov %cr3,%eax
		mov %eax,%cr3
		lock; decl wait_flush_tlb;
		movl v_apic_eoi_ptr,%eax
		movl $0,(%eax)
		popl %eax
		iret

.align 4
apic_timer:
		SAVE_ALL

		DO_THREAD

		movl v_apic_eoi_ptr,%eax
		movl $0,(%eax)
		RESTORE_ALL
		iret

.align 4
apic_error:
		SAVE_ALL
		call do_apic_error
		RESTORE_ALL
		iret

.align 4
spurious_int:
		SAVE_ALL
		call do_spurious_int
		RESTORE_ALL
		iret

.align 4
dummy_int:
		iret

.align 4
boot_syscall:
		cmp $0,%eax
		jne 0f
		mov $boot_errno,%eax
		jmp 1f
0:		mov $-38,%eax
1:		iret

.align 4
context_switch:
		pushfl
		pushl %ebp
		pushl %esi
		pushl %edi
		pushl %ebx

		cli

		GET_CURRENT_THREAD(%esi)
		movl %esp,THREAD_ESP(%esi)

		movl 24(%esp),%edi
		movl THREAD_CR3(%edi),%eax
		movl %eax,%cr3
		movl THREAD_ESP(%edi),%esp
		movl $0,THREAD_NEED_RESCHEDULE(%edi)

		movl THREAD_TSS(%esi),%eax
		movl THREAD_ESP0(%edi),%ebx
		movl %ebx,TSS_ESP0(%eax)
		movl %eax,THREAD_TSS(%edi)

		popl %ebx
		popl %edi
		popl %esi
		popl %ebp
		popfl
		ret

.comm jiffies, 4
.comm wait_halt, 4
.comm wait_flush_tlb, 4
.comm boot_errno, 4
