
/*
kernel/page_pae.c @ Xorix Operating System
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

#include "debug.h"
#include "multiboot.h"
#include "cpu.h"
#include "spinlock.h"
#include "smp.h"
#include "memory.h"
#include "page_pae.h"

volatile pae_pdpt_t *mother_pdpt;
static pae_pd_t *vmem_pd;

static unsigned int last_vmen_pde;
static unsigned int last_vmen_pte;

static short *highmem_map;
static struct pae_kmap_table *kmap_table;
static short last_kmap_nr;

static spinlock_t pae_vmem_lock;
static spinlock_t pae_kmap_lock;

static inline void pae_flush_tlb()
{
	asm volatile("mov %%cr3, %%eax  \n"
	             "mov %%eax, %%cr3  \n"
	             :::"memory", "eax");

	if(use_smp == true)
		smp_flush_tlb();

	return;
}

static inline bool __pae_vmem_alloc(unsigned int *p_pde, unsigned int *p_pte, unsigned long count)
{
	unsigned int pde, pte;
	unsigned long c;
	pae_pt_t *pt;

	// Da beginnen, wo wir das letzte Mal
	// aufgehoert haben.

	pde = last_vmen_pde;
	pte = last_vmen_pte + 1;
	if(pte >= PAE_MAX_PTE)
	{pde++; pte = 0;}

	// Nach freien virtuellen Adressraum suchen...

	count++;
	c = 0;

	while(pde <= PAE_LAST_VMEM_PDE)
	{
		pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

		while(pte < PAE_MAX_PTE)
		{
			if(PAE_PTE_IS_CLEAR(pt->pte[pte]))
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

	pde = PAE_FIRST_VMEM_PDE;
	pte = 0;

	while(pde <= last_vmen_pde)
	{
		pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

		while(pte < PAE_MAX_PTE)
		{
			if(PAE_PTE_IS_CLEAR(pt->pte[pte]))
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

	/* last_vmen_pde = */ *p_pde = pde;
	/* last_vmen_pte = */ *p_pte = pte;

	PAE_PTE_MAKE_BROKEN(pt->pte[pte]);

	return true;
}

void *pae_vmalloc(size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	pae_pt_t *pt;

	// ...

	if(size <= 0) return NULL;
	count = BYTES2PAGES(size);

	// Nach freien virtuellen Adressraum suchen...
	
	spin_lock(&pae_vmem_lock);
	if(!__pae_vmem_alloc(&pde, &pte, count))
		{spin_unlock(&pae_vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

	while(count)
	{
		page = get_free_page(GFP_HIGHMEM);
		if(page == 0)
		{
			pae_vfree(PAE_PTR_BY_PDPTE_PDE_PTE(PAE_VMEM_PDPTE, pde, pte));
			spin_unlock(&pae_vmem_lock);
			return NULL;
		}

		if(pte == 0)
		{
			pde--; pte = PAE_MAX_PTE - 1;
			pt = PAE_PDE_PTR(vmem_pd->pde[pde]);
		}
		else pte--;

		PAE_PTE_SET_ENTRIE(pt->pte[pte], page, PAE_PF_VMALLOC);
		count--;
	}

	spin_unlock(&pae_vmem_lock);
	return PAE_PTR_BY_PDPTE_PDE_PTE(PAE_VMEM_PDPTE, pde, pte);
}

void pae_vfree(void *ptr)
{
	unsigned int pde, pte;
	pae_pt_t *pt;

	// Ist der Pointer ueberhaubt im VMEM?
	
	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}
	
	// Virtuellen Adressraum und Pages freigeben...
	
	pde = PAE_PDE_BY_PTR(ptr);
	pte = PAE_PTE_BY_PTR(ptr);
	
	wprintk(L"(%i,%i)", pde, pte);
	
	spin_lock(&pae_vmem_lock);
	pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

	while(!PAE_PTE_IS_BROKEN(pt->pte[pte]))
	{
		free_page(PAE_PTE_GET_PAGE(pt->pte[pte]));
		PAE_PTE_CLEAR(pt->pte[pte]);

		if(pte >= PAE_MAX_PTE)
		{
			pde++; pte = 0;
			pt = PAE_PDE_PTR(vmem_pd->pde[pde]);
		}
		else pte++;
	}

	PAE_PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&pae_vmem_lock);

	pae_flush_tlb();
	
	return;
}

void *pae_ioremap(void *ptr, size_t size)
{
	unsigned long page;
	unsigned long count;
	unsigned int pde, pte;
	unsigned int offset;
	pae_pt_t *pt;
	
	wprintk(L"[PAE]");
	
	// ...
	
	if(size <= 0) return NULL;
	
	page = BYTE2PAGE(((uintptr_t) ptr) - 1 + size);
	count = BYTES2PAGES(size);
	offset = PAE_OFFSET_BY_PTR(ptr);

	// Low Memory behandeln...
	
	//if(page >= 0xA0 && page < 0x100) return ptr;
	//if(page < MAX_LOW_PAGES) return NULL;
	
	// Nach freien virtuellen Adressraum suchen...

	spin_lock(&pae_vmem_lock);
	if(!__pae_vmem_alloc(&pde, &pte, count))
		{spin_unlock(&pae_vmem_lock); return NULL;}

	// Freier Adressraum gefunden!

	pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = PAE_MAX_PTE - 1;
			pt = PAE_PDE_PTR(vmem_pd->pde[pde]);
		}
		else pte--;

		PAE_PTE_SET_ENTRIE(pt->pte[pte], page, PAE_PF_IOREMAP);
		page--; count--;
	}

	spin_unlock(&pae_vmem_lock);
	return PAE_PTR_BY_PDPTE_PDE_PTE_OFFSET(PAE_VMEM_PDPTE, pde, pte, offset);
}

void pae_iounmap(void *ptr)
{
	unsigned int pde, pte;
	pae_pt_t *pt;

	// Ist der Pointer ueberhaubt im VMEM?

	if(((uintptr_t) ptr) < VMEM_OFFSET ||
	   ((uintptr_t) ptr) > VMEM_OFFSET + VMEM_LIMIT)
	{return;}

	// Virtuellen Adressraum freigeben...

	pde = PAE_PDE_BY_PTR(ptr);
	pte = PAE_PTE_BY_PTR(ptr);

	wprintk(L"(%i,%i)", pde, pte);

	spin_lock(&pae_vmem_lock);
	pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

	while(!PAE_PTE_IS_BROKEN(pt->pte[pte]))
	{
		PAE_PTE_CLEAR(pt->pte[pte]);

		if(pte >= PAE_MAX_PTE)
		{
			pde++; pte = 0;
			pt = PAE_PDE_PTR(vmem_pd->pde[pde]);
		}
		else pte++;
	}

	PAE_PTE_CLEAR(pt->pte[pte]);
	spin_unlock(&pae_vmem_lock);

	pae_flush_tlb();

	return;
}

/*
 *	pae_flush_unused_kmaps() leert unbenutzte kmaps...
 */

static inline void pae_flush_unused_kmaps()
{
	short i;
	unsigned long p;

	for(i = 0; i < KMAP_PAGES; i++)
	{
		if(kmap_table[i].users == 1)
		{
			p = PAE_PTE_GET_PAGE(*kmap_table[i].pte) - MAX_LOW_PAGES;
			highmem_map[p] = -1;
			kmap_table[i].users = 0;
			PAE_PTE_MAKE_BROKEN(*kmap_table[i].pte);
		}
	}

	pae_flush_tlb();
}

/*
 *	pae_kmap() blendet ein Page aus dem Highmem ein...
 */

void *pae_kmap(unsigned long page)
{
	short i;

	// Eine Page im Low-Memory braucht natuerlich
	// nicht neu eingeblendet zu werden...

	if(page < MAX_LOW_PAGES)
		return PAGE2PTR(page);

#ifdef PAGE_DEBUG
	if(page >= total_pages) BUG();
#endif

	// Ist die Page schon eingeblendet?

	spin_lock(&pae_kmap_lock);
	i = highmem_map[page - MAX_LOW_PAGES];

	if(i != -1)
	{
		kmap_table[i].users++;
		spin_unlock(&pae_kmap_lock);
		return kmap_table[i].ptr;
	}

	// Nach freien virtuellen Adressraum suchen...

	for(i = last_kmap_nr + 1; i < KMAP_PAGES; i++)
		if(!kmap_table[i].users) goto found;

	// Keinen gefunden? Unbenutzte kmaps leeren
	// und von vorn beginnen!

	pae_flush_unused_kmaps();

	for(i = 0; i < KMAP_PAGES; i++)
		if(!kmap_table[i].users) goto found;

	// Nix gefunden...

	BUG(); /* FIXME! */

	// Freier Adressraum gefunden!

	found:

	highmem_map[page - MAX_LOW_PAGES] = i;
	kmap_table[i].users = 2;
	PAE_PTE_SET_ENTRIE(*kmap_table[i].pte, page, PAE_PF_VMALLOC);
	last_kmap_nr = i;

	spin_unlock(&pae_kmap_lock);
	return kmap_table[i].ptr;
}

/*
 *	kunmap() blendet die Page aus dem Highmem
 *	wieder aus...
 */

void pae_kunmap(unsigned long page)
{
	unsigned long p;
	unsigned short t;

	// Pages im Low-Memory duerfen natuerlich
	// nicht ausgeblendet werden...

	if(page < MAX_LOW_PAGES)
		return;

#ifdef PAGE_DEBUG
	if(page >= total_pages) BUG();
#endif

	// Page freigeben...

	spin_lock(&pae_kmap_lock);
	p = page - MAX_LOW_PAGES;
	t = highmem_map[p];

	kmap_table[t].users--;
#ifdef PAGE_DEBUG
	if(kmap_table[t].users <= 0) BUG();
#endif
	spin_unlock(&pae_kmap_lock);

	return;
}

/*
 *	pae_pageing_init() initialiesiert das Pageing in PAE...
 */

void pae_pageing_init()
{
	cpu_info_t *c;
	unsigned long count;
	unsigned int page;
	unsigned int pdn;
	unsigned int ptn;
	pae_pd_t *pd;
	pae_pt_t *pt;
	unsigned int pdpte, pde, pte;
	unsigned long i;

	// Spinlocks initialisieren.

	spin_lock_init(&pae_vmem_lock);
	spin_lock_init(&pae_kmap_lock);

	// Mother Page-Directory-Pointer-Table anlegen...

	mother_pdpt = PAGE2PTR(get_free_page(GFP_KERNEL));
	if(mother_pdpt == NULL) kernel_panic("Out of Memory");

	for(pdpte = 0; pdpte < PAE_MAX_PDPTE; pdpte++)
	{
		pdn = get_free_page(GFP_KERNEL);
		pd = PAGE2PTR(pdn);
		if(pd == NULL) kernel_panic("Out of Memory");
		PAE_PDPTE_SET_ENTRIE(mother_pdpt->pdpte[pdpte], pdn, PAE_PDPTE_PRESENT);
		memset(pd, 0, sizeof(pae_pd_t));
		if(pdpte == PAE_VMEM_PDPTE) vmem_pd = pd;
	}

	// Kernel Physical Memory Page-Directory initialisieren...

	page = 0;

	pdpte = PAE_FIRST_PMEM_PDPTE;
	pde = PAE_FIRST_PMEM_PDE;

	while(page < low_pages)
	{
		pd = PAE_PDPTE_PTR(mother_pdpt->pdpte[pdpte]);

		while(pde < PAE_MAX_PDE && page < low_pages)
		{
			if(use_pse == true && page >= 0x100)
			{
				PAE_PDE_SET_ENTRIE(pd->pde[pde], page, PAE_PDE_PRESENT | PAE_PDE_WRITE | PAE_PDE_PAGE_SIZE);
				if(use_pge == true) PAE_PDE_OR_FLAGS(pd->pde[pde], PAE_PDE_GLOBAL);
				page += 512;
			}
			else
			{
				ptn = get_free_page(GFP_KERNEL);
				pt = PAGE2PTR(ptn);
				if(pt == NULL) kernel_panic("Out of Memory");
				PAE_PDE_SET_ENTRIE(pd->pde[pde], ptn, PAE_PDE_PRESENT | PAE_PDE_WRITE);
				memset(pt, 0, sizeof(pae_pt_t));

				pte = 0;

				while(pte < PAE_MAX_PTE && page < low_pages)
				{
					PAE_PTE_SET_ENTRIE(pt->pte[pte], page, PAE_PTE_PRESENT | PAE_PTE_WRITE);
					if(use_pge == true) PAE_PTE_OR_FLAGS(pt->pte[pte], PAE_PTE_GLOBAL);
					if(page >= 0xA0 && page < 0x100) PAE_PTE_OR_FLAGS(pt->pte[pte], PAE_PTE_PAGE_WRITE_THROUGH | PAE_PTE_PAGE_CACHE_DISABLE);
					mem_table[ptn].u[0].used_ptes++;
					page++;
					pte++;
				}
			}

			pde++;
		}

		pdpte++;
	}

	// Virtual Memory anlegen...

	pde = PAE_FIRST_VMEM_PDE;

	while(pde <= PAE_LAST_VMEM_PDE)
	{
		ptn = get_free_page(GFP_KERNEL);
		pt = PAGE2PTR(ptn);
		if(pt == NULL) kernel_panic("Out of Memory");
		PAE_PDE_SET_ENTRIE(vmem_pd->pde[pde], ptn, PAE_PDE_PRESENT | PAE_PDE_WRITE);
		memset(pt, 0, sizeof(pae_pt_t));

		pde++;
	}

	last_vmen_pde = PAE_FIRST_VMEM_PDE;
	last_vmen_pte = 0;

	// Pageing aktivieren...

	SET_CR3(mother_pdpt);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR4(CR4_PAE);
	OR_CR0(CR0_PG);

	// Highmem-Map fuer kmap() anlegen...

	if(high_pages > 0)
	{
		highmem_map = pae_vmalloc(high_pages * sizeof(short));
		if(highmem_map == NULL) kernel_panic("Out of Memory");
		for(i = 0; i < high_pages; i++)
		{highmem_map[i] = -1;}
	}

	// Speicher fuer die kmap()-Table allocieren...

	kmap_table = pae_vmalloc(KMAP_PAGES * sizeof(struct pae_kmap_table));
	if(kmap_table == NULL) kernel_panic("Out of Memory");
	last_kmap_nr = 0;

	// Nach freien virtuellen Adressraum fuer
	// kmap() suchen...

	count = KMAP_PAGES;

	if(!__pae_vmem_alloc(&pde, &pte, count))
		kernel_panic("Out of Memory");

	// Virtuellen Adressraum blegen und die
	// kmap()-Table initialiesieren...

	pt = PAE_PDE_PTR(vmem_pd->pde[pde]);

	while(count)
	{
		if(pte == 0)
		{
			pde--; pte = PAE_MAX_PTE - 1;
			pt = PAE_PDE_PTR(vmem_pd->pde[pde]);
		}
		else pte--;

		PAE_PTE_MAKE_BROKEN(pt->pte[pte]);
		kmap_table[count - 1].users = 0;
		kmap_table[count - 1].pte = &pt->pte[pte];
		kmap_table[count - 1].ptr = PAE_PTR_BY_PDPTE_PDE_PTE(PAE_VMEM_PDPTE, pde, pte);

		count--;
	}

	return;
}

/*
 *	pae_ap_pageing_init() aktiviert das Pageing beim AP...
 */

void pae_ap_pageing_init()
{
	// Pageing aktivieren...

	SET_CR3(mother_pdpt);
	if(use_pse == true) OR_CR4(CR4_PSE);
	if(use_pge == true) OR_CR4(CR4_PGE);
	OR_CR4(CR4_PAE);
	OR_CR0(CR0_PG);

	return;
}
