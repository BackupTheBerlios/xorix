
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

#define MAX_PDE			1024
#define MAX_PTE			1024

#define FIRST_KERNEL_PDE	0
#define LAST_KERNEL_PDE		511
#define FIRST_USER_PDE		512
#define LAST_USER_PDE		1023

#define FIRST_PMEM_PDE		0
#define LAST_PMEM_PDE		447
#define FIRST_VMEM_PDE		448
#define LAST_VMEM_PDE		511

#define FIRST_USER_CODE_PDE	512
#define LAST_USER_CODE_PDE	767
#define FIRST_USER_DATA_PDE	768
#define LAST_USER_DATA_PDE	1023

#define PTE_PRESENT		1
#define PTE_WRITE		2
#define PTE_USER		4
#define PTE_PAGE_WRITE_THROUGH	8
#define PTE_PAGE_CACHE_DISABLE	16
#define PTE_ACCESSED		32
#define PTE_DIRTY		64
#define PTE_GLOBAL		256
#define PTE_OS_MAGIC		512
#define PTE_OS_COPY_ON_WRITE	1024
#define PTE_OS_RESERVED		2048

#define PF_VMALLOC		(PTE_PRESENT | PTE_WRITE)
#define PF_IOREMAP		(PTE_PRESENT | PTE_WRITE | PTE_PAGE_WRITE_THROUGH | PTE_PAGE_CACHE_DISABLE)

#define PDE_PRESENT		1
#define PDE_WRITE		2
#define PDE_USER		4
#define PDE_PAGE_WRITE_THROUGH	8
#define PDE_PAGE_CACHE_DISABLE	16
#define PDE_ACCESSED		32
#define PDE_DIRTY		64
#define PDE_PAGE_SIZE		128
#define PDE_GLOBAL		256

#ifndef ASM

/* Makros fuers Pageing... */

#define PTR_BY_PDE_PTE_OFFSET(pde, pte, offset) \
	((void *) (((pde) << 22) | ((pte) << 12) | (offset)))

#define PTR_BY_PDE_PTE(pde, pte) \
	((void *) (((pde) << 22) | ((pte) << 12)))

#define PDE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0xFFC00000) >> 22)

#define PTE_BY_PTR(ptr) \
	((((uintptr_t) (ptr)) & 0x003FF000) >> 12)

#define OFFSET_BY_PTR(ptr) \
	(((uintptr_t) (ptr)) & 0x00000FFF)

/* Makros zum Modifizieren der Page Table... */

#define PTE_IS_CLEAR(pte) \
	((pte).pte_val == 0)

#define PTE_CLEAR(pte) \
	(pte).pte_val = 0;

#define PTE_COPY(from_pte, to_pte) \
	(to_pte).pte_val = (from_pte).pte_val;

#define PTE_SET_ENTRIE(pte, page, flags) \
	(pte).pte_val = ((page) << 12) | (flags)

#define PTE_TEST_FLAG(pte, flag) \
	((pte).pte_val & (flag))

#define PTE_GET_FLAGS(pte) \
	((pte).pte_val & 0xFFF)

#define PTE_SET_FLAGS(pte, flags) \
	(pte).pte_val = ((pte).pte_val & 0xFFFFF000) | (flags)

#define PTE_OR_FLAGS(pte, flags) \
	(pte).pte_val |= (flags)

#define PTE_XOR_FLAGS(pte, flags) \
	(pte).pte_val ^= (flags)

#define PTE_GET_PAGE(pte) \
	((pte).pte_val >> 12)

#define PTE_SET_PAGE(pte, page) \
	(pte).pte_val = ((pte).pte_val & 0xFFF) | ((page) << 12)

#define PTE_PTR(pte) \
	((void *) ((pte).pte_val & 0xFFFFF000))

#define PTE_IS_BROKEN(pte) \
	((pte).pte_val == PTE_OS_MAGIC)

#define PTE_MAKE_BROKEN(pte) \
	(pte).pte_val = PTE_OS_MAGIC

/* Makros zum Modifizieren des Page Directorys... */

#define PDE_IS_CLEAR(pde) \
	((pde).pde_val == 0)

#define PDE_CLEAR(pde) \
	(pde).pde_val = 0;

#define PDE_COPY(from_pde, to_pde) \
	(to_pde).pde_val = (from_pde).pde_val;

#define PDE_SET_ENTRIE(pde, page, flags) \
	(pde).pde_val = ((page) << 12) | (flags)

#define PDE_TEST_FLAG(pde, flag) \
	((pde).pde_val & (flag))

#define PDE_GET_FLAGS(pde) \
	((pde).pde_val & 0xFFF)

#define PDE_SET_FLAGS(pde, flags) \
	(pde).pde_val = ((pde).pde_val & 0xFFFFF000) | (flags)

#define PDE_OR_FLAGS(pde, flags) \
	(pde).pde_val |= (flags)

#define PDE_XOR_FLAGS(pde, flags) \
	(pde).pde_val ^= (flags)

#define PDE_GET_PAGE(pde) \
	((pde).pde_val >> 12)

#define PDE_SET_PAGE(pde, page) \
	(pde).pde_val = ((pde).pde_val & 0xFFF) | ((page) << 12)

#define PDE_PTR(pde) \
	((void *) ((pde).pde_val & 0xFFFFF000))




#define COPY_PAGE(from,to) \
	asm("cld; rep; movsl"::"S" (from),"D" (to),"c" (1024))



/* Page-Table */

typedef struct pte
{
	uint32_t pte_val;
} pte_t;

typedef struct pt
{
	pte_t pte[MAX_PTE];
} pt_t;

/* Page-Directory */

typedef struct pde
{
	uint32_t pde_val;
} pde_t;

typedef struct pd
{
	pde_t pde[MAX_PDE];
} pd_t;

/* kmap()-Structuren */

struct kmap_table
{
	unsigned int users;
	pte_t *pte;
	void *ptr;
};

extern volatile pd_t *mother_pd;

extern bool use_pae;
extern bool use_pge;
extern bool use_pse;

void *vmalloc(size_t size);
void vfree(void *ptr);

void *ioremap(void *ptr, size_t size);
void iounmap(void *ptr);

void *kmap(unsigned long page);
void kunmap(unsigned long page);

void pageing_init();
void ap_pageing_init();

#endif

#endif











