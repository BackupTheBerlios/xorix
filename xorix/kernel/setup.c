
/*
kernel/setup.c @ Xorix Operating System
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
#include <wchar.h>
#include <wctype.h>
#include <string.h>

#include <errno.h>

#include "multiboot.h"
#include "debug.h"
#include "memory.h"
#include "page.h"
#include "cpu.h"
#include "idt.h"
#include "gdt.h"
#include "apm.h"
#include "shutdown.h"
#include "smp.h"
#include "task.h"
#include "setup.h"

#include <limits.h>

#include "entry.h"

#define MBI_MEM_SIZE 1024
char mbi_mem[MBI_MEM_SIZE];

multiboot_info_t *evacuate_multiboot_info(multiboot_info_t *mbi)
{
#define EVACUATE_MEM(mem, size) { \
	z = (size); \
	if(i + z >= MBI_MEM_SIZE) return NULL; \
	memcpy(&mbi_mem[i], (mem), z); \
	i += z; }
#define EVACUATE_STR(str) { \
	z = strlen((str)) + 1; \
	if(i + z >= MBI_MEM_SIZE) return NULL; \
	memcpy(&mbi_mem[i], (str), z); \
	i += z; }

	multiboot_info_t *new_mbi;
	size_t i = 0;
	size_t z;
	module_t *mods;
	module_t *new_mods;
	uint32_t mods_count;
	uint32_t m;

	/* Multiboot Information */

	new_mbi = (multiboot_info_t *) &mbi_mem[i];
	EVACUATE_MEM(mbi, sizeof(multiboot_info_t));

	/* Kernel Command Line */

	if(mbi->flags & MB_INFO_CMDLINE)
	{
		new_mbi->cmdline = (uint32_t) &mbi_mem[i];
		EVACUATE_STR((char *) mbi->cmdline);
	}

	/* Module */

	if((mbi->flags & MB_INFO_MODS) && mbi->mods_count > 0)
	{
		new_mbi->mods_addr = (uint32_t) &mbi_mem[i];
		EVACUATE_MEM((module_t *) mbi->mods_addr,
		             mbi->mods_count * sizeof(module_t));

		mods = (module_t *) mbi->mods_addr;
		new_mods = (module_t *) new_mbi->mods_addr;
		mods_count = mbi->mods_count;

		for(m = 0; m < mods_count; m++)
		{
			if((char *) mods[m].string != NULL)
			{
				new_mods->string = (uint32_t) &mbi_mem[i];
				EVACUATE_STR((char *) mods[m].string);
			}
		}
	}

	/* BIOS Memory Map */

	if(mbi->flags & MB_INFO_MEM_MAP)
	{
		new_mbi->mmap_addr = (uint32_t) &mbi_mem[i];
		EVACUATE_MEM((memory_map_t *) mbi->mmap_addr, mbi->mmap_length);
	}

	/* Drive Info */

	if(mbi->flags & MB_INFO_DRIVE_INFO)
	{
		new_mbi->drives_addr = (uint32_t) &mbi_mem[i];
		EVACUATE_MEM((drive_info_t *) mbi->drives_addr, mbi->drives_length);
	}

	/* BIOS ROM Configuration Table (FIXME) */

	new_mbi->flags &= ~MB_INFO_CONFIG_TABLE;

	/* Boot Loader Name */

	if(mbi->flags & MB_INFO_BOOT_LOADER_NAME)
	{
		new_mbi->boot_loader_name = (uint32_t) &mbi_mem[i];
		EVACUATE_STR((char *) mbi->boot_loader_name);
	}

	/* APM Table */

	if(mbi->flags & MB_INFO_APM_TABLE)
	{
		new_mbi->apm_table = (uint32_t) &mbi_mem[i];
		EVACUATE_MEM((apm_table_t *) mbi->apm_table, sizeof(apm_table_t));
	}

	return new_mbi;

#undef EVACUATE_MEM
#undef EVACUATE_STR
}

#include "apic.h"

void t_b()
{
	while(1)
	{
		wprintk(L"Kernel-Thread 1 @ CPU%u\n", use_smp == true ? get_apic_id() : 0);
		//asm volatile("hlt");
		offer_reschedule();
	}

	return;
}

void t_c()
{
	while(1)
	{
		wprintk(L"Kernel-Thread 2 @ CPU%u\n", use_smp == true ? get_apic_id() : 0);
		//asm volatile("hlt");
		offer_reschedule();
	}

	return;
}

void setup()
{
	idt_init();
	gdt_init();
	cpu_init();

	multiboot_info = evacuate_multiboot_info(multiboot_info);
	if(multiboot_info == NULL) BUG();

	memory_init();
	pageing_init();
	task_init();
	smp_init();
	//apm_init();

	create_kernel_thread(t_b);
	create_kernel_thread(t_c);

	while(1)
	{
		wprintk(L"Kernel-Thread 0 @ CPU%u\n", use_smp == true ? get_apic_id() : 0);
		//asm volatile("hlt");
		offer_reschedule();
	}

	//reboot();

	return;
}
