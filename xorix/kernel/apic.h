
/*
kernel/apic.h @ Xorix Operating System
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

#ifndef _APIC_H
#define _APIC_H

#define APIC_SIZE	4096

#define PIC_MODE	0
#define APIC_MODE	1

#define APIC_ID		0x20
#define APIC_VERSION	0x30
#define APIC_EOI	0xb0
#define APIC_SVR	0xf0
#define APIC_ICR_LOW	0x300
#define APIC_ICR_HI	0x310
#define APIC_LVT0	0x320
#define APIC_LVT1	0x350
#define APIC_LVT2	0x360
#define APIC_LVT3	0x370
#define APIC_LVT4	0x340
#define APIC_TMICR	0x380
#define APIC_TMCCR	0x390
#define APIC_TMDCR	0x3e0

#define APIC_ENABLED	0x100

#define SET_ICR_DEST_FIELD(d) ((d) << 24)
#define ICR_DS_FIELD		0x00000
#define ICR_DS_SELF		0x40000
#define ICR_DS_ALL		0x80000
#define ICR_DS_ALL_OTHER	0xc0000
#define ICR_DM_FIXED		0x00000
#define ICR_DM_LOWEST		0x00100
#define ICR_DM_SMI		0x00200
#define ICR_DM_NMI		0x00400
#define ICR_DM_INIT		0x00500
#define ICR_DM_START_UP		0x00600
#define ICR_LOGICAL		0x00800
#define ICR_BUSY		0x01000
#define ICR_LV_ASSERT		0x04000
#define ICR_TM_LEVEL		0x08000

#define LVT0_BUSY	0x1000
#define LVT0_MASKED	0x10000
#define LVT0_PERIODIC	0x20000

#define TMDCR_DIV_2	0x0
#define TMDCR_DIV_4	0x1
#define TMDCR_DIV_8	0x2
#define TMDCR_DIV_16	0x3
#define TMDCR_DIV_32	0x8
#define TMDCR_DIV_64	0x9
#define TMDCR_DIV_128	0xa
#define TMDCR_DIV_1	0xb

#ifndef ASM

extern void *v_apic_ptr;
extern void *v_apic_eoi_ptr;

/*
 *	apic_write() schreibt einen Wert in das
 *	angegebene APIC-Register...
 */

static inline void apic_write(unsigned int reg, uint32_t value)
{
	*((volatile uint32_t *) (((uintptr_t) v_apic_ptr) + reg)) = value;
	return;
}

/*
 *	apic_read() liest einen Wert aus dem
 *	angegebenen APIC-Register...
 */

static inline uint32_t apic_read(unsigned int reg)
{
	return *((volatile uint32_t *) (((uintptr_t) v_apic_ptr) + reg));
}

/*
 *	get_apic_version() gibt die Version des APIC's zurueck...
 */

static inline unsigned int get_apic_version()
{
	return apic_read(APIC_VERSION) & 0xff;
}

/*
 *	get_apic_version() gibt die Version des APIC's zurueck...
 */

static inline unsigned int get_apic_maxlvt()
{
	if(get_apic_version() >= 0x10)
	{
		return (apic_read(APIC_VERSION) >> 16) & 0xff;
	}

	return 2;
}

/*
 *	get_apic_id() gibt die ID des APIC's zurueck...
 */

static inline unsigned int get_apic_id()
{
	return (apic_read(APIC_ID) >> 24) & 0xf;
}

void do_apic_error();
void do_spurious_int();
void apic_eoi();
void ap_apic_init();
void apic_init(void *apic, bool imcr);

#endif

#endif

