
/*
kernel/sleep.c @ Xorix Operating System
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

#include "entry.h"
#include "debug.h"
#include "cpu.h"
#include "sleep.h"

void calibrate_k_usleep(cpu_info_t *cpu_info)
{
	unsigned long tsc1, tsc2;
	unsigned long loops;
	char o;
	unsigned long c;
	uint32_t t;

	// TSC...

	if(cpu_info->feature_flags & IDF_TSC)
	{
		t = jiffies();
		while(t == jiffies());

		tsc1 = RDTSC();
		t = jiffies();
		while(t == jiffies());
		tsc2 = RDTSC();

		cpu_info->loops_per_jiffie = tsc2 - tsc1;

		return;
	}

	// Loop...

	loops = 1;

	do
	{
		loops *= 2;

		t = jiffies();
		while(t == jiffies());

		t = jiffies();
		LOOP_SLEEP(loops);
	} while(t == jiffies());

	c = loops / 2;
	o = '-';

	while(c > 0)
	{
		if(o == '+')
		{
			do
			{
				loops += c;

				t = jiffies();
				while(t == jiffies());

				t = jiffies();
				LOOP_SLEEP(loops);
			} while(t == jiffies());

			o = '-';
		}
		else
		{
			do
			{
				loops -= c;

				t = jiffies();
				while(t == jiffies());

				t = jiffies();
				LOOP_SLEEP(loops);
			} while(t != jiffies());

			o = '+';
		}

		c /= 2;
	}

	cpu_info->loops_per_jiffie = loops;

	return;
}

void k_usleep(unsigned long usec)
{
	cpu_info_t *c;
	unsigned long loops;

	c = get_my_cpu_info();

	// loops = loops_per_jiffie * usec / 10000

	asm("mull %%ebx          \n"
	    "movl $10000, %%ebx  \n"
	    "divl %%ebx          \n"
	    :"=a" (loops)
	    :"a" (c->loops_per_jiffie), "b" (usec)
	    :"edx");

	// Zzz...

	if(c->feature_flags & IDF_TSC)
		RDTSC_SLEEP(loops);
	else
		LOOP_SLEEP(loops);

	return;
}
