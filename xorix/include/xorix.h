
/*
xorix.h @ Xorix Operating System
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

#ifndef _XORIX_H
#define _XORIX_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define _STR(s) #s
#define STR(s) _STR(s)

#define MAX_PATH			1024

#define SIG_NR				7
#define SIG_ALL				127
#define SIG_NONE			0

#define SIGKILL				1
#define SIGTERM				2
#define SIGINT				4
#define SIGABRT				8
#define SIGILL				16
#define SIGFPE				32
#define SIGSEGV				64

#define SIG_DFL				((void *) 0)
#define SIG_IGN				((void *) 1)
#define SIG_ERR				((void *) 2)

#define P_EXIT_SUCCESS			0
#define P_EXIT_FAILURE			1

#define T_EXIT_SUCCESS			((void *) 0)
#define T_EXIT_FAILURE			((void *) 1)

typedef int conn_t;

int memory_alloc(void **ptr, size_t size);
int memory_free(void *ptr);

void process_exit(uint32_t status);

int thread_create(void (*function)(void *), void *arg, size_t stack_size, bool detached);
void thread_exit(void *status);

int signal_set_handler(uint32_t signals, void *handler);
int signal_block(uint32_t signals);
int signal_unblock(uint32_t signals);
int signal_set_mask(uint32_t signals);
uint32_t signal_get_mask();

void _debug(int number);
void _debug_wcs(wchar_t *wcs);

extern inline unsigned int check_flag(unsigned int flags, unsigned int bit)
{
	return flags & (1 << bit);
}

#endif

void process_exit(uint32_t status);
void thread_exit();


