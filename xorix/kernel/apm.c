
/*
kernel/apm.c @ Xorix Operating System
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

#include "debug.h"
#include "multiboot.h"
#include "cpu.h"
#include "gdt.h"
#include "apm.h"

struct apm_bios_regs
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
};


struct apm_bios_entry
{
	uint32_t offset;
	uint16_t selektor;
};

struct apm_event
{
	uint16_t event;
	uint16_t info;
};

bool apm_active = false;
bool apm_support = false;
struct apm_bios_entry apm_bios_entry;
apm_table_t *apm_table;

static int call_apm_bios(struct apm_bios_regs *regs)
{
	if(!apm_support)
	{
		regs->eax = 0x00008601;
		return 0x86;
	}
	
	asm volatile("pushfl       \n"
		     "pushl %%edi  \n"
	             "pushl %%ebp  \n"
	             "lcall *%5    \n"
		     "setc %%al    \n"
		     "popl %%ebp   \n"
		     "popl %%edi   \n"
		     "popfl        \n"
	             :"=a" (regs->eax), "=b" (regs->ebx),
		      "=c" (regs->ecx), "=d" (regs->edx),"=S" (regs->esi)
		     :"m" (apm_bios_entry), "a" (regs->eax),
		      "b" (regs->ebx), "c" (regs->ecx));
	
	if(regs->eax & 1)
		return (regs->eax >> 8) & 0xff;
	
	return 0;
}

static int get_pm_event(int *event, int *info)
{
	struct apm_bios_regs regs;
	int error;
	
	regs.eax = 0x530B;
	error = call_apm_bios(&regs);
	
	if(!error)
	{
		*event = regs.ebx & 0xffff;
		*info = regs.ecx & 0xffff;
		return 0;
	}
	
	return error;
}

static int set_power_state(int device, int state)
{
	struct apm_bios_regs regs;
	int error;
	
	regs.eax = 0x5307;
	regs.ebx = device;
	regs.ecx = state;
	error = call_apm_bios(&regs);
	
	return error;
}

void apm_power_off()
{
	struct apm_bios_regs regs;
	
	// Versuchen den PC auszuschalten...
	
	set_power_state(APM_DEVICE_ALL, APM_STATE_OFF);
	
	// Wenn es nicht funktioniert, ...
	
	CLI();
	HLT();
	
	return;
}

void apm_init()
{
	apm_table_t *apm_table;
	uint32_t a, c;
	int i;
	
	struct apm_bios_regs regs;
	bool power;
	int f, e;
	int event, info;
	int error;
	
	// Ist APM >= 1.2 vorhanden?
	
	if(!(multiboot_info->flags & MB_INFO_APM_TABLE)) return;
	apm_table = (apm_table_t *) multiboot_info->apm_table;
	if(apm_table->version < 0x0102) return;
	if(!(apm_table->flags & 2)) return;
	
	// Jep, ist es...
	
	wprintk(L"APM: APM BIOS version %i.%i found\n",
	        (apm_table->version >> 8) & 0xff,
		apm_table->version & 0xff);
	
	set_gdt_deskriptor(APM_CS_SELEKTOR, 
	                   apm_table->cseg << 4, 
	                   (apm_table->cseg_len - 1) & 0xffff,
	                   GT_CODESEG | GT_PRESENT, 
	                   GG_80386);
	set_gdt_deskriptor(APM_CS_16_SELEKTOR, 
		           apm_table->cseg_16 << 4, 
	                   (apm_table->cseg_16_len - 1) & 0xffff, 
	                   GT_CODESEG | GT_PRESENT, 
	                   0);
	set_gdt_deskriptor(APM_DS_SELEKTOR, 
	                   apm_table->dseg << 4, 
	                   (apm_table->dseg_len - 1) & 0xffff, 
	                   GT_DATASEG | GT_PRESENT, 
	                   GG_80386);
	
	apm_bios_entry.offset = apm_table->offset;
	apm_bios_entry.selektor = APM_CS_SELEKTOR;
	
	apm_support = true;
	
	//apm_poweroff();
	
	f = 0;
	while(1)
	{
		error = get_pm_event(&event, &info);
		wprintk(L"{%i,%x,%x}\n",
		        error, event, info);
		
		if(!error)
		{
			switch(f)
			{
			case 0:
				wprintk(L"APM_DEVICE_PARALLEL_ALL = APM_STATE_OFF\n");
				e = set_power_state(APM_DEVICE_PARALLEL(0),
				                    APM_STATE_OFF);
				wprintk(L"[=%x]\n", e);
				break;
			case 1:
				wprintk(L"APM_DEVICE_PARALLEL_ALL = APM_STATE_APM_ENABLED\n");
				e = set_power_state(APM_DEVICE_PARALLEL(0),
				                    APM_STATE_APM_ENABLED);
				wprintk(L"[=%x]\n", e);
				break;
			case 2:
				wprintk(L"APM_DEVICE_DISPLAY(0) = APM_STATE_OFF\n");
				e = set_power_state(APM_DEVICE_DISPLAY(0),
				                    APM_STATE_OFF);
				wprintk(L"[=%x]\n", e);
				break;
			case 3:
				wprintk(L"APM_DEVICE_DISPLAY(0) = APM_STATE_APM_ENABLED\n");
				e = set_power_state(APM_DEVICE_DISPLAY(0),
				                    APM_STATE_APM_ENABLED);
				wprintk(L"[=%x]\n", e);
				break;
			case 4:
				wprintk(L"e111 = APM_STATE_OFF\n");
				e = set_power_state(0xe111,
				                    APM_STATE_OFF);
				wprintk(L"[=%x]\n", e);
				break;
			case 5:
				wprintk(L"APM_DEVICE_STORAGE(0) = APM_STATE_OFF\n");
				e = set_power_state(APM_DEVICE_STORAGE(0),
				                    APM_STATE_OFF);
				wprintk(L"[=%x]\n", e);
				break;
			case 6:
				wprintk(L"APM_DEVICE_ALL = APM_STATE_OFF\n[=?]\n");
				apm_power_off();
			}
			
			f++;
		}
		
		for(i = 0; i < (128 * 1024 * 1024); i++);
	}
	
	wprintk(L"COOL!");
	wprintk(L" (%x:%x)", a, c);
	
	xxx: goto xxx;
	
	return;
}








