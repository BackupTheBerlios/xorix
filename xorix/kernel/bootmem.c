
/*
kernel/bootmem.c @ Xorix Operating System
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
#include <string.h>
#include <inttypes.h>

#include "cpu.h"
#include "memory.h"
#include "multiboot.h"
#include "smpboot.h"
#include "debug.h"
#include "bootmem.h"

unsigned long pages_start = 0x100;

/*
 *	print_memory_map() gibt die Memory Map vom BIOS
 *	wenn vorhanden auf dem Bildschirm aus.
 */

void print_memory_map()
{
	struct memory_map *mmap;
	uint64_t l, h;

	if(multiboot_info->flags & MB_INFO_MEM_MAP)
	{
		wprintk(L"BIOS-provided physical memory map:\n");

		mmap = (struct memory_map *) multiboot_info->mmap_addr;

		while((uintptr_t) mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length)
		{
			wprintk (L"%#.16" PRIx64 " - %#.16" PRIx64 " (%#.2x)\n",
			         mmap->base_addr,
			         mmap->base_addr + mmap->length,
			         mmap->type);

			mmap = (struct memory_map *) ((uintptr_t) mmap + mmap->size + sizeof (mmap->size));
		}
	}

	return;
}

/*
 *	check_total_pages() ermittelt den verfuegbaren Arbeitsspeicher.
 */

unsigned long check_total_pages()
{
	unsigned long pages;

	if(multiboot_info->flags & MB_INFO_MEMORY)
	{
		pages = multiboot_info->mem_upper / PAGE_KB_SIZE + 0x100;

		if(get_bsp_cpu_info()->feature_flags & IDF_PAE)
		{
			if(pages > MAX_TOTAL_PAGES)
				return MAX_TOTAL_PAGES;

			return pages;
		}

		if(pages > MAX_LOW_PAGES) return MAX_LOW_PAGES;

		return pages;
	}

	kernel_panic("Can't detect the memory size.");

	return 0;
}

/*
 *	check_low_pages() ermittelt den verfuegbaren Low Memory.
 */

unsigned long check_low_pages()
{
	unsigned long pages;

	pages = check_total_pages();
	if(pages > MAX_LOW_PAGES) return MAX_LOW_PAGES;

	return pages;
}

/*
 *	check_high_pages() ermittelt den verfuegbaren High Memory.
 */

unsigned long check_high_pages()
{
	unsigned long pages;
	
	pages = check_total_pages();
	if(pages > MAX_LOW_PAGES) return pages - MAX_LOW_PAGES;
	
	return 0;
}

/*
 *	is_mem_in_page() ermittelt, ob sich der uebergebene
 *	Speicherbereich ganz oder teilweise in der 
 *	uebergebenen Page befindet.
 */

static inline bool is_mem_in_page(const void *s, size_t n, unsigned long page)
{
	unsigned long first_page, last_page;
	
	first_page = PTR2PAGE(s);
	if(n < 1) last_page = first_page;
	else last_page = BYTE2PAGE(((uintptr_t) s) - 1 + n);
	
	if(page >= first_page && page <= last_page)
		return true;
	
	return false;
}

/*
 *	is_str_in_page() ermittelt, ob sich der uebergebene
 *	String ganz oder teilweise in der uebergebenen
 *	Page befindet.
 */

static inline bool is_str_in_page(const char *s, unsigned long page)
{
	unsigned long first_page, last_page;
	
	first_page = PTR2PAGE(s);
	last_page = BYTE2PAGE(((uintptr_t) s) + strlen(s));
	
	if(page >= first_page && page <= last_page)
		return true;
	
	return false;
}

/*
 *	is_page_available() testet ob die Page zur verfuegung steht.
 */

bool is_page_available(unsigned long page)
{
	multiboot_info_t *mbi = multiboot_info;
	module_t *mods;
	uint32_t mods_count;
	char *str;
	memory_map_t *mmap;
	uint64_t first_page, last_page;
	uint32_t i;
	
	/* Gibs die Page ueberhaubt? */
	if(page >= check_total_pages()) return false;

	/* Ersten 4 KByte vor dem ueberschreiben schuetzen. */
	if(page == 0) return false;

	/* SMP-Boot-Page frei halten. */

	if(page == SMPBOOT_PAGE) return false;

	/* Video-RAM, etc vor dem ueberschreiben schuetzen. */

	if(page < 0x100)
	{
		if(page >= 0xA0) return false;
		
		if(multiboot_info->flags & MB_INFO_MEMORY)
		{
			if(page >= multiboot_info->mem_lower / PAGE_KB_SIZE)
				return false;
		}
	}
	
	/* Boot-Memory vor dem Ueberschreiben schuetzen. */
	if(page >= 0x100 && page < pages_start) return false;
	
	/* Kernel vor dem ueberschreiben schuetzen. */
	if(is_mem_in_page(&_start,
	                  ((uintptr_t) &_end) - ((uintptr_t) &_start),
	                  page))
	{return false;}
	
	/* Module vor dem ueberschreiben schuetzen. */
	if((mbi->flags & MB_INFO_MODS) && mbi->mods_count > 0)
	{
		mods = (module_t *) mbi->mods_addr;
		mods_count = mbi->mods_count;
		
		for(i = 0; i < mods_count; i++)
		{
			if(is_mem_in_page((void *) mods[i].mod_start, 
			                  mods[i].mod_end - mods[i].mod_start,
			                  page))
			{return false;}
		}
	}
	
	/* In der BIOS Memory Map als Reseviert eingtragene
	   Pages schuetzen. */
	if(mbi->flags & MB_INFO_MEM_MAP)
	{
		/* In der BIOS Memory Map als Reseviert eingtragene
		   Pages schuetzen. */
		
		mmap = (struct memory_map *) multiboot_info->mmap_addr;
		
		while((uintptr_t) mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length)
		{
			first_page = BYTE2PAGE(mmap->base_addr + PAGE_SIZE - 1);
			last_page = BYTE2PAGE(mmap->base_addr + mmap->length);
			
			if(page >= first_page && page < last_page)
			{
				if(mmap->type == 1) return true;
			}
			
			mmap = (struct memory_map *) ((uintptr_t) mmap + mmap->size + sizeof (mmap->size));
		}
		
		return false;
	}
	
	return true;
}

void *alloc_bootmem(size_t size)
{
	unsigned long p;
	unsigned long page;
	unsigned long count;
	unsigned long p_size;
	
	p_size = BYTES2PAGES(size);
	count = 0;
	page = pages_start;
	
	for(p = page; count < p_size; p++)
	{
		if(p >= check_low_pages()) 
			kernel_panic("Out of Memory");
		
		if(is_page_available(p)) count++;
		else {count = 0; page = p + 1;}
	}
	
	pages_start = p;
	
	return PAGE2PTR(page);
}




