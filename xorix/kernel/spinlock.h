
/*
kernel/spinlock.h @ Xorix Operating System
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

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#ifndef ASM

#define SPIN_LOCK_UNLOCKED ((spinlock_t) {0})

typedef struct spinlock
{
	volatile uint32_t lock;
} spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
	asm volatile("0:                \n"
	             "lock; btsl $0,%0  \n"
	             "jnb 2f            \n"
	             "1:                \n"
	             "testl $1,%0       \n"
	             "je 0b             \n"
	             "rep; nop          \n"
	             "jmp 1b            \n"
	             "2:                \n"
	             ::"m" (lock->lock));

	return;
}

static inline void spin_unlock(spinlock_t *lock)
{
	asm volatile("movb $0,%0"::"m" (lock->lock));
	return;
}

static inline void spin_lock_init(spinlock_t *lock)
{
	asm volatile("movb $0,%0"::"m" (lock->lock));
	return;
}

#endif

#endif




