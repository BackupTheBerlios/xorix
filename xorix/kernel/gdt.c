
/*
kernel/gdt.c @ Xorix Operating System
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

#include "cpu.h"
#include "gdt.h"

struct gdt_deskriptor gdt[GDT_ENTRIES];

/*
 *	set_gdt_deskriptor() setzt einen Deskriptor...
 */

void set_gdt_deskriptor(uint16_t selektor, uint32_t base, uint32_t limit, uint8_t type, uint8_t gran)
{
	struct gdt_deskriptor *gdt_deskriptor;

	// Deskriptor aus dem Selektor ermitteln...

	gdt_deskriptor = &gdt[selektor >> 3];

	// Segment-Größe setzen...

	gdt_deskriptor->limit_0_15 = limit & 0xFFFF;
	gdt_deskriptor->limit_16_19 = (limit >> 16) & 0xFF;

	// Segment-Type setzen...

	gdt_deskriptor->type = type;

	// Granularity setzen...

	gdt_deskriptor->limit_16_19 += gran;

	// Base setzen...

	gdt_deskriptor->base_0_15 = base & 0xFFFF;
	gdt_deskriptor->base_16_23 = (base >> 16) & 0xFF;
	gdt_deskriptor->base_24_31 = (base >> 24) & 0xFF;

	return;
}

/*
 *	set_tss_deskriptor() setzt einen TSS-Deskriptor...
 */

void set_tss_deskriptor(uint16_t selektor, tss_t *tss)
{
	if(tss == NULL) set_gdt_deskriptor(selektor, 0, 0, 0, 0);
	else set_gdt_deskriptor(selektor, (uint32_t) tss, sizeof(tss_t) - 1, GT_TSS_80386 | GT_PRESENT, 0);
	return;
}

/*
 *	load_own_gdt() laedt die eigene GDT...
 */

static void load_own_gdt()
{
	struct
	{
		uint16_t dummy;
		uint16_t limit;
		uint32_t base;
	} gdtr;

	gdtr.limit = GDT_LIMIT;
	gdtr.base = (uint32_t) gdt;

	asm volatile("lgdt (%%eax)   \n"
	             "mov %%bx,%%ds  \n"
	             "mov %%bx,%%es  \n"
	             "mov %%bx,%%ss  \n"
	             "mov %%bx,%%fs  \n"
	             "mov %%bx,%%gs  \n"
	             "ljmp $" STR(KERNEL_CODE_SELEKTOR) ",$0f\n"
	             "0:             \n"
	             ::"a" (&gdtr.limit), "b" (KERNEL_DATA_SELEKTOR));

	return;
}

/*
 *	load_tr() laedt das Task-Register...
 */

void load_tr(uint16_t selektor)
{
	asm volatile("ltr %%ax"::"a" (selektor));
}

/*
 *	gdt_init() initialisiert die GDT...
 */

void gdt_init()
{
	int i;

	set_gdt_deskriptor(DUMMY_SELEKTOR, 0, 0, 0, 0);
	set_gdt_deskriptor(KERNEL_CODE_SELEKTOR, 0, 0x7FFFF, GT_CODESEG | GT_PRESENT, GG_80386 | GG_GRAN);
	set_gdt_deskriptor(KERNEL_DATA_SELEKTOR, 0, 0x7FFFF, GT_DATASEG | GT_PRESENT, GG_80386 | GG_GRAN);
	set_gdt_deskriptor(USER_CODE_SELEKTOR, 0x80000000, 0x3FFFF, GT_CODESEG | GT_DPL3 | GT_PRESENT, GG_80386 | GG_GRAN);
	set_gdt_deskriptor(USER_DATA_SELEKTOR, 0x80000000, 0x7FFFF, GT_DATASEG | GT_DPL3 | GT_PRESENT, GG_80386 | GG_GRAN);
	set_gdt_deskriptor(APM_CS_SELEKTOR, 0, 0, 0, 0);
	set_gdt_deskriptor(APM_CS_16_SELEKTOR, 0, 0, 0, 0);
	set_gdt_deskriptor(APM_DS_SELEKTOR, 0, 0, 0, 0);

	for(i = 0; i <= MAX_TSS_SELEKTOR; i++)
	{
		set_tss_deskriptor(TSS_SELEKTOR(i), NULL);
	}

	load_own_gdt();

	return;
}

