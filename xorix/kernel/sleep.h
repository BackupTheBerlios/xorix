
/*
kernel/sleep.h @ Xorix Operating System
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

#ifndef _SLEEP_H
#define _SLEEP_H

#define HZ 100

#ifndef ASM

#define LOOP_SLEEP(loops) \
	asm volatile("0:         \n" \
	             "dec %%eax  \n" \
	             "jnz 0b     \n" \
	             ::"a" (loops))

#define RDTSC_SLEEP(loops) \
	asm volatile("rdtsc            \n" \
	             "mov %%eax,%%ebx  \n" \
	             "0:               \n" \
	             "rep; nop         \n" \
	             "rdtsc            \n" \
		     "sub %%ebx,%%eax  \n" \
	             "cmp %%ecx,%%eax  \n" \
	             "jb 0b            \n" \
	             ::"c" (loops):"ebx", "edx")


void calibrate_k_usleep(cpu_info_t *cpu_info);
void k_usleep(unsigned long usec);

#endif

#endif




