
/*
kernel/debug.c @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>
Copyright (C) 2001 Gregor Weckbecker <gregW@gmx.de>

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
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#include "console.h"
#include "cpu.h"
#include "spinlock.h"
#include "smp.h"
#include "debug.h"

static spinlock_t wprintk_lock = SPIN_LOCK_UNLOCKED;
static wchar_t wprintk_buffer[8192];

static spinlock_t kernel_panic_lock = SPIN_LOCK_UNLOCKED;

void wprintk(const wchar_t *format, ...)
{
	va_list arg;
	int r;

	disable_interrupts();
	spin_lock(&wprintk_lock);

	va_start(arg, format);
	r = vswprintf(wprintk_buffer, 8192, format, arg);
	va_end(arg);

	if(r > 0)
		console_wprint(wprintk_buffer);

	spin_unlock(&wprintk_lock);
	enable_interrupts();

	return;
}

void kernel_bug(const char *file, const char *line)
{
	unsigned int pos;

	disable_interrupts();
	spin_lock(&kernel_panic_lock);
	if(use_smp == true) smp_halt();

	panic_new_line();
	pos = panic_new_line();
	pos = panic_print(pos, "Kernel panic: BUG at ");
	pos = panic_print(pos, file);
	pos = panic_print(pos, ":");
	pos = panic_print(pos, line);
	panic_print(pos, "!");
	panic_new_line();

	// goto rulez ;o)
	loop: goto loop;

	return;
}

void kernel_panic(const char *s)
{
	unsigned int pos;

	disable_interrupts();
	spin_lock(&kernel_panic_lock);
	if(use_smp == true) smp_halt();

	panic_new_line();
	pos = panic_new_line();
	pos = panic_print(pos, "Kernel panic: ");
	panic_print(pos, s);
	panic_new_line();

	// goto rulez ;o)
	loop: goto loop;

	return;
}
