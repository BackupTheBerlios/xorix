
/*
kernel/cpu.c @ Xorix Operating System
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
#include <inttypes.h>
#include <wchar.h>
#include <wctype.h>

#include "debug.h"
#include "idt.h"
#include "apic.h"
#include "smp.h"
#include "cpu.h"
#include "sleep.h"

int cpu_count;
cpu_info_t cpu_info[MAX_CPU];

cpu_info_t *get_bsp_cpu_info()
{
	return &cpu_info[0];
}

cpu_info_t *get_my_cpu_info()
{
	if(use_smp == true)
	{
		return apic_id_map[get_apic_id()];
	}

	return &cpu_info[0];
}

static void check_cpu(cpu_info_t *cpu_info)
{
	uint32_t f, i;
	uint32_t eax, ebx, ecx, edx;
	uint16_t fpu_temp;
	int8_t fpu_check;
	bool no_cpu_name;

	no_cpu_name = true;

	// CPUID-Support testen...

	asm("pushfl               \n"
	    "popl %%eax           \n"
	    "movl %%eax,%%ebx     \n"
	    "xorl $0x200000,%%eax \n"
	    "pushl %%eax          \n"
	    "popfl                \n"
	    "pushfl               \n"
	    "popl %%eax           \n"
	    "xorl %%ebx,%%eax     \n"
	    "jnz 1f               \n"
	    "0:                   \n"
	    "movl $0,%%eax        \n"
	    "jmp 2f               \n"
	    "1:                   \n"
	    "movl $1,%%eax        \n"
	    "2:                   \n"
	    :"=a" (cpu_info->old_feature_flags)::"ebx");
	
	if((cpu_info->old_feature_flags & OFF_CPUID))
	{
		CPUID(0, eax, ebx, ecx, edx);
		cpu_info->cpuid_level   = eax;
		cpu_info->vendor_id[0]  = *(((char *) &ebx) + 0);
		cpu_info->vendor_id[1]  = *(((char *) &ebx) + 1);
		cpu_info->vendor_id[2]  = *(((char *) &ebx) + 2);
		cpu_info->vendor_id[3]  = *(((char *) &ebx) + 3);
		cpu_info->vendor_id[4]  = *(((char *) &edx) + 0);
		cpu_info->vendor_id[5]  = *(((char *) &edx) + 1);
		cpu_info->vendor_id[6]  = *(((char *) &edx) + 2);
		cpu_info->vendor_id[7]  = *(((char *) &edx) + 3);
		cpu_info->vendor_id[8]  = *(((char *) &ecx) + 0);
		cpu_info->vendor_id[9]  = *(((char *) &ecx) + 1);
		cpu_info->vendor_id[10] = *(((char *) &ecx) + 2);
		cpu_info->vendor_id[11] = *(((char *) &ecx) + 3);
		cpu_info->vendor_id[12] = L'\0';
		
		if(cpu_info->cpuid_level >= 1)
		{
			CPUID(1, eax, ebx, ecx, edx);
			cpu_info->stepping_id     = (eax & 0x0000000F);
			cpu_info->model_number    = (eax & 0x000000F0) >> 4;
			cpu_info->family_code     = (eax & 0x00000F00) >> 8;
			cpu_info->type            = (eax & 0x00003000) >> 12;
			cpu_info->extended_model  = (eax & 0x000F0000) >> 16;
			cpu_info->extended_family = (eax & 0x0FF00000) >> 20;
			cpu_info->feature_flags   = edx;
		}
		
		CPUID(0x80000000, eax, ebx, ecx, edx);
		cpu_info->cpuid_ext_level = eax;
		
		if(cpu_info->cpuid_ext_level >= 0x80000004)
		{
			f = 0x80000002;
			i = 0;
			
			while(f <= 0x80000004)
			{
				CPUID(f, eax, ebx, ecx, edx);
				cpu_info->cpu_name[i] = *(((char *) &eax) + 0); i++;
				cpu_info->cpu_name[i] = *(((char *) &eax) + 1); i++;
				cpu_info->cpu_name[i] = *(((char *) &eax) + 2); i++;
				cpu_info->cpu_name[i] = *(((char *) &eax) + 3); i++;
				cpu_info->cpu_name[i] = *(((char *) &ebx) + 0); i++;
				cpu_info->cpu_name[i] = *(((char *) &ebx) + 1); i++;
				cpu_info->cpu_name[i] = *(((char *) &ebx) + 2); i++;
				cpu_info->cpu_name[i] = *(((char *) &ebx) + 3); i++;
				cpu_info->cpu_name[i] = *(((char *) &ecx) + 0); i++;
				cpu_info->cpu_name[i] = *(((char *) &ecx) + 1); i++;
				cpu_info->cpu_name[i] = *(((char *) &ecx) + 2); i++;
				cpu_info->cpu_name[i] = *(((char *) &ecx) + 3); i++;
				cpu_info->cpu_name[i] = *(((char *) &edx) + 0); i++;
				cpu_info->cpu_name[i] = *(((char *) &edx) + 1); i++;
				cpu_info->cpu_name[i] = *(((char *) &edx) + 2); i++;
				cpu_info->cpu_name[i] = *(((char *) &edx) + 3); i++;
				
				f++;
			}
			
			cpu_info->cpu_name[47] = L'\0';
			no_cpu_name = false;
		}
		else if(!wcscmp(cpu_info->vendor_id, L"GenuineIntel"))
		{
			switch(cpu_info->family_code)
			{
			case 3:
				wcscpy(cpu_info->cpu_name, L"Intel386");
				no_cpu_name = false;
				break;
			case 4:
				wcscpy(cpu_info->cpu_name, L"Intel486");
				no_cpu_name = false;
				break;
			case 5:
				wcscpy(cpu_info->cpu_name, L"Intel Pentium");
				no_cpu_name = false;
				break;
			case 6:
				switch(cpu_info->model_number)
				{
				case 1:
					wcscpy(cpu_info->cpu_name, L"Intel Pentium Pro");
					no_cpu_name = false;
					break;
				case 3:
				case 5:
					wcscpy(cpu_info->cpu_name, L"Intel Pentium II");
					no_cpu_name = false;
					break;
				case 6:
					wcscpy(cpu_info->cpu_name, L"Intel Celeron");
					no_cpu_name = false;
					break;
				case 7:
				case 8:
				case 11:
					wcscpy(cpu_info->cpu_name, L"Intel Pentium III");
					no_cpu_name = false;
					break;
				case 10:
					wcscpy(cpu_info->cpu_name, L"Intel Pentium III Xeon");
					no_cpu_name = false;
					break;
				}
				break;
			}
		}
		else if(!wcscmp(cpu_info->vendor_id, L"AuthenticAMD"))
		{
			switch(cpu_info->family_code)
			{
			case 4:
				if(cpu_info->model_number == 0xE || cpu_info->model_number == 0xF)
				{wcscpy(cpu_info->cpu_name, L"Am5x86");}
				else wcscpy(cpu_info->cpu_name, L"Am486");
				no_cpu_name = false;
				break;
			case 5:
				if(cpu_info->model_number <= 3)
				{
					wcscpy(cpu_info->cpu_name, L"AMD-K5");
					no_cpu_name = false;
				}
				break;
			}
		}
	}
	else
	{
		cpu_info->cpuid_level = 0;
		cpu_info->vendor_id[0] = L'\0';
		
		// 80386 oder 80486?
		
		asm("pushfl               \n"
		    "popl %%eax           \n"
		    "movl %%eax,%%ebx     \n"
		    "xorl $0x40000,%%eax  \n"
		    "pushl %%eax          \n"
		    "popfl                \n"
		    "pushfl               \n"
		    "popl %%eax           \n"
		    "xorl %%ebx,%%eax     \n"
		    "jnz 1f               \n"
		    "0:                   \n"
		    "movl $3,%%eax        \n"
		    "jmp 2f               \n"
		    "1:                   \n"
		    "movl $4,%%eax        \n"
		    "2:                   \n"
		    "pushl %%ebx          \n"
		    "popfl                \n"
		    :"=a" (cpu_info->family_code)::"ebx");
		
		cpu_info->stepping_id     = 0;
		cpu_info->model_number    = 0;
		cpu_info->type            = 0;
		cpu_info->extended_model  = 0;
		cpu_info->extended_family = 0;
		cpu_info->feature_flags   = 0;
		
		// Testen, ob ein Coprocessor vorhanden ist...
		
		asm("movb $0,%1       \n"
		    "fninit           \n"
		    "movw $0xFFFF,%0  \n"
		    "fnstsw %0        \n"
		    "cmpw $0,%0       \n"
		    "jne 0f           \n"
		    "movw $0xFFFF,%0  \n"
		    "fnstcw %0        \n"
		    "cmpw $0x037F,%0  \n"
		    "jne 0f           \n"
		    "movb $1,%1       \n"
		    "0:               \n"
		    : "=m" (fpu_temp), "=m" (fpu_check));

		if(fpu_check) cpu_info->feature_flags |= IDF_FPU;
	}
	
	if(no_cpu_name == true)
	{
		if(!wcscmp(cpu_info->vendor_id, L"GenuineIntel"))
		{swprintf(cpu_info->cpu_name, 48, L"Intel 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"AuthenticAMD"))
		{swprintf(cpu_info->cpu_name, 48, L"AMD 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"CyrixInstead"))
		{swprintf(cpu_info->cpu_name, 48, L"Cyrix 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"GenuineTMx86"))
		{swprintf(cpu_info->cpu_name, 48, L"Transmeta 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"NexGenDriven"))
		{swprintf(cpu_info->cpu_name, 48, L"NexGen 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"RiseRiseRise"))
		{swprintf(cpu_info->cpu_name, 48, L"Rise 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"UMC UMC UMC "))
		{swprintf(cpu_info->cpu_name, 48, L"UMC 80%" PRIX32 "86", cpu_info->family_code);}
		else if(!wcscmp(cpu_info->vendor_id, L"CentaurHauls"))
		{swprintf(cpu_info->cpu_name, 48, L"Centaur 80%" PRIX32 "86", cpu_info->family_code);}
		else
		{swprintf(cpu_info->cpu_name, 48, L"80%" PRIX32 "86", cpu_info->family_code);}
	}

	// FPU-Type ermitteln...

	if(cpu_info->feature_flags & IDF_FPU)
	{
		asm("fninit        \n"
		    "fld1          \n"
		    "fldz          \n"
		    "fdivp         \n"
		    "fld %%st      \n"
		    "fchs          \n"
		    "fcompp        \n"
		    "fstsw %0      \n"
		    "movw %0,%%ax  \n"
		    "movl $2,%1    \n"
		    "sahf          \n"
		    "jz 0f         \n"
		    "movl $3,%1    \n"
		    "0:            \n"
		    : "=m" (fpu_temp), "=m" (cpu_info->fpu_type));
	}
	else cpu_info->fpu_type = 0;

	//wprintk(L"FPU_TYPE: %i\n", cpu_info->fpu_type);

	// APIC noch nicht ermittelbar...

	cpu_info->apic_id = -1;

	return;
}

static void print_a_cpu_name(const cpu_info_t *cpu_info)
{
	if(iswalpha(cpu_info->cpu_name[0]))
	{
		switch(cpu_info->cpu_name[0])
		{
		case L'A':
		case L'a':
		case L'E':
		case L'e':
		case L'I':
		case L'i':
		case L'O':
		case L'o':
		case L'U':
		case L'u':
			wprintk(L"an");
			break;
		default:
			wprintk(L"a");
		}
	}
	else if(cpu_info->cpu_name[0] == L'8' &&
	        cpu_info->cpu_name[1] == L'0' &&
		cpu_info->cpu_name[3] == L'8' &&
		cpu_info->cpu_name[4] == L'6')
	{
		wprintk(L"an");
	}
	else
	{
		wprintk(L"a(n)");
	}

	wprintk(L" %ls", cpu_info->cpu_name);

	return;
}

void cpu_init()
{
	cpu_count = 1;

	// CPU testen...

	check_cpu(&cpu_info[0]);
	cpu_info[0].is_bsp = true;

	wprintk(L"The BSP is ");
	print_a_cpu_name(&cpu_info[0]);
	wprintk(L"\n");

	// Testen, ob ein Coprocessor vorhanden ist...

	if(!(cpu_info[0].feature_flags & IDF_FPU))
		kernel_panic("No Math Coprocessor");

	FSETPM();
	SET_CR0(GET_CR0() & ~CR0_EM | CR0_MP);

	// k_usleep() kalibrieren...

	calibrate_k_usleep(&cpu_info[0]);

	return;
}

void smp_cpu_init()
{
	unsigned int apic_id;

	apic_id = get_apic_id();

	if(apic_id >= MAX_APIC_ID)
		kernel_panic("The BSP has a not supported or bad APIC ID");

	cpu_info[0].apic_id = apic_id;

	apic_id_map[apic_id] = &cpu_info[0];
	wprintk(L"The BSP is CPU%" PRIu8 "\n", apic_id);

	return;
}

void ap_cpu_init()
{
	cpu_info_t *cpu;

	cpu = apic_id_map[boot_apic_id];

	// CPU testen...

	check_cpu(cpu);
	cpu->is_bsp = false;
	cpu->apic_id = boot_apic_id;

	wprintk(L"The AP is ");
	print_a_cpu_name(cpu);
	wprintk(L"\n");

	if(cpu->feature_flags != cpu_info[0].feature_flags)
	{
		wprintk(L"Error: The features of the AP and BSP are not equal - cannot use it\n");
		ap_is_unusable();
	}

	FSETPM();
	SET_CR0(GET_CR0() & ~CR0_EM | CR0_MP);

	// k_usleep() kalibrieren...

	calibrate_k_usleep(cpu);

	return;
}

