
/*
kernel/page_pg.c @ Xorix Operating System
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

#define PAGE_PG_DEBUG 1

#include <xorix.h>
#include <string.h>

#include "debug.h"
#include "multiboot.h"
#include "cpu.h"
#include "spinlock.h"
#include "smp.h"
#include "memory.h"
#include "page_pg.h"
#include "page.h"

static pg_pd_t *mother_pd;

static unsigned int last_vmen_pde;
static unsigned int last_vmen_pte;

static short *highmem_map;
static struct pg_kmap_table *kmap_table;
static short last_kmap_nr;

static spinlock_t vmem_lock;
static spinlock_t kmap_lock;

static inline void pg_flush_tlb()
{
	asm volatile("mov %%cr3,%%eax  \n\t"
	             "mov %%eax,%%cr3  \n\t"
	             :::"memory", "eax");

	if(use_smp == true)
		smp_flush_tlb();

	return;
}

static inline bool __pg_vmem_alloc(unsigned int *p_pde, unsigned int *p_pte, unsigned long count)
{
	unsigned int pde, pte;
	unsigned long c;
	pg_pt_t *pt;

	// Da beginnen, wo wir das letzte Mal
	// aufgehoert haben.

	pde = last_vmen_pde;
	pte = last_vmen_pte + 1;
	if(pte >= PG_MAX_PTE)
	{pde++; pte = 0;}

	// Nach freien virtuellen Adressraum suchen...

	count++;
	c = 0;

	while(pde <= PG_LAST_VMEM_PDE)
	{
		pt = PG_PDE_PTR(mother_pd->pde[pde]);

		while(pte < PG_MAX_PTE)
		{
			if(PG_PTE_IS_CLEAR(pt->pte[pte]))
			{
				c++;
				if(c >= count) goto found;
			}
			else c = 0;

			pte++;
		}

		pte = 0;
		pde++;
	}

	// Nix gefunden? Von vorn beginnen!

	c = 0;

	pde = PG_FIRST_VMEM_PDE;
	pte = 0;

	while(pde <= last_vmen_pde)
	{
		pt = PG_PDE_PTR(mother_pd->pde[pde]);

		while(pte < PG_MAX_PTE)
		{
			if(PG_PTE_IS_CLEAR(pt->pte[pte]))
			{
				c++;
				if(c >= count) goto found;
			}
			else c = 0;

			pte++;
		}

		pte = 0;
		pde++;
	}

	// Sorry, nix da...

	return false;

	// Freier Adressraum gefunden!

	found:

	last_vmen_pde = *p_pde = pde;
	last_vmen_pte = *p_pte = pte;

	PG_PTE_MAKE_BROKEN(pt->pte[pte]);

	return true;
}

void *pg_vmalloc(size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	pg_pt_t *pt;

	// ...

	if(size <= 0) return NULL;
	count = BYTES2PAGES(size);

	// Nach freien virtuellen Adressraum suchen...

	spin_lock(&vmem_lock);
	if(!__pg_vmem_alloc(&pde, &pte, count))
		{spin_unlock(&vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PG_PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		page = get_free_page(GFP_HIGHMEM);
		if(page == 0)
		{
			pg_vfree(PG_PTR_BY_PDE_PTE(pde, pte));
			spin_unlock(&vmem_lock);
			return NULL;
		}

		if(pte == 0)
		{
			pde--; pte = PG_MAX_PTE - 1;
			pt = PG_PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PG_PTE_SET_ENTRIE(pt->pte[pte], page, PG_PF_VMALLOC);
		count--;
	}

	spin_unlock(&vmem_lock);
	return PG_PTR_BY_PDE_PTE(pde, pte);
}

void pg_vfree(void *ptr)
{
	unsigned int pde, pte;
	pg_pt_t *pt;

	// Ist der Pointer ueberhaubt im VMEM?

	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}

	// Virtuellen Adressraum und Pages freigeben...

	pde = PG_PDE_BY_PTR(ptr);
	pte = PG_PTE_BY_PTR(ptr);

	//wprintk(L"(%i,%i)", pde, pte);

	spin_lock(&vmem_lock);
	pt = PG_PDE_PTR(mother_pd->pde[pde]);

	while(!PG_PTE_IS_BROKEN(pt->pte[pte]))
	{
		free_page(PG_PTE_GET_PAGE(pt->pte[pte]));
		PG_PTE_CLEAR(pt->pte[pte]);

		if(pte >= PG_MAX_PTE)
		{
			pde++; pte = 0;
			pt = PG_PDE_PTR(mother_pd->pde[pde]);
		}
		else pte++;
	}

	PG_PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&vmem_lock);

	pg_flush_tlb();

	return;
}

void *pg_ioremap(void *ptr, size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	unsigned int offset;
	pg_pt_t *pt;

	// ...

	if(size <= 0) return NULL;

	page = BYTE2PAGE(((uintptr_t) ptr) - 1 + size);
	count = BYTES2PAGES(size);
	offset = PG_OFFSET_BY_PTR(ptr);

	// Low Memory behandeln...

	if(page >= 0xA0 && page < 0x100) return ptr;
	if(page < MAX_LOW_PAGES) return NULL;

	// Nach freien virtuellen Adressraum suchen...

	spin_lock(&vmem_lock);
	if(!__pg_vmem_alloc(&pde, &pte, count))
		{spin_unlock(&vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PG_PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = PG_MAX_PTE - 1;
			pt = PG_PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PG_PTE_SET_ENTRIE(pt->pte[pte], page, PG_PF_IOREMAP);
		page--; count--;
	}

	spin_unlock(&vmem_lock);
	return PG_PTR_BY_PDE_PTE_OFFSET(pde, pte, offset);
}

void pg_iounmap(void *ptr)
{
	unsigned int pde, pte;
	pg_pt_t *pt;

	// Ist der Pointer ueberhaubt im VMEM?

	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}

	// Virtuellen Adressraum freigeben...

	pde = PG_PDE_BY_PTR(ptr);
	pte = PG_PTE_BY_PTR(ptr);

	spin_lock(&vmem_lock);
	pt = PG_PDE_PTR(mother_pd->pde[pde]);

	while(!PG_PTE_IS_BROKEN(pt->pte[pte]))
	{
		PG_PTE_CLEAR(pt->pte[pte]);

		if(pte >= PG_MAX_PTE)
		{
			pde++; pte = 0;
			pt = PG_PDE_PTR(mother_pd->pde[pde]);
		}
		else pte++;
	}

	PG_PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&vmem_lock);

	pg_flush_tlb();

	return;
}

/*
 *	pg_flush_unused_kmaps() leert unbenutzte kmaps...
 */

static inline void pg_flush_unused_kmaps()
{
	short i;
	unsigned long p;

	for(i = 0; i < KMAP_PAGES; i++)
	{
		if(kmap_table[i].users == 1)
		{
			p = PG_PTE_GET_PAGE(*kmap_table[i].pte) - MAX_LOW_PAGES;
			highmem_map[p] = -1;
			kmap_table[i].users = 0;
			PG_PTE_MAKE_BROKEN(*kmap_table[i].pte);
		}
	}

	pg_flush_tlb();
}

/*
 *	__pg_kmap() blendet ein Page aus dem Highmem ein...
 */

void *__pg_kmap(unsigned long page)
{
	short i;

	// ...

#ifdef PAGE_PG_DEBUG
	if(page >= total_pages) BUG();
#endif

	// Ist die Page schon eingeblendet?

	spin_lock(&kmap_lock);
	i = highmem_map[page - MAX_LOW_PAGES];

	if(i != -1)
	{
		kmap_table[i].users++;
		spin_unlock(&kmap_lock);
		return kmap_table[i].ptr;
	}

	// Nach freien virtuellen Adressraum suchen...

	for(i = last_kmap_nr + 1; i < KMAP_PAGES; i++)
		if(!kmap_table[i].users) goto found;

	// Keinen gefunden? Unbenutzte kmaps leeren
	// und von vorn beginnen!

	pg_flush_unused_kmaps();

	for(i = 0; i < KMAP_PAGES; i++)
		if(!kmap_table[i].users) goto found;

	// Nix gefunden...

	BUG(); /* FIXME! */

	// Freier Adressraum gefunden!

	found:

	highmem_map[page - MAX_LOW_PAGES] = i;
	kmap_table[i].users = 2;
	PG_PTE_SET_ENTRIE(*kmap_table[i].pte, page, PG_PF_VMALLOC);
	last_kmap_nr = i;
	spin_unlock(&kmap_lock);

	return kmap_table[i].ptr;
}

/*
 *	__pg_kunmap() blendet die Page aus dem Highmem
 *	wieder aus...
 */

void __pg_kunmap(unsigned long page)
{
	unsigned long p;
	unsigned short t;

	// ...

#ifdef PAGE_PG_DEBUG
	if(page >= total_pages) BUG();
#endif

	// Page freigeben...

	spin_lock(&kmap_lock);
	p = page - MAX_LOW_PAGES;
	t = highmem_map[p];

	kmap_table[t].users--;
#ifdef PAGE_PG_DEBUG
	if(kmap_table[t].users <= 0) BUG();
#endif
	spin_unlock(&kmap_lock);

	return;
}

/*
 *	pg_pageing_init() initialiesiert und aktiviert
 *	das Pageing.
 */

void pg_pageing_init()
{
	cpu_info_t *c;
	unsigned long count;
	unsigned int page;
	unsigned int ptn;
	pg_pt_t *pt;
	unsigned int pde, pte;
	unsigned long i;

	// Spinlocks initialisieren.

	spin_lock_init(&vmem_lock);
	spin_lock_init(&kmap_lock);

	// Mother Page-Directory anlegen...

	mother_pd = PAGE2PTR(get_free_page(GFP_KERNEL));
	if(mother_pd == NULL) kernel_panic("Out of Memory");
	memset((void *) mother_pd, 0, sizeof(pg_pd_t));

	page = 0;
	pde = PG_FIRST_PMEM_PDE;

	while(pde <= PG_LAST_PMEM_PDE && page < low_pages)
	{
		if(use_pse == true && page >= 0x100)
		{
			PG_PDE_SET_ENTRIE(mother_pd->pde[pde], page, PG_PDE_PRESENT | PG_PDE_WRITE | PG_PDE_PAGE_SIZE);
			if(use_pge == true) PG_PDE_OR_FLAGS(mother_pd->pde[pde], PG_PDE_GLOBAL);
			page += 1024;
		}
		else
		{
			ptn = get_free_page(GFP_KERNEL);
			pt = PAGE2PTR(ptn);
			if(pt == NULL) kernel_panic("Out of Memory");
			PG_PDE_SET_ENTRIE(mother_pd->pde[pde], ptn, PG_PDE_PRESENT | PG_PDE_WRITE);
			memset(pt, 0, sizeof(pg_pt_t));

			pte = 0;

			while(pte < PG_MAX_PTE && page < low_pages)
			{
				PG_PTE_SET_ENTRIE(pt->pte[pte], page, PG_PTE_PRESENT | PG_PTE_WRITE);
				if(use_pge == true) PG_PTE_OR_FLAGS(pt->pte[pte], PG_PTE_GLOBAL);
				if(page >= 0xA0 && page < 0x100) PG_PTE_OR_FLAGS(pt->pte[pte], PG_PTE_PAGE_WRITE_THROUGH | PG_PTE_PAGE_CACHE_DISABLE);
				mem_table[ptn].u[0].used_ptes++;
				page++;
				pte++;
			}
		}

		pde++;
	}

	// Virtual Memory anlegen...

	pde = PG_FIRST_VMEM_PDE;

	while(pde <= PG_LAST_VMEM_PDE)
	{
		ptn = get_free_page(GFP_KERNEL);
		pt = PAGE2PTR(ptn);
		if(pt == NULL) kernel_panic("Out of Memory");
		PG_PDE_SET_ENTRIE(mother_pd->pde[pde], ptn, PG_PDE_PRESENT | PG_PDE_WRITE);
		memset(pt, 0, sizeof(pg_pt_t));

		pde++;
	}

	last_vmen_pde = PG_FIRST_VMEM_PDE;
	last_vmen_pte = 0;

	// Pageing aktivieren...

	kernel_cr3 = (uint32_t) mother_pd;

	SET_CR3(kernel_cr3);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR0(CR0_PG);

	// Highmem-Map fuer kmap() anlegen...

	if(high_pages > 0)
	{
		highmem_map = pg_vmalloc(high_pages * sizeof(short));
		if(highmem_map == NULL) kernel_panic("Out of Memory");
		for(i = 0; i < high_pages; i++)
		{highmem_map[i] = -1;}
	}

	// Speicher fuer die kmap()-Table allocieren...

	kmap_table = pg_vmalloc(KMAP_PAGES * sizeof(struct pg_kmap_table));
	if(kmap_table == NULL) kernel_panic("Out of Memory");
	last_kmap_nr = 0;

	// Nach freien virtuellen Adressraum fuer
	// kmap() suchen...

	count = KMAP_PAGES;

	if(!__pg_vmem_alloc(&pde, &pte, count))
		kernel_panic("Out of Memory");

	// Virtuellen Adressraum blegen und die
	// kmap()-Table initialiesieren...

	pt = PG_PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = PG_MAX_PTE - 1;
			pt = PG_PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PG_PTE_MAKE_BROKEN(pt->pte[pte]);
		kmap_table[count - 1].users = 0;
		kmap_table[count - 1].pte = &pt->pte[pte];
		kmap_table[count - 1].ptr = PG_PTR_BY_PDE_PTE(pde, pte);

		count--;
	}

	return;
}

/*
 *	pg_ap_pageing_init() aktiviert das Pageing beim AP...
 */

void pg_ap_pageing_init()
{
	// Pageing aktivieren...

	SET_CR3(kernel_cr3);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR0(CR0_PG);

	return;
}
