
/*
kernel/task.h @ Xorix Operating System
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

#ifndef _TASK_H
#define _TASK_H

#define THREAD_UNREADY		0
#define THREAD_READY		1
#define THREAD_IDLE		2

#define MAX_PRIORITY		100
#define COUNTER_TICK		10
#define COUNTER_SUSPEND		5

#ifndef ASM
struct thread
{
	tss_t *tss;
	uint32_t cr3;
	uint32_t esp0;
	uint32_t esp;

	volatile uint32_t need_reschedule;
	volatile int32_t counter;

	struct thread *next;
	struct thread *prev;

	struct thread *q_next;

	int state;
	int priority;
};
#endif

#define THREAD_TSS		0
#define THREAD_CR3		4
#define THREAD_ESP0		8
#define THREAD_ESP		12
#define THREAD_NEED_RESCHEDULE	16
#define THREAD_COUNTER		20

#ifndef ASM

void schedule();
void idle();
void create_kernel_thread(void *function);
void task_init();
void ap_task_init();

static inline struct thread *get_current_thread()
{
	struct thread *thread;

	asm volatile("movl $0xfffff000,%0  \n"
	             "andl %%esp,%0        \n"
	             :"=r" (thread));

	return thread;
}

static inline void offer_reschedule()
{
	struct thread *thread;

	thread = get_current_thread();
	if(thread->need_reschedule == 1)
	{schedule();}

	return;
}

#endif

#ifdef ASM

#define GET_CURRENT_THREAD(reg) \
	movl $0xfffff000,reg; \
	andl %esp,reg;

#endif

#endif




