
/*
kernel/cpu.h @ Xorix Operating System
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

#ifndef _CPU_H
#define _CPU_H

#define F_CF		0x0001
#define F_PF		0x0004
#define F_AF		0x0010
#define F_ZF		0x0040
#define F_SF		0x0080
#define F_TF		0x0100
#define F_IF		0x0200
#define F_DF		0x0400
#define F_OF		0x0800
#define F_IOPL_0	0x0000
#define F_IOPL_1	0x1000
#define F_IOPL_2	0x2000
#define F_IOPL_3	0x3000
#define F_NT		0x4000

#define EF_CF		0x00000001
#define EF_PF		0x00000004
#define EF_AF		0x00000010
#define EF_ZF		0x00000040
#define EF_SF		0x00000080
#define EF_TF		0x00000100
#define EF_IF		0x00000200
#define EF_DF		0x00000400
#define EF_OF		0x00000800
#define EF_IOPL_0	0x00000000
#define EF_IOPL_1	0x00001000
#define EF_IOPL_2	0x00002000
#define EF_IOPL_3	0x00003000
#define EF_NT		0x00004000
#define EF_RF		0x00010000
#define EF_VM		0x00020000
#define EF_AC		0x00040000
#define EF_VIF		0x00080000
#define EF_VIP		0x00100000
#define EF_ID		0x00200000

#define MSW_PE		0x0001
#define MSW_MP		0x0002
#define MSW_EM		0x0004
#define MSW_TS		0x0008
#define MSW_ET		0x0010
#define MSW_NE		0x0020

#define CR0_PE		0x00000001
#define CR0_MP		0x00000002
#define CR0_EM		0x00000004
#define CR0_TS		0x00000008
#define CR0_ET		0x00000010
#define CR0_NE		0x00000020
#define CR0_WP		0x00010000
#define CR0_AM		0x00040000
#define CR0_NW		0x20000000
#define CR0_CD		0x40000000
#define CR0_PG		0x80000000

#define CR3_PWT		0x00000008
#define CR3_PCD		0x00000010

#define CR4_VME		0x00000001
#define CR4_PVI		0x00000002
#define CR4_TSD		0x00000004
#define CR4_DE		0x00000008
#define CR4_PSE		0x00000010
#define CR4_PAE		0x00000020
#define CR4_MCE		0x00000040
#define CR4_PGE		0x00000080
#define CR4_PCE		0x00000100
#define CR4_OSFXR	0x00000200
#define CR4_OSXMMEXCPT	0x00000400

#define OFF_CPUID	0x00000001

#define IDF_FPU		0x00000001
#define IDF_VME		0x00000002
#define IDF_DE		0x00000004
#define IDF_PSE		0x00000008
#define IDF_TSC		0x00000010
#define IDF_MSR		0x00000020
#define IDF_PAE		0x00000040
#define IDF_MCE		0x00000080
#define IDF_CX8		0x00000100
#define IDF_APIC	0x00000200
#define IDF_SEP		0x00000800
#define IDF_MTTR	0x00001000
#define IDF_PGE		0x00002000
#define IDF_MCA		0x00004000
#define IDF_CMOV	0x00008000
#define IDF_PAT		0x00010000
#define IDF_PSE_36	0x00020000
#define IDF_PSN		0x00040000
#define IDF_CLFSH	0x00080000
#define IDF_DS		0x00200000
#define IDF_ACPI	0x00400000
#define IDF_MMX		0x00800000
#define IDF_FXSR	0x01000000
#define IDF_SSE		0x02000000
#define IDF_SSE2	0x04000000
#define IDF_SS		0x08000000
#define IDF_TM		0x20000000
#define IDF_3DNOW_EXT	0x40000000
#define IDF_3DNOW	0x80000000

#ifndef ASM

#define CPUID(function, eax, ebx, ecx, edx) \
	asm volatile("cpuid":"=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx):"a" (function))

#define CLI() asm volatile("cli")
#define STI() asm volatile("sti")
#define CLTS() asm volatile("clts")

#define OUTB(value, port) asm volatile("outb %%al,%%dx"::"d" (port), "a" (value))
#define INB(port) ({uint8_t value; asm volatile("inb %%dx,%%al":"=a" (value):"d" (port)); value;})

#define OUTW(value, port) asm volatile("outw %%ax,%%dx"::"d" (port), "a" (value))
#define INW(port) ({uint16_t value; asm volatile("inw %%dx,%%ax":"=a" (value):"d" (port)); value;})

#define SET_MSW(value) asm volatile("lmsww %%ax"::"a" (value))
#define GET_MSW() ({uint32_t value; asm volatile("smsww %%ax":"=a" (value)); value;})
#define AND_MSW(value) asm volatile("smsww %%ax; andw %%bx,%%ax; lmsww %%ax"::"b" (value):"ax")
#define OR_MSW(value) asm volatile("smsww %%ax; orw %%bx,%%ax; lmsww %%ax"::"b" (value):"ax")
#define XOR_MSW(value) asm volatile("smsww %%ax; xorw %%bx,%%ax; lmsww %%ax"::"b" (value):"ax")

#define SET_CR0(value) asm volatile("movl %%eax,%%cr0"::"a" (value))
#define GET_CR0() ({uint32_t value; asm volatile("movl %%cr0,%%eax":"=a" (value)); value;})
#define AND_CR0(value) asm volatile("movl %%cr0,%%eax; andl %%ebx, %%eax; movl %%eax,%%cr0"::"b" (value):"eax");
#define OR_CR0(value) asm volatile("movl %%cr0,%%eax; orl %%ebx, %%eax; movl %%eax,%%cr0"::"b" (value):"eax");
#define XOR_CR0(value) asm volatile("movl %%cr0,%%eax; xorl %%ebx, %%eax; movl %%eax,%%cr0"::"b" (value):"eax");

#define SET_CR1(value) asm volatile("movl %%eax,%%cr1"::"a" (value))
#define GET_CR1() ({uint32_t value; asm volatile("movl %%cr1,%%eax":"=a" (value)); value;})
#define AND_CR1(value) asm volatile("movl %%cr1,%%eax; andl %%ebx, %%eax; movl %%eax,%%cr1"::"b" (value):"eax");
#define OR_CR1(value) asm volatile("movl %%cr1,%%eax; orl %%ebx, %%eax; movl %%eax,%%cr1"::"b" (value):"eax");
#define XOR_CR1(value) asm volatile("movl %%cr1,%%eax; xorl %%ebx, %%eax; movl %%eax,%%cr1"::"b" (value):"eax");

#define SET_CR2(value) asm volatile("movl %%eax,%%cr2"::"a" (value))
#define GET_CR2() ({uint32_t value; asm volatile("movl %%cr2,%%eax":"=a" (value)); value;})
#define AND_CR2(value) asm volatile("movl %%cr2,%%eax; andl %%ebx, %%eax; movl %%eax,%%cr2"::"b" (value):"eax");
#define OR_CR2(value) asm volatile("movl %%cr2,%%eax; orl %%ebx, %%eax; movl %%eax,%%cr2"::"b" (value):"eax");
#define XOR_CR2(value) asm volatile("movl %%cr2,%%eax; xorl %%ebx, %%eax; movl %%eax,%%cr2"::"b" (value):"eax");

#define SET_CR3(value) asm volatile("movl %%eax,%%cr3"::"a" (value))
#define GET_CR3() ({uint32_t value; asm volatile("movl %%cr3,%%eax":"=a" (value)); value;})
#define AND_CR3(value) asm volatile("movl %%cr3,%%eax; andl %%ebx, %%eax; movl %%eax,%%cr3"::"b" (value):"eax");
#define OR_CR3(value) asm volatile("movl %%cr3,%%eax; orl %%ebx, %%eax; movl %%eax,%%cr3"::"b" (value):"eax");
#define XOR_CR3(value) asm volatile("movl %%cr3,%%eax; xorl %%ebx, %%eax; movl %%eax,%%cr3"::"b" (value):"eax");

#define SET_CR4(value) asm volatile("movl %%eax,%%cr4"::"a" (value))
#define GET_CR4() ({uint32_t value; asm volatile("movl %%cr4,%%eax":"=a" (value)); value;})
#define AND_CR4(value) asm volatile("movl %%cr4,%%eax; andl %%ebx, %%eax; movl %%eax,%%cr4"::"b" (value):"eax");
#define OR_CR4(value) asm volatile("movl %%cr4,%%eax; orl %%ebx, %%eax; movl %%eax,%%cr4"::"b" (value):"eax");
#define XOR_CR4(value) asm volatile("movl %%cr4,%%eax; xorl %%ebx, %%eax; movl %%eax,%%cr4"::"b" (value):"eax");

#define RDTSC() ({uint32_t value; asm volatile("rdtsc":"=a" (value)); value;})

#define _LJMP(selektor, offset) asm volatile("ljmp $" #selektor ",$" #offset)
#define LJMP(selektor, offset) _LJMP(selektor, offset)
#define LTR(value) asm volatile("ltr %%ax"::"a" (value))
#define HLT() asm volatile("hlt")

#define FSETPM() asm volatile(".byte 0xDB,0xE4")

typedef struct tss
{
	uint16_t back_link, reserved0;
	uint32_t esp0;
	uint16_t ss0, reserved1;
	uint32_t esp1;
	uint16_t ss1, reserved2;
	uint32_t esp2;
	uint16_t ss2, reserved3;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es, reserved4;
	uint16_t cs, reserved5;
	uint16_t ss, reserved6;
	uint16_t ds, reserved7;
	uint16_t fs, reserved8;
	uint16_t gs, reserved9;
	uint16_t ldt, reserved10;
	uint16_t trap_on_task, io_bitmap;
} tss_t;

#endif

#define TSS_ESP0	4

#ifndef ASM

typedef struct cpu_info
{
	wchar_t cpu_name[48];
	uint32_t old_feature_flags;
	uint32_t fpu_type;
	uint32_t cpuid_level;
	uint32_t cpuid_ext_level;
	wchar_t vendor_id[13];
	uint32_t stepping_id;
	uint32_t model_number;
	uint32_t family_code;
	uint32_t type;
	uint32_t extended_model;
	uint32_t extended_family;
	uint32_t feature_flags;
	unsigned long loops_per_jiffie;
	int apic_id;
	bool is_bsp;
} cpu_info_t;

extern int cpu_count;
extern cpu_info_t cpu_info[];

static inline unsigned long save_flags()
{
	unsigned long flags;

	asm volatile("pushfl   \n\t"
	             "popl %0  \n\t":"=r" (flags)::"memory");

	return flags;
}

static inline void restore_flags(unsigned long flags)
{
	asm volatile("pushl %0  \n\t"
	             "popfl     \n\t"::"r" (flags):"memory");

	return;
}

static inline void disable_interrupts()
{
	asm volatile("cli");
	return;
}

static inline void enable_interrupts()
{
	asm volatile("sti");
	return;
}

cpu_info_t *get_bsp_cpu_info();
cpu_info_t *get_my_cpu_info();
void cpu_init();
void smp_cpu_init();
void ap_cpu_init();

#endif

#endif
