
/*
kernel/smp.h @ Xorix Operating System
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

#ifndef _SMP_H
#define _SMP_H

#define MAX_CPU		15
#define MAX_APIC_ID	15

#define MPF2_IMCRP	0x80

#define CT_CPU		0
#define CT_BUS		1
#define CT_IO_APIC	2
#define CT_IO_INT	3
#define CT_LOCAL_INT	4

#ifndef ASM

typedef struct ct_cpu
{
	uint8_t entry_type;
	uint8_t apic_id;
	uint8_t apic_version;
	uint8_t cpu_flags;
	uint32_t cpu_signature;
	uint32_t cpu_features;
	uint32_t reserved0;
	uint32_t reserved1;
} ct_cpu_t;

typedef struct ct_bus
{
	uint8_t entry_type;
	uint8_t id;
	char type[6];
} ct_bus_t;

typedef struct ct_io_apic
{
	uint8_t entry_type;
	uint8_t id;
	uint8_t version;
	uint32_t address;
} ct_io_apic_t;

typedef struct ct_io_int
{
	uint8_t entry_type;
	uint8_t type;
	uint16_t flags;
	uint8_t bus_id;
	uint8_t bus_irq;
	uint8_t apic_id;
	uint8_t apic_intin;
} ct_io_int_t;

typedef struct ct_local_int
{
	uint8_t entry_type;
	uint8_t type;
	uint16_t flags;
	uint8_t bus_id;
	uint8_t bus_irq;
	uint8_t apic_id;
	uint8_t apic_lintin;
} ct_local_int_t;

typedef struct config_table
{
	char signature[4];
	uint16_t base_table_length;
	uint8_t spec_rev;
	uint8_t checksum;
	char oem_id[8];
	char product_id[12];
	uint32_t oem_table_ptr;
	uint16_t oem_table_size;
	uint16_t entry_count;
	uint32_t local_apic_ptr;
	uint16_t ext_table_length;
	uint8_t ext_table_checksum;
	uint8_t reserved0;
} config_table_t;

typedef struct floating_ptr
{
	char signature[4];
	uint32_t config_table;
	uint8_t length;
	uint8_t spec_rev;
	uint8_t checksum;
	uint8_t mp_feature1;
	uint8_t mp_feature2;
	uint8_t mp_feature3;
	uint8_t mp_feature4;
	uint8_t mp_feature5;
} floating_ptr_t;

extern volatile bool use_smp;
extern void *apic_id_map[];
extern unsigned int boot_apic_id;

void smp_halt();
void smp_flush_tlb();

void ap_is_up();
void ap_is_unusable();

void smp_init();

#endif

#endif

