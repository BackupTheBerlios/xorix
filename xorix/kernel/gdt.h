
/*
kernel/gdt.h @ Xorix Operating System
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

#ifndef _GDT_H
#define _GDT_H

#define GDT_ENTRIES			25
#define GDT_LIMIT			199

#define MAX_TSS_SELEKTOR		15

#define DUMMY_SELEKTOR			0x00
#define KERNEL_CODE_SELEKTOR		0x08
#define KERNEL_DATA_SELEKTOR		0x10
#define USER_CODE_SELEKTOR		0x1B
#define USER_DATA_SELEKTOR		0x23
#define APM_CS_SELEKTOR			0x28
#define APM_CS_16_SELEKTOR		0x30
#define APM_DS_SELEKTOR			0x38
#define BSP_TSS_SELEKTOR		0x40
#define TSS_SELEKTOR(x)			(0x48 + ((x) * 8))

#define GT_ACCESSED			0x01
#define GT_DATASEG_READ_ONLY		0x10
#define GT_DATASEG			0x12
#define GT_DATASEG_EXPANDABLE_READ_ONLY	0x14
#define GT_DATASEG_EXPANDABLE		0x16
#define GT_CODESEG_EXEC_ONLY		0x18
#define GT_CODESEG			0x1A
#define GT_CODESEG_CONFORM_EXEC_ONLY	0x1C
#define GT_CODESEG_CONFORM		0x1E
#define GT_DPL0				0x00
#define GT_DPL1				0x20
#define GT_DPL2				0x40
#define GT_DPL3				0x60
#define GT_PRESENT			0x80
#define GT_TSS_80286			0x01
#define GT_TSS_80286_ACTIVE		0x03
#define GT_TSS_80386			0x09
#define GT_TSS_80386_ACTIVE		0x0B

#define GG_GRAN				0x80
#define GG_80386			0x40
#define GG_AVL				0x10

#ifndef ASM

struct gdt_deskriptor
{
	uint16_t limit_0_15;
	uint16_t base_0_15;
	uint8_t base_16_23;
	uint8_t type;
	uint8_t limit_16_19;
	uint8_t base_24_31;
};

extern struct gdt_deskriptor gdt[GDT_ENTRIES];

void set_gdt_deskriptor(uint16_t selektor, uint32_t offset, uint32_t limit, uint8_t type, uint8_t gran);
void set_tss_deskriptor(uint16_t selektor, tss_t *tss);
void load_tr(uint16_t selektor);
void gdt_init();

#endif

#endif







