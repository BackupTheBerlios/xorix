
/*
kernel/memory.h @ Xorix Operating System
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

#ifndef _MEMORY_H
#define _MEMORY_H

#define MAX_TOTAL_PAGES	16777216
#define MAX_LOW_PAGES	458752
#define MAX_HIGH_PAGES	16318464

#define PAGE_SIZE	4096
#define PAGE_KB_SIZE	4

#define VMEM_OFFSET	0x70000000
#define VMEM_LIMIT	0x0FFFFFFF

#define KMAP_PAGES	1024

#define __GFP_HIGH	1
#define __GFP_USER	2

#define GFP_KERNEL	0
#define GFP_HIGHMEM	__GFP_HIGH

#define GFP_USER	__GFP_USER
#define GFP_HIGHUSER	(__GFP_HIGH | __GFP_HIGH)

#ifndef ASM

#define BYTES2PAGES(bytes) \
	(((bytes) / PAGE_SIZE) + (((bytes) % PAGE_SIZE) ? 1 : 0))

#define KBYTES2PAGES(kbytes) \
	(((kbytes) / PAGE_KB_SIZE) + (((kbytes) % PAGE_KB_SIZE) ? 1 : 0))

#define PAGE2BYTE(page) ((page) << 12)
#define BYTE2PAGE(byte) ((byte) >> 12)
#define PAGE2PTR(page) ((void *) ((page) << 12))
#define PTR2PAGE(ptr) (((uintptr_t) (ptr)) >> 12)
#define IS_PAGE(page) (!(((uintptr_t) (page)) & 0xFFF))
#define IS_NO_PAGE(page) (((uintptr_t) (page)) & 0xFFF)
#define PAGE_OFFSET(ptr) (((uintptr_t) (ptr)) & 0xFFF)

struct mem_table
{
	unsigned long users;
	
	union
	{
		unsigned long unknown;
		unsigned long used_ptes; // for Page Table's
	} u[1];
};

struct mem_list
{
	struct mem_list *next;
};

struct mem_stack
{
	unsigned long *elements;
	unsigned long size;
	unsigned long max_size;
};

extern unsigned long total_pages;
extern unsigned long low_pages;
extern unsigned long high_pages;

extern struct mem_table *mem_table;

void memory_init();

unsigned long get_free_page(int gfp_mask);
void free_page(unsigned long page);
void add_page_user(unsigned long page);

#endif

#endif






