
/*
kernel/task.c @ Xorix Operating System
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

#include <xorix.h>

#include "debug.h"
#include "entry.h"
#include "memory.h"
#include "spinlock.h"
#include "multiboot.h"
#include "cpu.h"
#include "gdt.h"
#include "apic.h"
#include "smp.h"
#include "task.h"

/* Task */

static spinlock_t task_lock = SPIN_LOCK_UNLOCKED;
static struct thread *first_thread = NULL;
static struct thread *last_thread = NULL;

/* Schedule */

static spinlock_t schedule_lock = SPIN_LOCK_UNLOCKED;
static struct thread *queue_first = NULL;
static struct thread *queue_last = NULL;
static struct thread *idle_first = NULL;

static inline void add_thread(struct thread *thread)
{
	spin_lock(&task_lock);

	if(first_thread == NULL)
	{
		thread->next = thread;
		thread->prev = thread;
		first_thread = thread;
		last_thread = thread;
	}
	else
	{
		thread->next = first_thread;
		thread->prev = last_thread;
		first_thread->prev = thread;
		last_thread->next = thread;
		last_thread = thread;
	}

	spin_unlock(&task_lock);

	return;
}

static inline void del_thread(struct thread *thread)
{
	spin_lock(&task_lock);

	if(first_thread == last_thread)
	{
		first_thread = NULL;
		last_thread = NULL;
	}
	else
	{
		thread->next->prev = thread->prev;
		thread->prev->next = thread->next;
		if(last_thread == thread) last_thread = thread->prev;
		else if(first_thread == thread) first_thread = thread->next;
	}

	spin_unlock(&task_lock);

	return;
}

void schedule()
{
	struct thread *ot;
	struct thread *nt;

	ot = get_current_thread();
	spin_lock(&schedule_lock);

	// Alten Thread ans Ende der Queue packen...

	if(ot->state != THREAD_UNREADY)
	{
		disable_interrupts();
		ot->counter = ot->priority;
		ot->need_reschedule = 1;
		enable_interrupts();

		// War es ein normaler Thread?

		if(ot->state == THREAD_READY)
		{
			if(queue_first == NULL)
			{
				ot->q_next = NULL;
				queue_first = ot;
				queue_last = ot;
			}
			else
			{
				ot->q_next = NULL;
				queue_last->q_next = ot;
				queue_last = ot;
			}
		}

		// Nein, ein IDLE-Thread!

		else
		{
			ot->q_next = idle_first;
			idle_first = ot;
		}
	}

	// Neuen Thread aus der Queue holen...

	if(queue_first != NULL)
	{
		nt = queue_first;
		queue_first = nt->q_next;
	}

	// Wenn keiner da, IDLE-Thread holen...

	else
	{
		if(idle_first == NULL) BUG();

		nt = idle_first;
		idle_first = nt->q_next;
	}

	// Context Switch!!!

	spin_unlock(&schedule_lock);
	if(nt != ot) context_switch(nt);

	return;
}

/*
 *	idle() ist der IDLE-Thread...
 */

void idle()
{
	while(1)
	{
		wprintk(L"IDLE            @ CPU%u\n", use_smp == true ? get_apic_id() : 0);
		schedule();
	}

	return;
}

static inline void suspend_thread()
{
	struct thread *thread;
	thread = get_current_thread();
	
	disable_interrupts();
	thread->counter -= COUNTER_SUSPEND;
	thread->need_reschedule = 1;
	enable_interrupts();

	spin_lock(&schedule_lock);
	thread->state = THREAD_UNREADY;
	spin_unlock(&schedule_lock);

	schedule();

	return;
}

static inline void resume_thread(struct thread *thread)
{
	spin_lock(&schedule_lock);

	if(thread->state == THREAD_UNREADY)
	{
		thread->state = THREAD_READY;

		if(thread->counter > 0)
		{
			if(queue_first == NULL)
			{
				thread->q_next = NULL;
				queue_first = thread;
				queue_last = thread;
			}
			else
			{
				thread->q_next = queue_first;
				queue_first = thread;
			}
		}
		else
		{
			if(queue_first == NULL)
			{
				thread->q_next = NULL;
				queue_first = thread;
				queue_last = thread;
			}
			else
			{
				thread->q_next = NULL;
				queue_last->q_next = thread;
				queue_last = thread;
			}
		}
	}

	spin_unlock(&schedule_lock);

	return;
}

void create_kernel_thread(void *function)
{
	unsigned long page;
	struct thread *thread;
	uint32_t esp0;
	uint32_t *esp;

	page = get_free_page(GFP_KERNEL);
	thread = PAGE2PTR(page);
	esp0 = PAGE2BYTE(page + 1);
	esp = (uint32_t *) (esp0 - (6 * sizeof(uint32_t)));

	thread->esp0 = esp0;
	thread->esp = (uint32_t) esp;

	thread->state = THREAD_UNREADY;

	thread->need_reschedule = 0;
	thread->priority = MAX_PRIORITY;
	thread->counter = MAX_PRIORITY;

	esp[0] = 0;			/* ebx */
	esp[1] = 0;			/* edi */
	esp[2] = 0;			/* esi */
	esp[3] = 0;			/* ebp */
	esp[4] = EF_IF;			/* eflags */
	esp[5] = (uint32_t) function;	/* eip */

	add_thread(thread);
	resume_thread(thread);

	return;
}

void task_init()
{
	struct thread *t_kernel;
	struct thread *t_idle;
	uint32_t *stack;
	tss_t *tss;

	// Kernel-Thread initialiesieren...

	t_kernel = &kernel_stack;
	t_kernel->tss = NULL;
	t_kernel->esp0 = ((uint32_t) t_kernel) + PAGE_SIZE;;
	t_kernel->esp = 0; /* ??? */
	t_kernel->state = THREAD_READY;
	t_kernel->priority = MAX_PRIORITY;

	disable_interrupts();
	t_kernel->counter = t_kernel->priority;
	t_kernel->need_reschedule = 0;
	enable_interrupts();

	// Kernel-Thread in die Thread-Liste
	// aufnehmen und die Run-Queue
	// initialiesieren...

	t_kernel->next = t_kernel;
	t_kernel->prev = t_kernel;
	first_thread = t_kernel;
	last_thread = t_kernel;

	t_kernel->q_next = NULL;
	queue_first = NULL;
	queue_last = NULL;

	// IDLE-Thread fuer den BSP erzeugen...

	t_idle = &idle_stack;
	t_idle->tss = NULL;
	t_idle->esp0 = ((uint32_t) t_idle) + PAGE_SIZE;
	t_idle->esp = t_idle->esp0 - (6 * sizeof(uint32_t));
	t_idle->state = THREAD_IDLE;
	t_idle->priority = 0;

	t_idle->counter = t_idle->priority;
	t_idle->need_reschedule = 0;

	stack = (uint32_t *) t_idle->esp;
	stack[0] = 0;			/* ebx */
	stack[1] = 0;			/* edi */
	stack[2] = 0;			/* esi */
	stack[3] = 0;			/* ebp */
	stack[4] = EF_IF;		/* eflags */
	stack[5] = (uint32_t) idle;	/* eip */

	// IDLE-Thread in die Thread- und IDLE-
	// Liste aufnehmen...

	add_thread(t_idle);

	t_idle->q_next = NULL;
	idle_first = t_idle;

	// TSS initialiesieren und laden...

	tss = &bsp_tss;
	memset(tss, 0, sizeof(tss_t));
	tss->esp0 = t_kernel->esp0;
	tss->ss0 = KERNEL_DATA_SELEKTOR;
	set_tss_deskriptor(BSP_TSS_SELEKTOR, tss);
	load_tr(BSP_TSS_SELEKTOR);
	t_kernel->tss = tss;

	return;
}

void ap_task_init()
{
	struct thread *t_idle;
	tss_t *tss;
	uint16_t selektor;

	// IDLE-Thread fuer den AP erzeugen...

	t_idle = get_current_thread();
	t_idle->tss = NULL; /* ??? */
	t_idle->esp0 = ((uint32_t) t_idle) + PAGE_SIZE;;
	t_idle->esp = 0; /* ??? */
	t_idle->state = THREAD_IDLE;
	t_idle->priority = 0;

	disable_interrupts();
	t_idle->counter = t_idle->priority;
	t_idle->need_reschedule = 0;
	enable_interrupts();

	// IDLE-Thread in die Thread-Liste aufnehmen...

	add_thread(t_idle);

	// TSS initialiesieren und laden...

	tss = (tss_t *) (t_idle + 1);
	selektor = TSS_SELEKTOR(get_apic_id());
	memset(tss, 0, sizeof(tss_t));
	tss->esp0 = t_idle->esp0;
	tss->ss0 = KERNEL_DATA_SELEKTOR;
	set_tss_deskriptor(BSP_TSS_SELEKTOR, tss);
	load_tr(BSP_TSS_SELEKTOR);
	t_idle->tss = tss;

	return;
}
