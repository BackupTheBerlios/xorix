
/*
kernel/page.c @ Xorix Operating System
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

#define MEMORY_DEBUG 1

#include <xorix.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

#include "multiboot.h"
#include "bootmem.h"
#include "debug.h"
#include "spinlock.h"
#include "memory.h"

unsigned long total_pages;

unsigned long low_pages;
static unsigned long free_low_pages;

unsigned long high_pages;
static unsigned long free_high_pages;

struct mem_table *mem_table;
static struct mem_stack mem_stack;
static struct mem_list *mem_list;

spinlock_t mem_lock;

/*
 *	memory_init() initialiesiert die Memory Table.
 */

void memory_init()
{
	struct mem_list *new;
	unsigned long p;
	unsigned long i;

	// Spinlock initialisieren.

	spin_lock_init(&mem_lock);

	// Memory Map vom BIOS ausgeben...

	print_memory_map();

	// Groesse des Arbeitspeichers ermitteln...

	total_pages = check_total_pages();

	low_pages = check_low_pages();
	free_low_pages = 0;
	high_pages = check_high_pages();
	free_high_pages = 0;

	// Speicher für die Pages Table allocieren.

	mem_table = alloc_bootmem(total_pages * sizeof(struct mem_table));

	if(high_pages > 0)
	{mem_stack.elements = alloc_bootmem(high_pages * sizeof(unsigned long));}

	// Memory Table initialisieren.

	for(p = 0; p < total_pages; p++)
	{
		if(is_page_available(p))
		{
			mem_table[p].users = 0;
			mem_table[p].u[0].unknown = 0;
		}
		else
		{
			mem_table[p].users = 1;
			mem_table[p].u[0].unknown = 0;
		}
	}

	// Memory List initialisieren.

	mem_list = NULL;
	p = low_pages;

	while(p--)
	{
		if(mem_table[p].users == 0)
		{
			new = PAGE2PTR(p);
			new->next = mem_list;
			mem_list = new;
			free_low_pages++;
		}
	}

	// Den Memory Stack initialisieren.

	if(high_pages > 0)
	{
		mem_stack.size = 0;
		mem_stack.max_size = high_pages;

		i = high_pages;

		while(i--)
		{
			p = MAX_LOW_PAGES + i;

			if(mem_table[p].users == 0)
			{
				mem_stack.elements[mem_stack.size] = p;
				mem_stack.size++;
			}
		}
	}

	return;
}

/*
 *	get_free_page() gibt die Nummer einer freien Page zurueck.
 */

unsigned long get_free_page(int gfp_mask)
{
	unsigned long page = 0;
	
	spin_lock(&mem_lock);

	// Kann die Page im Highmem sein?

	if(gfp_mask & __GFP_HIGH)
	{
		// Im High Memory Stack nach einer
		// freien Page suchen...

		if(free_high_pages > 0)
		{
#ifdef MEMORY_DEBUG
			if(mem_stack.size < 1) BUG();
#endif
			mem_stack.size--;
			page = mem_stack.elements[mem_stack.size];

			mem_table[page].users = 1;
			mem_table[page].u[0].unknown = 0;
			free_high_pages--;
			
			spin_unlock(&mem_lock);

			return 0;
		}
	}

	// In der Low Memory Liste nach einer
	// freien Page suchen...

	if(free_low_pages > 0)
	{
#ifdef MEMORY_DEBUG
		if(mem_list == NULL) BUG();
#endif
		page = PTR2PAGE(mem_list);
		mem_list = mem_list->next;

		mem_table[page].users = 1;
		mem_table[page].u[0].unknown = 0;
		free_low_pages--;

		spin_unlock(&mem_lock);

		return page;
	}

	// Kein Speicher mehr frei...
	
	spin_unlock(&mem_lock);

	return 0;
}

/*
 *	free_page() gibt die uebergebende Page wieder frei.
 */

void free_page(unsigned long page)
{
	struct mem_list *new;
	
	spin_lock(&mem_lock);

#ifdef MEMORY_DEBUG
	if(page == 0) BUG();
	if(page >= total_pages) BUG();
	if(mem_table[page].users == 0) BUG();
#endif

	// Wird die Page von jemand Anderen noch benutzt?

	if(mem_table[page].users > 1)
		{mem_table[page].users--; spin_unlock(&mem_lock); return;}

	// Wenn nein, Page wieder Frei geben...

	mem_table[page].users = 0;

	if(page < MAX_LOW_PAGES)
	{
		// Page in die Low Memory Liste eintragen.

		new = PAGE2PTR(page);
		new->next = mem_list;
		mem_list = new;
		free_low_pages++;
	}
	else
	{
		// Page in den High Memory Stack eintragen.

#ifdef MEMORY_DEBUG
		if(mem_stack.size >= mem_stack.max_size) BUG();
#endif
		mem_stack.elements[mem_stack.size] = page;
		mem_stack.size++;
		free_high_pages++;
	}

	spin_unlock(&mem_lock);

	return;
}

/*
 *	add_page_user() fuegt einen Nutzer der Page hinzu...
 */

void add_page_user(unsigned long page)
{
	spin_lock(&mem_lock);
#ifdef MEMORY_DEBUG
	if(page >= total_pages) BUG();
#endif
	mem_table[page].users++;
	spin_unlock(&mem_lock);

	return;
}

