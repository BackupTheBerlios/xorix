
/*
kernel/page_pae.h @ Xorix Operating System
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

#ifndef _PAGE_PAE_H
#define _PAGE_PAE_H

#define PAE_MAX_PTE			512
#define PAE_MAX_PDE			512
#define PAE_MAX_PDPTE			4

#define PAE_FIRST_KERNEL_PDPTE		0
#define PAE_LAST_KERNEL_PDPTE		1
#define PAE_FIRST_USER_PDPTE		2
#define PAE_LAST_USER_PDPTE		3

#define PAE_FIRST_PMEM_PDPTE		0
#define PAE_LAST_PMEM_PDPTE		1
#define PAE_FIRST_PMEM_PDE		0
#define PAE_LAST_PMEM_PDE		383

#define PAE_VMEM_PDPTE			1
#define PAE_FIRST_VMEM_PDE		384
#define PAE_LAST_VMEM_PDE		511

#define PAE_USER_CODE_PDPTE		2
#define PAE_USER_DATA_PDPTE		3

#define PAE_PTE_PRESENT			1
#define PAE_PTE_WRITE			2
#define PAE_PTE_USER			4
#define PAE_PTE_PAGE_WRITE_THROUGH	8
#define PAE_PTE_PAGE_CACHE_DISABLE	16
#define PAE_PTE_ACCESSED		32
#define PAE_PTE_DIRTY			64
#define PAE_PTE_GLOBAL			256
#define PAE_PTE_OS_MAGIC		512
#define PAE_PTE_OS_COPY_ON_WRITE	1024
#define PAE_PTE_OS_RESERVED		2048

#define PAE_PF_VMALLOC			(PAE_PTE_PRESENT | PAE_PTE_WRITE)
#define PAE_PF_IOREMAP			(PAE_PTE_PRESENT | PAE_PTE_WRITE | PAE_PTE_PAGE_WRITE_THROUGH | PAE_PTE_PAGE_CACHE_DISABLE)

#define PAE_PDE_PRESENT			1
#define PAE_PDE_WRITE			2
#define PAE_PDE_USER			4	
#define PAE_PDE_PAGE_WRITE_THROUGH	8
#define PAE_PDE_PAGE_CACHE_DISABLE	16
#define PAE_PDE_ACCESSED		32
#define PAE_PDE_DIRTY			64
#define PAE_PDE_PAGE_SIZE		128
#define PAE_PDE_GLOBAL			256

#define PAE_PDPTE_PRESENT		1
#define PAE_PDPTE_PAGE_WRITE_THROUGH	8
#define PAE_PDPTE_PAGE_CACHE_DISABLE	16

#ifndef ASM

/* Makros fuers Pageing... */

#define PAE_PTR_BY_PDPTE_PDE_PTE_OFFSET(pdpte, pde, pte, offset) \
	((void *) (((pdpte) << 30) | ((pde) << 21) | ((pte) << 12) | (offset)))

#define PAE_PTR_BY_PDPTE_PDE_PTE(pdpte, pde, pte) \
	((void *) (((pdpte) << 30) | ((pde) << 21) | ((pte) << 12)))

#define PAE_PDPTE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0xC0000000) >> 30)

#define PAE_PDE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0x3FE00000) >> 21)

#define PAE_PTE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0x001FF000) >> 12)

#define PAE_OFFSET_BY_PTR(ptr) \
	(((uintptr_t) (ptr)) & 0x00000FFF)

/* Makros zum Modifizieren der Page-Table in PAE... */

#define PAE_PTE_IS_CLEAR(pte) \
	(((pte).pae_pte_val_0_31 == 0) && \
	((pte).pae_pte_val_32_63 == 0))

#define PAE_PTE_CLEAR(pte) \
	(pte).pae_pte_val_0_31 = 0; \
	(pte).pae_pte_val_32_63 = 0

#define PAE_PTE_COPY(from_pte, to_pte) \
	(to_pte).pae_pte_val_0_31 = (from_pte).pae_pte_val_0_31; \
	(to_pte).pae_pte_val_32_63 = (from_pte).pae_pte_val_32_63

#define PAE_PTE_SET_ENTRIE(pte, page, flags) \
	(pte).pae_pte_val_0_31 = ((page) << 12) | (flags); \
	(pte).pae_pte_val_32_63 = (page) >> 20

#define PAE_PTE_TEST_FLAG(pte, flag) \
	((pte).pae_pte_val_0_31 & (flag))

#define PAE_PTE_GET_FLAGS(pte) \
	((pte).pae_pte_val_0_31 & 0xFFF)

#define PAE_PTE_SET_FLAGS(pte, flags) \
	(pte).pae_pte_val_0_31 = ((pte).pae_pte_val_0_31 & 0xFFFFF000) | (flags)

#define PAE_PTE_OR_FLAGS(pte, flags) \
	(pte).pae_pte_val_0_31 |= (flags)

#define PAE_PTE_XOR_FLAGS(pte, flags) \
	(pte).pae_pte_val_0_31 ^= (flags)

#define PAE_PTE_GET_PAGE(pte) \
	(((pte).pae_pte_val_0_31 >> 12) | ((pte).pae_pte_val_32_63 << 20))

#define PAE_PTE_SET_PAGE(pte, page) \
	(pte).pae_pte_val_0_31 = ((pte).pae_pte_val_0_31 & 0xFFF) | ((page) << 12); \
	(pte).pae_pte_val_32_63 = (page) >> 20

#define PAE_PTE_PTR(pte) \
	((void *) ((pte).pae_pte_val_0_31 & 0xFFFFF000))

#define PAE_PTE_IS_BROKEN(pte) \
	(((pte).pae_pte_val_0_31 == PAE_PTE_OS_MAGIC) && \
	((pte).pae_pte_val_32_63 == 0))

#define PAE_PTE_MAKE_BROKEN(pte) \
	(pte).pae_pte_val_0_31 = PAE_PTE_OS_MAGIC; \
	(pte).pae_pte_val_32_63 = 0

/* Makros zum Modifizieren des Page-Directorys in PAE... */

#define PAE_PDE_IS_CLEAR(pde) \
	(((pte).pae_pde_val_0_31 == 0) && \
	((pte).pae_pde_val_32_63 == 0))

#define PAE_PDE_CLEAR(pde) \
	(pde).pae_pde_val_0_31 = 0; \
	(pde).pae_pde_val_32_63 = 0

#define PAE_PDE_COPY(from_pde, to_pde) \
	(to_pde).pae_pde_val_0_31 = (from_pde).pae_pde_val_0_31; \
	(to_pde).pae_pde_val_32_63 = (from_pde).pae_pde_val_32_63

#define PAE_PDE_SET_ENTRIE(pde, page, flags) \
	(pde).pae_pde_val_0_31 = ((page) << 12) | (flags); \
	(pde).pae_pde_val_32_63 = (page) >> 20

#define PAE_PDE_TEST_FLAG(pde, flag) \
	((pde).pae_pde_val_0_31 & (flag))

#define PAE_PDE_GET_FLAGS(pde) \
	((pde).pae_pde_val_0_31 & 0xFFF)

#define PAE_PDE_SET_FLAGS(pde, flags) \
	(pde).pae_pde_val_0_31 = ((pde).pae_pde_val_0_31 & 0xFFFFF000) | (flags)

#define PAE_PDE_OR_FLAGS(pde, flags) \
	(pde).pae_pde_val_0_31 |= (flags)

#define PAE_PDE_XOR_FLAGS(pde, flags) \
	(pde).pae_pde_val_0_31 ^= (flags)

#define PAE_PDE_GET_PAGE(pde) \
	(((pde).pae_pde_val_0_31 >> 12) | ((pde).pae_pde_val_32_63 << 20))

#define PAE_PDE_SET_PAGE(pde, page) \
	(pte).pae_pde_val_0_31 = ((pde).pae_pde_val_0_31 & 0xFFF) | ((page) << 12); \
	(pte).pae_pde_val_32_63 = (page) >> 20

#define PAE_PDE_PTR(pde) \
	((void *) ((pde).pae_pde_val_0_31 & 0xFFFFF000))

/* Makros zum Modifizieren der Page-Directory-Pointer-Table in PAE... */

#define PAE_PDPTE_COPY(from_pdpte, to_pdpte) \
	(to_pdpte).pae_pdpte_val_0_31 = (from_pdpte).pae_pdpte_val_0_31; \
	(to_pdpte).pae_pdpte_val_32_63 = (from_pdpte).pae_pdpte_val_32_63

#define PAE_PDPTE_SET_ENTRIE(pdpte, page, flags) \
	(pdpte).pae_pdpte_val_0_31 = ((page) << 12) | (flags); \
	(pdpte).pae_pdpte_val_32_63 = (page) >> 20

#define PAE_PDPTE_TEST_FLAG(pdpte, flag) \
	((pdpte).pae_pdpte_val_0_31 & (flag))

#define PAE_PDPTE_GET_FLAGS(pdpte) \
	((pdpte).pae_pdpte_val_0_31 & 0xFFF)

#define PAE_PDPTE_SET_FLAGS(pdpte, flags) \
	(pdpte).pae_pdpte_val_0_31 = ((pdpte).pae_pdpte_val_0_31 & 0xFFFFF000) | (flags)

#define PAE_PDPTE_OR_FLAGS(pdpte, flags) \
	(pdpte).pae_pdpte_val_0_31 |= (flags)

#define PAE_PDPTE_XOR_FLAGS(pdpte, flags) \
	(pdpte).pae_pdpte_val_0_31 ^= (flags)

#define PAE_PDPTE_GET_PAGE(pdpte) \
	(((pdpte).pae_pdpte_val_0_31 >> 12) | ((pdpte).pae_pdpte_val_32_63 << 20))

#define PAE_PDPTE_SET_PAGE(pdpte, page) \
	(pdpte).pae_pdpte_val_0_31 = ((pdpte).pae_pdpte_val_0_31 & 0xFFF) | ((page) << 12); \
	(pdpte).pae_pdpte_val_32_63 = (page) >> 20

#define PAE_PDPTE_PTR(pdpte) \
	((void *) ((pdpte).pae_pdpte_val_0_31 & 0xFFFFF000))

/* Page-Table in PAE */

typedef struct pae_pte
{
	uint32_t pae_pte_val_0_31;
	uint32_t pae_pte_val_32_63;
} pae_pte_t;

typedef struct pae_pt
{
	pae_pte_t pte[PAE_MAX_PTE];
} pae_pt_t;

/* Page-Directory in PAE */

typedef struct pae_pde
{
	uint32_t pae_pde_val_0_31;
	uint32_t pae_pde_val_32_63;
} pae_pde_t;

typedef struct pea_pd
{
	pae_pde_t pde[PAE_MAX_PDE];
} pae_pd_t;

/* Page-Directory-Pointer-Table in PAE */

typedef struct pae_pdpte
{
	uint32_t pae_pdpte_val_0_31;
	uint32_t pae_pdpte_val_32_63;
} pae_pdpte_t;

typedef struct pae_pdpt
{
	pae_pdpte_t pdpte[PAE_MAX_PDPTE];
} pae_pdpt_t;

/* kmap()-Structuren */

struct pae_kmap_table
{
	unsigned int users;
	pae_pte_t *pte;
	void *ptr;
};

void *__pae_kmap(unsigned long page);
static inline void *pae_kmap(unsigned long page)
{
	if(page < MAX_LOW_PAGES)
		return PAGE2PTR(page);

	return __pae_kmap(page);
}

void __pae_kunmap(unsigned long page);
static inline void pae_kunmap(unsigned long page)
{
	if(page < MAX_LOW_PAGES)
		return;

	__pae_kunmap(page);
	return;
}

static inline void pae_clear_page(unsigned long page)
{
	uint32_t d0, d1;
	void *p;

	p = pae_kmap(page);

	asm volatile("cld; rep; stosl"
	             :"=D" (d0), "=c" (d1)
	             :"D" (p), "a" (0), "c" (1024)
	             :"memory");

	pae_kunmap(page);

	return;
}

static inline void pae_copy_page(unsigned long d_page, unsigned long s_page)
{
	uint32_t d0, d1, d2;
	void *d;
	void *s;

	d = pae_kmap(d_page);
	s = pae_kmap(s_page);

	asm volatile("cld; rep; movsl"
	             :"=D" (d0), "=S" (d1), "=c" (d2)
	             :"D" (d), "S" (s),"c" (1024)
	             :"memory");

	pae_kunmap(s_page);
	pae_kunmap(d_page);

	return;
}

void *pae_vmalloc(size_t size);
void pae_vfree(void *ptr);

void *pae_ioremap(void *ptr, size_t size);
void pae_iounmap(void *ptr);

void pae_pageing_init();
void pae_ap_pageing_init();

#endif

#endif











