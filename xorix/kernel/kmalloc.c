
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

#include <xorix.h>

#include "debug.h"
#include "memory.h"
#include "page.h"
#include "entry.h"
#include "spinlock.h"
#include "kmalloc.h"

static spinlock_t kmalloc_lock;

static struct size_struct sizes[] = {
	{16, NULL},
	{32, NULL},
	{64, NULL},
	{127, NULL},
	{255, NULL},
	{510, NULL},
	{1020, NULL},
	{2040, NULL},
};

struct page_header *new_kmalloc_page(unsigned short nsize)
{
	struct page_header *page_header;
	struct block_header *block_header;
	unsigned short size;

	page_header = PAGE2PTR(get_free_page(GFP_KERNEL));
	if(page_header == NULL) return NULL;

	size = sizes[nsize].size;

	page_header->next = NULL;
	page_header->previous = NULL;
	page_header->first_free_block = ((void *) page_header) + sizeof(struct page_header);
	page_header->nsize = nsize;
	page_header->full = 0;

	block_header = page_header->first_free_block;
	block_header->next = (void *) block_header + size;

	while(((void *) block_header->next + size) <= ((void *) page_header + PAGE_SIZE))
	{
		block_header = block_header->next;
		block_header->next = (void *) block_header + size;
	}

	block_header->next = NULL;

	return page_header;
}

void *kmalloc(size_t size)
{
	struct block_header *bh; // ;)
	int i;

	// Die richtige Grösse suchen...

	if(size > 2040)
	{
		if(size > PAGE_SIZE) return vmalloc(size);
		else return PAGE2PTR(get_free_page(GFP_KERNEL));
	}

	for(i = 0; size > sizes[i].size; i++);

	// Wenn keine freie Page in der liste ist, wird
	// eine neue allociert...

	spin_lock(&kmalloc_lock);

	if(sizes[i].first_free_page == NULL)
	{wprintk(L"(NP)");
		sizes[i].first_free_page = new_kmalloc_page(i);

		if(sizes[i].first_free_page == NULL)
		{
			spin_unlock(&kmalloc_lock);
			return NULL;
		}
	}

	// Freien Block aus der Liste nehmen...

	bh = (void *) sizes[i].first_free_page->first_free_block;
	sizes[i].first_free_page->first_free_block = bh->next;
	sizes[i].first_free_page->full++;

	// Wenn die Page jetzt keine freien Blöcke mehr hat,
	// wird sie aus der Liste entfernt...

	if(sizes[i].first_free_page->first_free_block == NULL)
	{
		sizes[i].first_free_page = sizes[i].first_free_page->next;

		if(sizes[i].first_free_page != NULL)
		{
			sizes[i].first_free_page->previous = NULL;
		}
	}

	// Bye, bye...

	spin_unlock(&kmalloc_lock);
	return (void *) bh;
}

void kfree(void *ptr)
{
	unsigned int pn;
	struct page_header *ph;
	struct block_header *bh; // :)
	unsigned short ns;
	struct page_header *pi;

	// NULL-Pointer?

	if(ptr == NULL) return;

	// Ganze Page?

	if(IS_PAGE((uintptr_t) ptr))
	{
		if((uintptr_t) ptr >= VMEM_OFFSET) vfree(ptr);
		else free_page(PTR2PAGE(ptr));
		return;
	}

	// Infos ermitteln...

	spin_lock(&kmalloc_lock);

	bh = (struct block_header *) ptr;
	pn = PTR2PAGE(ptr);
	ph = PAGE2PTR(pn);
	ns = ph->nsize;

	// Block wieder freigeben...

	bh->next = ph->first_free_block;
	ph->first_free_block = bh;
	ph->full--;

	// Wenn die Page jetzt frei und nicht die
	// letzte ist, wird sie freigegeben...
if(ph->full < 1) wprintk(L"*");
	if(ph->full < 1 && (ph->next != NULL || ph->previous != NULL))
	{wprintk(L"(FP)");
		// Page aus der Liste austragen...

		if(ph->previous == NULL)
		{
			// Wenn die Page die letzte ist

			sizes[ns].first_free_page = ph->next;

			if(sizes[ns].first_free_page != NULL)
			{
				sizes[ns].first_free_page->previous = NULL;
			}
		}
		else
		{
			ph->previous->next = ph->next;

			if(ph->next != NULL)
			{
				ph->next->previous = ph->previous;
			}
		}

		// Page freigeben...

		free_page(pn);
	}

	// Wenn die Page, weil sie voll war, aus der Liste geworfen
	// wurde, wird sie jetzt wieder eingefügt...

	else if(bh->next == NULL)
	{
		ph->next = sizes[ns].first_free_page;
		ph->previous = NULL;
		sizes[ns].first_free_page = ph;

		if(ph->next != NULL)
		{
			ph->next->previous = ph;
		}
	}

	spin_unlock(&kmalloc_lock);

	return;
}

