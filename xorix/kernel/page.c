
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

#define PAGE_DEBUG 1

#include <xorix.h>
#include <string.h>

#include "debug.h"
#include "multiboot.h"
#include "cpu.h"
#include "spinlock.h"
#include "smp.h"
#include "memory.h"
#include "page_pae.h"
#include "page.h"

volatile pd_t *mother_pd;

static unsigned int pf_kernel;
static unsigned int pf_user;
static unsigned int pf_user_copy;

static unsigned int last_vmen_pde;
static unsigned int last_vmen_pte;

static short *highmem_map;
static struct kmap_table *kmap_table;
static short last_kmap_nr;

bool use_pae;
bool use_pge;
bool use_pse;

static spinlock_t vmem_lock;
static spinlock_t kmap_lock;

static inline void flush_tlb()
{
	asm volatile("mov %%cr3,%%eax  \n\t"
	             "mov %%eax,%%cr3  \n\t"
	             :::"memory", "eax");

	if(use_smp == true)
		smp_flush_tlb();

	return;
}

static inline bool __vmem_alloc(unsigned int *p_pde, unsigned int *p_pte, unsigned long count)
{
	unsigned int pde, pte;
	unsigned long c;
	pt_t *pt;

	// Da beginnen, wo wir das letzte Mal
	// aufgehoert haben.

	pde = last_vmen_pde;
	pte = last_vmen_pte + 1;
	if(pte >= MAX_PTE)
	{pde++; pte = 0;}

	// Nach freien virtuellen Adressraum suchen...

	count++;
	c = 0;

	while(pde <= LAST_VMEM_PDE)
	{
		pt = PDE_PTR(mother_pd->pde[pde]);

		while(pte < MAX_PTE)
		{
			if(PTE_IS_CLEAR(pt->pte[pte]))
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

	pde = FIRST_VMEM_PDE;
	pte = 0;

	while(pde <= last_vmen_pde)
	{
		pt = PDE_PTR(mother_pd->pde[pde]);

		while(pte < MAX_PTE)
		{
			if(PTE_IS_CLEAR(pt->pte[pte]))
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

	/*last_vmen_pde =*/ *p_pde = pde;
	/*last_vmen_pte =*/ *p_pte = pte;

	PTE_MAKE_BROKEN(pt->pte[pte]);

	return true;
}

void *vmalloc(size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	pt_t *pt;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
		return pae_vmalloc(size);

	// ...

	if(size <= 0) return NULL;
	count = BYTES2PAGES(size);

	// Nach freien virtuellen Adressraum suchen...

	spin_lock(&vmem_lock);
	if(!__vmem_alloc(&pde, &pte, count))
		{spin_unlock(&vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		page = get_free_page(GFP_HIGHMEM);
		if(page == 0)
		{
			vfree(PTR_BY_PDE_PTE(pde, pte));
			spin_unlock(&vmem_lock);
			return NULL;
		}

		if(pte == 0)
		{
			pde--; pte = MAX_PTE - 1;
			pt = PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PTE_SET_ENTRIE(pt->pte[pte], page, PF_VMALLOC);
		count--;
	}

	spin_unlock(&vmem_lock);
	return PTR_BY_PDE_PTE(pde, pte);
}

void vfree(void *ptr)
{
	unsigned int pde, pte;
	pt_t *pt;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_vfree(ptr);
		return;
	}

	// Ist der Pointer ueberhaubt im VMEM?

	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}

	// Virtuellen Adressraum und Pages freigeben...

	pde = PDE_BY_PTR(ptr);
	pte = PTE_BY_PTR(ptr);

	//wprintk(L"(%i,%i)", pde, pte);

	spin_lock(&vmem_lock);
	pt = PDE_PTR(mother_pd->pde[pde]);

	while(!PTE_IS_BROKEN(pt->pte[pte]))
	{
		free_page(PTE_GET_PAGE(pt->pte[pte]));
		PTE_CLEAR(pt->pte[pte]);

		if(pte >= MAX_PTE)
		{
			pde++; pte = 0;
			pt = PDE_PTR(mother_pd->pde[pde]);
		}
		else pte++;
	}

	PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&vmem_lock);

	flush_tlb();

	return;
}

void *ioremap(void *ptr, size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	unsigned int offset;
	pt_t *pt;
	
	// Physical Address Extension (PAE) ?

	if(use_pae == true)
		return pae_ioremap(ptr, size);
	
	// ...
	
	if(size <= 0) return NULL;
	
	page = BYTE2PAGE(((uintptr_t) ptr) - 1 + size);
	count = BYTES2PAGES(size);
	offset = OFFSET_BY_PTR(ptr);
	
	// Low Memory behandeln...
	
	//if(page >= 0xA0 && page < 0x100) return ptr;
	//if(page < MAX_LOW_PAGES) return NULL;
	
	// Nach freien virtuellen Adressraum suchen...

	spin_lock(&vmem_lock);
	if(!__vmem_alloc(&pde, &pte, count))
		{spin_unlock(&vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = MAX_PTE - 1;
			pt = PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PTE_SET_ENTRIE(pt->pte[pte], page, PF_IOREMAP);
		page--; count--;
	}

	spin_unlock(&vmem_lock);
	return PTR_BY_PDE_PTE_OFFSET(pde, pte, offset);
}

void iounmap(void *ptr)
{
	unsigned int pde, pte;
	pt_t *pt;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_iounmap(ptr);
		return;
	}

	// Ist der Pointer ueberhaubt im VMEM?
	
	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}

	// Virtuellen Adressraum freigeben...
	
	pde = PDE_BY_PTR(ptr);
	pte = PTE_BY_PTR(ptr);
	
	wprintk(L"(%i,%i)", pde, pte);
	
	spin_lock(&vmem_lock);
	pt = PDE_PTR(mother_pd->pde[pde]);

	while(!PTE_IS_BROKEN(pt->pte[pte]))
	{
		PTE_CLEAR(pt->pte[pte]);

		if(pte >= MAX_PTE)
		{
			pde++; pte = 0;
			pt = PDE_PTR(mother_pd->pde[pde]);
		}
		else pte++;
	}

	PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&vmem_lock);

	flush_tlb();

	return;
}

/*
 *	flush_unused_kmaps() leert unbenutzte kmaps...
 */

static inline void flush_unused_kmaps()
{
	short i;
	unsigned long p;

	for(i = 0; i < KMAP_PAGES; i++)
	{
		if(kmap_table[i].users == 1)
		{
			p = PTE_GET_PAGE(*kmap_table[i].pte) - MAX_LOW_PAGES;
			highmem_map[p] = -1;
			kmap_table[i].users = 0;
			PTE_MAKE_BROKEN(*kmap_table[i].pte);
		}
	}

	flush_tlb();
}

/*
 *	kmap() blendet ein Page aus dem Highmem ein...
 */

void *kmap(unsigned long page)
{
	short i;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
		return pae_kmap(page);

	// Eine Page im Low-Memory braucht natuerlich
	// nicht neu eingeblendet zu werden...

	if(page < MAX_LOW_PAGES)
		return PAGE2PTR(page);

#ifdef PAGE_DEBUG
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

	flush_unused_kmaps();

	for(i = 0; i < KMAP_PAGES; i++)
		if(!kmap_table[i].users) goto found;

	// Nix gefunden...

	BUG(); /* FIXME! */

	// Freier Adressraum gefunden!

	found:

	highmem_map[page - MAX_LOW_PAGES] = i;
	kmap_table[i].users = 2;
	PTE_SET_ENTRIE(*kmap_table[i].pte, page, PF_VMALLOC);
	last_kmap_nr = i;
	spin_unlock(&kmap_lock);

	return kmap_table[i].ptr;
}

/*
 *	kunmap() blendet die Page aus dem Highmem
 *	wieder aus...
 */

void kunmap(unsigned long page)
{
	unsigned long p;
	unsigned short t;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_kunmap(page);
		return;
	}

	// Pages im Low-Memory duerfen natuerlich
	// nicht ausgeblendet werden...

	if(page < MAX_LOW_PAGES)
		return;

#ifdef PAGE_DEBUG
	if(page >= total_pages) BUG();
#endif

	// Page freigeben...

	spin_lock(&kmap_lock);
	p = page - MAX_LOW_PAGES;
	t = highmem_map[p];

	kmap_table[t].users--;
#ifdef PAGE_DEBUG
	if(kmap_table[t].users <= 0) BUG();
#endif
	spin_unlock(&kmap_lock);

	return;
}

/*
 *	pageing_init() initialiesiert und aktiviert
 *	das Pageing.
 */

void pageing_init()
{
	cpu_info_t *c;
	unsigned long count;
	unsigned int page;
	unsigned int ptn;
	pt_t *pt;
	unsigned int pde, pte;
	unsigned long i;

	// Features...

	c = get_bsp_cpu_info();

	if(c->feature_flags & IDF_PAE) use_pae = true;
	else use_pae = false;

	if(c->feature_flags & IDF_PGE) use_pge = true;
	else use_pge = false;

	if(c->feature_flags & IDF_PSE) use_pse = true;
	else use_pse = false;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_pageing_init();
		return;
	}

	// Spinlocks initialisieren.

	spin_lock_init(&vmem_lock);
	spin_lock_init(&kmap_lock);

	// Mother Page-Directory anlegen...

	mother_pd = PAGE2PTR(get_free_page(GFP_KERNEL));
	if(mother_pd == NULL) kernel_panic("Out of Memory");
	memset((void *) mother_pd, 0, sizeof(pd_t));

	page = 0;
	pde = FIRST_PMEM_PDE;

	while(pde <= LAST_PMEM_PDE && page < low_pages)
	{
		if(use_pse == true && page >= 0x100)
		{
			PDE_SET_ENTRIE(mother_pd->pde[pde], page, PDE_PRESENT | PDE_WRITE | PDE_PAGE_SIZE);
			if(use_pge == true) PDE_OR_FLAGS(mother_pd->pde[pde], PDE_GLOBAL);
			page += 1024;
		}
		else
		{
			ptn = get_free_page(GFP_KERNEL);
			pt = PAGE2PTR(ptn);
			if(pt == NULL) kernel_panic("Out of Memory");
			PDE_SET_ENTRIE(mother_pd->pde[pde], ptn, PDE_PRESENT | PDE_WRITE);
			memset(pt, 0, sizeof(pt_t));

			pte = 0;

			while(pte < MAX_PTE && page < low_pages)
			{
				PTE_SET_ENTRIE(pt->pte[pte], page, PTE_PRESENT | PTE_WRITE);
				if(use_pge == true) PTE_OR_FLAGS(pt->pte[pte], PTE_GLOBAL);
				if(page >= 0xA0 && page < 0x100) PTE_OR_FLAGS(pt->pte[pte], PTE_PAGE_WRITE_THROUGH | PTE_PAGE_CACHE_DISABLE);
				mem_table[ptn].u[0].used_ptes++;
				page++;
				pte++;
			}
		}

		pde++;
	}

	// Virtual Memory anlegen...

	pde = FIRST_VMEM_PDE;

	while(pde <= LAST_VMEM_PDE)
	{
		ptn = get_free_page(GFP_KERNEL);
		pt = PAGE2PTR(ptn);
		if(pt == NULL) kernel_panic("Out of Memory");
		PDE_SET_ENTRIE(mother_pd->pde[pde], ptn, PDE_PRESENT | PDE_WRITE);
		memset(pt, 0, sizeof(pt_t));

		pde++;
	}

	last_vmen_pde = FIRST_VMEM_PDE;
	last_vmen_pte = 0;

	// Pageing aktivieren...

	SET_CR3(mother_pd);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR0(CR0_PG);

	// Highmem-Map fuer kmap() anlegen...

	if(high_pages > 0)
	{
		highmem_map = vmalloc(high_pages * sizeof(short));
		if(highmem_map == NULL) kernel_panic("Out of Memory");
		for(i = 0; i < high_pages; i++)
		{highmem_map[i] = -1;}
	}

	// Speicher fuer die kmap()-Table allocieren...

	kmap_table = vmalloc(KMAP_PAGES * sizeof(struct kmap_table));
	if(kmap_table == NULL) kernel_panic("Out of Memory");
	last_kmap_nr = 0;

	// Nach freien virtuellen Adressraum fuer
	// kmap() suchen...

	count = KMAP_PAGES;

	if(!__vmem_alloc(&pde, &pte, count))
		kernel_panic("Out of Memory");

	// Virtuellen Adressraum blegen und die
	// kmap()-Table initialiesieren...

	pt = PDE_PTR(mother_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = MAX_PTE - 1;
			pt = PDE_PTR(mother_pd->pde[pde]);
		}
		else pte--;

		PTE_MAKE_BROKEN(pt->pte[pte]);
		kmap_table[count - 1].users = 0;
		kmap_table[count - 1].pte = &pt->pte[pte];
		kmap_table[count - 1].ptr = PTR_BY_PDE_PTE(pde, pte);

		count--;
	}

	return;
}

/*
 *	ap_pageing_init() aktiviert das Pageing beim AP...
 */

void ap_pageing_init()
{
	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_ap_pageing_init();
		return;
	}

	// Pageing aktivieren...

	SET_CR3(mother_pd);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR0(CR0_PG);

	return;
}
