
/* multiboot.h - the header for Multiboot */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

/* Modified for Xorix OS by Ingmar Friedrichsen <ingmar@xorix.org>. */

#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

/* Macros. */

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the ELF Multiboot header. */

# define MULTIBOOT_HEADER_FLAGS		0x00000003

/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

/* The Multiboot information flags */

#define MB_INFO_MEMORY			0x00000001
#define MB_INFO_BOOTDEV			0x00000002
#define MB_INFO_CMDLINE			0x00000004
#define MB_INFO_MODS			0x00000008
#define MB_INFO_AOUT_SYMS		0x00000010
#define MB_INFO_ELF_SHDR		0x00000020
#define MB_INFO_MEM_MAP			0x00000040
#define MB_INFO_DRIVE_INFO		0x00000080
#define MB_INFO_CONFIG_TABLE		0x00000100
#define MB_INFO_BOOT_LOADER_NAME	0x00000200
#define MB_INFO_APM_TABLE		0x00000400
#define MB_INFO_VIDEO_INFO		0x00000800

#ifndef ASM
/* Do not include here in boot.S. */

/* Types. */

/* The Multiboot header. */
typedef struct multiboot_header
{
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
} multiboot_header_t;

/* The symbol table for a.out. */
typedef struct aout_symbol_table
{
	uint32_t tabsize;
	uint32_t strsize;
	uint32_t addr;
	uint32_t reserved;
} aout_symbol_table_t;

/* The section header table for ELF. */
typedef struct elf_section_header_table
{
	uint32_t num;
	uint32_t size;
	uint32_t addr;
	uint32_t shndx;
} elf_section_header_table_t;

/* The Multiboot information. */
typedef struct multiboot_info
{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;
	union
	{
		aout_symbol_table_t aout_sym;
		elf_section_header_table_t elf_sec;
	} u;
	uint32_t mmap_length;
	uint32_t mmap_addr;
	uint32_t drives_length;
	uint32_t drives_addr;
	uint32_t config_table;
	uint32_t boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
} multiboot_info_t;

/* The module structure. */
typedef struct module
{
	uint32_t mod_start;
	uint32_t mod_end;
	uint32_t string;
	uint32_t reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map
{
	uint32_t size;
	uint64_t base_addr;
	uint64_t length;
	uint32_t type;
} memory_map_t;

/* Drive Info structure.  */
typedef struct drive_info
{
	uint32_t size;
	uint8_t drive_number;
	uint8_t drive_mode;
	uint16_t drive_cylinders;
	uint8_t drive_heads;
	uint8_t drive_sectors;
	uint16_t drive_ports[0];
} drive_info_t;

/* The apm table. */
typedef struct apm_table
{
	uint16_t version;
	uint16_t cseg;
	uint32_t offset;
	uint16_t cseg_16;
	uint16_t dseg;
	uint16_t flags;
	uint16_t cseg_len;
	uint16_t cseg_16_len;
	uint16_t dseg_len;
} apm_table_t;

/* ELF symbols */
extern void _start;
extern void _end;

/* Multiboot information */
extern struct multiboot_info *multiboot_info;

/* BSP-TSS */
extern void bsp_tss;

/* Kernel Stack */
extern void kernel_stack;

/* IDLE Stack */
extern void idle_stack;

#endif /* ! ASM */
#endif /* _MULTIBOOT_H */




