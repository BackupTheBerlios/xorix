
/*
kernel/page.h @ Xorix Operating System
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

#ifndef _PAGE_H
#define _PAGE_H

#ifndef ASM

extern uint32_t kernel_cr3;

extern bool use_pae;
extern bool use_pge;
extern bool use_pse;

#ifndef _PAGE_PG_H
#ifndef _PAGE_PAE_H

#include "page_pg.h"
#include "page_pae.h"

static inline void *kmap(unsigned long page)
{
	if(use_pae == true)
		return pae_kmap(page);

	return pg_kmap(page);
}

static inline void kunmap(unsigned long page)
{
	if(use_pae == true)
	{
		pae_kunmap(page);
		return;
	}

	pg_kunmap(page);
	return;
}

static inline void clear_page(unsigned long page)
{
	if(use_pae == true)
	{
		pae_clear_page(page);
		return;
	}

	pg_clear_page(page);
	return;
}

static inline void copy_page(unsigned long d_page, unsigned long s_page)
{
	if(use_pae == true)
	{
		pae_copy_page(d_page, s_page);
		return;
	}

	pg_copy_page(d_page, s_page);
	return;
}

static inline void *vmalloc(size_t size)
{
	if(use_pae == true)
		return pae_vmalloc(size);

	return pg_vmalloc(size);
}

static inline void vfree(void *ptr)
{
	if(use_pae == true)
	{
		pae_vfree(ptr);
		return;
	}

	pg_vfree(ptr);
	return;
}

static inline void *ioremap(void *ptr, size_t size)
{
	if(use_pae == true)
		return pae_ioremap(ptr, size);

	return pg_ioremap(ptr, size);
}

static inline void iounmap(void *ptr)
{
	if(use_pae == true)
	{
		pae_iounmap(ptr);
		return;
	}

	pg_iounmap(ptr);
	return;
}

void pageing_init();

static inline void ap_pageing_init()
{
	if(use_pae == true)
	{
		pae_ap_pageing_init();
		return;
	}

	pg_ap_pageing_init();
	return;
}

#endif
#endif

#endif

#endif











