
/*
kernel/page_pg.h @ Xorix Operating System
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

#ifndef _PAGE_PG_H
#define _PAGE_PG_H

#define PG_MAX_PDE			1024
#define PG_MAX_PTE			1024

#define PG_FIRST_KERNEL_PDE		0
#define PG_LAST_KERNEL_PDE		511
#define PG_FIRST_USER_PDE		512
#define PG_LAST_USER_PDE		1023

#define PG_FIRST_PMEM_PDE		0
#define PG_LAST_PMEM_PDE		447
#define PG_FIRST_VMEM_PDE		448
#define PG_LAST_VMEM_PDE		511

#define PG_FIRST_USER_CODE_PDE		512
#define PG_LAST_USER_CODE_PDE		767
#define PG_FIRST_USER_DATA_PDE		768
#define PG_LAST_USER_DATA_PDE		1023

#define PG_PTE_PRESENT			1
#define PG_PTE_WRITE			2
#define PG_PTE_USER			4
#define PG_PTE_PAGE_WRITE_THROUGH	8
#define PG_PTE_PAGE_CACHE_DISABLE	16
#define PG_PTE_ACCESSED			32
#define PG_PTE_DIRTY			64
#define PG_PTE_GLOBAL			256
#define PG_PTE_OS_MAGIC			512
#define PG_PTE_OS_COPY_ON_WRITE		1024
#define PG_PTE_OS_RESERVED		2048

#define PG_PF_VMALLOC			(PG_PTE_PRESENT | PG_PTE_WRITE)
#define PG_PF_IOREMAP			(PG_PTE_PRESENT | PG_PTE_WRITE | PG_PTE_PAGE_WRITE_THROUGH | PG_PTE_PAGE_CACHE_DISABLE)

#define PG_PDE_PRESENT			1
#define PG_PDE_WRITE			2
#define PG_PDE_USER			4
#define PG_PDE_PAGE_WRITE_THROUGH	8
#define PG_PDE_PAGE_CACHE_DISABLE	16
#define PG_PDE_ACCESSED			32
#define PG_PDE_DIRTY			64
#define PG_PDE_PAGE_SIZE		128
#define PG_PDE_GLOBAL			256

#ifndef ASM

/* Makros fuers Pageing... */

#define PG_PTR_BY_PDE_PTE_OFFSET(pde, pte, offset) \
	((void *) (((pde) << 22) | ((pte) << 12) | (offset)))

#define PG_PTR_BY_PDE_PTE(pde, pte) \
	((void *) (((pde) << 22) | ((pte) << 12)))

#define PG_PDE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0xFFC00000) >> 22)

#define PG_PTE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0x003FF000) >> 12)

#define PG_OFFSET_BY_PTR(ptr) \
	(((uintptr_t) (ptr)) & 0x00000FFF)

/* Makros zum Modifizieren der Page Table... */

#define PG_PTE_IS_CLEAR(pte) \
	((pte).pg_pte_val == 0)

#define PG_PTE_CLEAR(pte) \
	(pte).pg_pte_val = 0;

#define PG_PTE_COPY(from_pte, to_pte) \
	(to_pte).pg_pte_val = (from_pte).pg_pte_val;

#define PG_PTE_SET_ENTRIE(pte, page, flags) \
	(pte).pg_pte_val = ((page) << 12) | (flags)

#define PG_PTE_TEST_FLAG(pte, flag) \
	((pte).pg_pte_val & (flag))

#define PG_PTE_GET_FLAGS(pte) \
	((pte).pg_pte_val & 0xFFF)

#define PG_PTE_SET_FLAGS(pte, flags) \
	(pte).pg_pte_val = ((pte).pg_pte_val & 0xFFFFF000) | (flags)

#define PG_PTE_OR_FLAGS(pte, flags) \
	(pte).pg_pte_val |= (flags)

#define PG_PTE_XOR_FLAGS(pte, flags) \
	(pte).pg_pte_val ^= (flags)

#define PG_PTE_GET_PAGE(pte) \
	((pte).pg_pte_val >> 12)

#define PG_PTE_SET_PAGE(pte, page) \
	(pte).pg_pte_val = ((pte).pg_pte_val & 0xFFF) | ((page) << 12)

#define PG_PTE_PTR(pte) \
	((void *) ((pte).pg_pte_val & 0xFFFFF000))

#define PG_PTE_IS_BROKEN(pte) \
	((pte).pg_pte_val == PG_PTE_OS_MAGIC)

#define PG_PTE_MAKE_BROKEN(pte) \
	(pte).pg_pte_val = PG_PTE_OS_MAGIC

/* Makros zum Modifizieren des Page Directorys... */

#define PG_PDE_IS_CLEAR(pde) \
	((pde).pg_pde_val == 0)

#define PG_PDE_CLEAR(pde) \
	(pde).pg_pde_val = 0;

#define PG_PDE_COPY(from_pde, to_pde) \
	(to_pde).pg_pde_val = (from_pde).pg_pde_val;

#define PG_PDE_SET_ENTRIE(pde, page, flags) \
	(pde).pg_pde_val = ((page) << 12) | (flags)

#define PG_PDE_TEST_FLAG(pde, flag) \
	((pde).pg_pde_val & (flag))

#define PG_PDE_GET_FLAGS(pde) \
	((pde).pg_pde_val & 0xFFF)

#define PG_PDE_SET_FLAGS(pde, flags) \
	(pde).pg_pde_val = ((pde).pg_pde_val & 0xFFFFF000) | (flags)

#define PG_PDE_OR_FLAGS(pde, flags) \
	(pde).pg_pde_val |= (flags)

#define PG_PDE_XOR_FLAGS(pde, flags) \
	(pde).pg_pde_val ^= (flags)

#define PG_PDE_GET_PAGE(pde) \
	((pde).pg_pde_val >> 12)

#define PG_PDE_SET_PAGE(pde, page) \
	(pde).pg_pde_val = ((pde).pg_pde_val & 0xFFF) | ((page) << 12)

#define PG_PDE_PTR(pde) \
	((void *) ((pde).pg_pde_val & 0xFFFFF000))

/* Page-Table */

typedef struct pg_pte
{
	uint32_t pg_pte_val;
} pg_pte_t;

typedef struct pg_pt
{
	pg_pte_t pte[PG_MAX_PTE];
} pg_pt_t;

/* Page-Directory */

typedef struct pg_pde
{
	uint32_t pg_pde_val;
} pg_pde_t;

typedef struct pg_pd
{
	pg_pde_t pde[PG_MAX_PDE];
} pg_pd_t;

/* kmap()-Structuren */

struct pg_kmap_table
{
	unsigned int users;
	pg_pte_t *pte;
	void *ptr;
};

void *__pg_kmap(unsigned long page);
static inline void *pg_kmap(unsigned long page)
{
	if(page < MAX_LOW_PAGES)
		return PAGE2PTR(page);

	return __pg_kmap(page);
}

void __pg_kunmap(unsigned long page);
static inline void pg_kunmap(unsigned long page)
{
	if(page < MAX_LOW_PAGES)
		return;

	__pg_kunmap(page);
	return;
}

static inline void pg_clear_page(unsigned long page)
{
	uint32_t d0, d1;
	void *p;

	p = pg_kmap(page);

	asm volatile("cld; rep; stosl"
	             :"=D" (d0), "=c" (d1)
	             :"D" (p), "a" (0), "c" (1024)
	             :"memory");

	pg_kunmap(page);

	return;
}

static inline void pg_copy_page(unsigned long d_page, unsigned long s_page)
{
	uint32_t d0, d1, d2;
	void *d;
	void *s;

	d = pg_kmap(d_page);
	s = pg_kmap(s_page);

	asm volatile("cld; rep; movsl"
	             :"=D" (d0), "=S" (d1), "=c" (d2)
	             :"D" (d), "S" (s),"c" (1024)
	             :"memory");

	pg_kunmap(s_page);
	pg_kunmap(d_page);

	return;
}

void *pg_vmalloc(size_t size);
void pg_vfree(void *ptr);

void *pg_ioremap(void *ptr, size_t size);
void pg_iounmap(void *ptr);

void pg_pageing_init();
void pg_ap_pageing_init();

#endif

#endif











