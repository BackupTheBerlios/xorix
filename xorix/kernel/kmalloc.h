
/*
kernel/kmalloc.h @ Xorix Operating System
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

#ifndef _KMALLOC_H
#define _KMALLOC_H

#ifndef ASM

struct block_header
{
	struct block_header *next;
};

struct page_header
{
	struct page_header *next;
	struct page_header *previous;

	struct block_header *first_free_block;
	unsigned short nsize;
	unsigned short full;
};

struct size_struct
{
	unsigned int size;
	struct page_header *first_free_page;
};

struct page_header *new_kmalloc_page(unsigned short nsize);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif

#endif

