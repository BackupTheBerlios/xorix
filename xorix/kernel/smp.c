
/*
kernel/smp.c @ Xorix Operating System
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

#define SMP_DEBUG

#include <xorix.h>
#include <string.h>
#include <inttypes.h>

#include "debug.h"
#include "spinlock.h"
#include "entry.h"
#include "cmos.h"
#include "cpu.h"
#include "memory.h"
#include "page.h"
#include "task.h"
#include "sleep.h"
#include "idt.h"
#include "apic.h"
#include "smpboot.h"
#include "smp.h"

volatile bool use_smp = false;

static uint8_t mp_spec_rev;
static void *mp_apic_ptr;
static bool mp_imcr;
void *apic_id_map[MAX_APIC_ID];
unsigned int boot_apic_id;
static volatile bool _ap_is_up;
static volatile bool _ap_fault;
static spinlock_t halt_lock;
static spinlock_t flush_tlb_lock;

/*
 *	Testet, ob der AP in der aktullen Umgebung laufen wuerde.
 */

static bool is_ap_usable(ct_cpu_t *cpu)
{
	if(!(cpu->cpu_flags & 1))
	{
		wprintk(L"Error: CPU%" PRIu8 " is unusable\n", cpu->apic_id);
		return false;
	}
	else if(cpu->apic_id >= MAX_APIC_ID)
	{
		wprintk(L"Error: AP with bad APIC ID found - cannot use it\n");
	}

	return true;
}

/*
 *	mp_checksum() ueberprueft die Checksum
 *	von einer MP Structur.
 */

static bool mp_checksum(void *ptr, size_t size)
{
	uint8_t sum;
	uint8_t *b_ptr;

	sum = 0;
	b_ptr = ptr;
	while(size > 0)
	{
		sum += *b_ptr;
		b_ptr++;
		size--;
	}

	if(!sum) return true;
	return false;
}

/*
 *	find_floating_ptr() sucht die
 *	MP Floating Pointer Structure...
 */

static floating_ptr_t *find_floating_ptr()
{
	uintptr_t ptr;
	floating_ptr_t *fp;
	config_table_t *ct;

	// MP Floating Pointer Structure suchen...

	for(ptr = 0xF0000; ptr + 4 <= 0xFFFFF; ptr++)
	{
		if(!memcmp((char *) ptr, "_MP_", 4))
		{fp = (floating_ptr_t *) ptr; goto found;}
	}

	return NULL;

found:
	// MP Floating Pointer Structure testen...

	if(fp->length <= 0 || fp->length > 64) return NULL;
	if(!mp_checksum(fp, fp->length * 16)) return NULL;

	// MP Configuration Table, wenn vorhanden, testen...

	ct = (config_table_t *) fp->config_table;

	if(ct != NULL)
	{
		if(memcmp(ct->signature, "PCMP", 4))
			return NULL;
		if(!mp_checksum(ct, ct->base_table_length))
			return NULL;
	}

	return fp;
}

/*
 *	parse_smp_configuration() analysiert die
 *	MP Configuration Table...
 */

static void parse_mp_configuration(floating_ptr_t *fp)
{
	config_table_t *ct;
	uint8_t *type;
	int i;
	ct_cpu_t *cpu;

	mp_spec_rev = fp->spec_rev;

	// Die APIC ID Map initialiesieren...

	for(i = 0; i < MAX_APIC_ID; i++)
		apic_id_map[i] = NULL;

	// IMCR vorhanden?

	mp_imcr = fp->mp_feature2 & MPF2_IMCRP ? true : false;

	// Keine Configuration Table vorhanden?

	ct = (config_table_t *) fp->config_table;

	if(ct == NULL)
	{
		mp_apic_ptr = (void *) 0x0FEE00000;
		apic_id_map[0] = (void *) 1;
		apic_id_map[1] = (void *) 1;
		return;
	}

	mp_apic_ptr = (void *) ct->local_apic_ptr;

	// ...

	type = ((uint8_t *) ct) + sizeof(config_table_t);

	for(i = 0; i < ct->entry_count; i++)
	{
		switch(*type)
		{
		case CT_CPU:
			cpu = (ct_cpu_t *) type;

			if(is_ap_usable(cpu))
				apic_id_map[cpu->apic_id] = (void *) 1;

			type += sizeof(ct_cpu_t);
			break;
		case CT_BUS:
			type += sizeof(ct_bus_t);
			break;
		case CT_IO_APIC:
			type += sizeof(ct_io_apic_t);
			break;
		case CT_IO_INT:
			type += sizeof(ct_io_int_t);
			break;
		case CT_LOCAL_INT:
			type += sizeof(ct_local_int_t);
			break;
		default:
			BUG();
			return;
		}
	}

	return;
}

/*
 *	set_bios_shutdown() initialisiert den
 *	BIOS Shutdown-Code auf 0x0a und setzt
 *	den Warm Reset Vector...
 */

static void set_bios_shutdown(uint16_t segment, uint16_t offset)
{
	cmos_write(0x0f, 0x0a);
	*((volatile uint16_t *) 467) = offset;
	*((volatile uint16_t *) 469) = segment;
	return;
}

static void send_init_ipi(uint8_t apic_id)
{
	int status;
	int timeout;

	// INIT Level Assert...

	apic_write(APIC_ICR_HI, SET_ICR_DEST_FIELD(apic_id));
	apic_write(APIC_ICR_LOW, ICR_DM_INIT | ICR_LV_ASSERT | ICR_TM_LEVEL);

	// Warten, bis der IPI versand ist...

	status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
	timeout = 4; /* 20 Mikrosekunden */

	while(status && timeout)
	{
		k_usleep(5);

		status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
		timeout--;
	}

	// INIT Level De-assert...

	apic_write(APIC_ICR_HI, SET_ICR_DEST_FIELD(apic_id));
	apic_write(APIC_ICR_LOW, ICR_DM_INIT | ICR_TM_LEVEL);

	// Warten, bis der IPI versand ist...

	status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
	timeout = 4; /* 20 Mikrosekunden */

	while(status && timeout)
	{
		k_usleep(5);

		status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
		timeout--;
	}

	return;
}

static void send_startup_ipi(uint8_t apic_id, uint8_t vector)
{
	int status;
	int timeout;

	// START-UP...

	apic_write(APIC_ICR_HI, SET_ICR_DEST_FIELD(apic_id));
	apic_write(APIC_ICR_LOW, ICR_DM_START_UP | vector);

	// Warten, bis der IPI versand ist...

	status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
	timeout = 4; /* 20 Mikrosekunden */

	while(status && timeout)
	{
		k_usleep(5);

		status = apic_read(APIC_ICR_LOW) & ICR_BUSY;
		timeout--;
	}

	return;
}

void smp_halt()
{
	unsigned long f;

	spin_lock(&halt_lock);
	wait_halt = cpu_count;

	// Halt IPI senden...

	f = save_flags();
	disable_interrupts();
	apic_write(APIC_ICR_HI, 0);
	apic_write(APIC_ICR_LOW, ICR_DS_ALL_OTHER | V_HALT_IPI);
	restore_flags(f);

	// Auf die anderen CPU's warten...

	while(wait_halt > 1);
	spin_unlock(&halt_lock);

	return;
}

void smp_flush_tlb()
{
	unsigned long f;

	spin_lock(&flush_tlb_lock);
	wait_flush_tlb = cpu_count;

	// Flush TLB IPI senden...

	f = save_flags();
	disable_interrupts();
	apic_write(APIC_ICR_HI, 0);
	apic_write(APIC_ICR_LOW, ICR_DS_ALL_OTHER | V_FLUSH_TLB_IPI);
	restore_flags(f);

	// Auf die anderen CPU's warten...

	while(wait_flush_tlb > 1);
	spin_unlock(&flush_tlb_lock);

	return;
}

static void install_smpboot_page()
{
	memcpy((void *) SMPBOOT_OFFSET,
	       &smpboot_start,
	       &smpboot_end - &smpboot_start);

	return;
}

static bool boot_ap(unsigned int apic_id)
{
	_ap_is_up = false;
	_ap_fault = false;

	apic_id_map[apic_id] = &cpu_info[cpu_count];
	cpu_count++;
	boot_apic_id = apic_id;

	stack_for_ap = PAGE2PTR(get_free_page(GFP_KERNEL) + 1);
	set_bios_shutdown(SMPBOOT_OFFSET >> 4, 0);

	send_init_ipi(apic_id);
	k_usleep(10000);
	if(get_apic_version() >= 0x10)
	{
		send_startup_ipi(apic_id, SMPBOOT_PAGE);
		k_usleep(200);
		send_startup_ipi(apic_id, SMPBOOT_PAGE);
		k_usleep(200);
	}

	while(_ap_is_up != true && _ap_fault != true);

	if(_ap_fault == true)
	{
		cpu_count--;
		apic_id_map[apic_id] = NULL;
		return false;
	}

	return true;
}

void ap_is_up()
{
	_ap_is_up = true;
	while(use_smp == false);
	return;
}

void ap_is_unusable()
{
	asm volatile("cli");
	_ap_fault = true;
	while(1);
}

void ap_setup()
{
	ap_cpu_init();
	ap_pageing_init();
	ap_apic_init();
	ap_task_init();

	ap_is_up();
	idle();

	return;
}

/*
 *	smp_init() initialiesiert das Symmetric
 *	MultiProcessing (SMP)...
 */

void smp_init()
{
	floating_ptr_t *fp;
	int i;

	// MP Floating Pointer Structure suchen...

	fp = find_floating_ptr();
	if(fp == NULL) return;

	// ...

	parse_mp_configuration(fp);

	wprintk(L"Intel MultiProcessor Specification v1.%i\n", mp_spec_rev);
	if(mp_imcr == true) wprintk(L"PIC Mode is implemented\n");
	else wprintk(L"Virtual Wire Mode is implemented\n");

	// APIC initialiesieren...

	apic_init(mp_apic_ptr, mp_imcr);

	// CPU Structuren für SMP einrichten...

	smp_cpu_init();

	// ...

	spin_lock_init(&halt_lock);
	spin_lock_init(&flush_tlb_lock);

	// AP's booten...

	install_smpboot_page();

	for(i = 0; i < MAX_APIC_ID; i++)
	{
		if(apic_id_map[i] != NULL && i != get_apic_id())
		{
			wprintk(L"Booting CPU%u:\n", i);
			boot_ap(i);
		}
	}

	use_smp = true;

	return;
}

