
/*
xorix.c @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#define extern
#define inline

#include <stdarg.h>
#include <xorix.h>
#include <wchar.h>

int memory_alloc(void **ptr, size_t size)
{
	void *p;
	asm volatile("int $0x80":"=a" (p):"a" (0), "b" (size));
	*ptr = p;
	return 0;
}

int memory_free(void *ptr)
{
	int r;
	asm volatile("int $0x80":"=a" (r):"a" (1), "b" (ptr));
	return r;
}

void process_exit(uint32_t status)
{
	asm volatile("int $0x80"::"a" (3), "b" (status));
	return;
}

// int thread_create(void (*function)(void *), void *arg, size_t stack_size, bool detached)
// "c" (arg), "d" (stack_size), "S" (detached)

int thread_create(void (*function)(void *), void *arg, size_t stack_size, bool detached)
{
	int r;
	
	asm volatile("int $0x80":"=a" (r):"a" (4), "b" (function), "c" (arg), "d" (stack_size), "S" ((uint32_t) detached));
	
	return r;
}

void thread_exit(void *status)
{
	asm volatile("int $0x80"::"a" (5), "b" (status));
	return;
}

int signal_set_handler(uint32_t signals, void *handler)
{
	int r;
	asm volatile("int $0x80":"=a" (r):"a" (9), "b" (signals), "c" (handler));
	return r;
}

int signal_block(uint32_t signals)
{
	int r;
	asm volatile("int $0x80":"=a" (r):"a" (11), "b" (signals));
	return r;
}

int signal_unblock(uint32_t signals)
{
	int r;
	asm volatile("int $0x80":"=a" (r):"a" (12), "b" (signals));
	return r;
}

int signal_set_mask(uint32_t signals)
{
	int r;
	asm volatile("int $0x80":"=a" (r):"a" (13), "b" (signals));
	return r;
}

uint32_t signal_get_mask()
{
	uint32_t r;
	asm volatile("int $0x80":"=a" (r):"a" (14));
	return r;
}

void _debug(int number)
{
	asm volatile("int $0x80"::"a" (31), "b" (number));
	return;
}

void _debug_wcs(wchar_t *wcs)
{
	asm volatile("int $0x80"::"a" (32), "b" (wcs));
	return;
}

