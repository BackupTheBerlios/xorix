
/*
kernel/apic.c @ Xorix Operating System
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

#define APIC_DEBUG

#include <xorix.h>

#include "debug.h"
#include "cpu.h"
#include "page.h"
#include "idt.h"
#include "entry.h"
#include "apic.h"

static void *apic_ptr;
void *v_apic_ptr;
void *v_apic_eoi_ptr;

/*
 *	do_spurious_int() behandelt den APIC-Error...
 */

void do_apic_error()
{
	kernel_panic("APIC-Error");
	return;
}

/*
 *	do_spurious_int() behandelt den Spurious-Interrupt...
 */

void do_spurious_int()
{
	kernel_panic("Spurious-Interrupt");
	return;
}

/*
 *	apic_eoi() sendet ein END-OF-INTERRUPT
 *	an den APIC...
 */

void apic_eoi()
{
	apic_write(APIC_EOI, 0);
	return;
}

/*
 *	setup_apic_timer() kalibriert und initialisiert
 *	den APIC-Timer...
 */

static void setup_apic_timer()
{
	uint32_t atm1, atm2;
	uint32_t count;
	uint32_t t;

	apic_write(APIC_TMDCR, TMDCR_DIV_16);
	apic_write(APIC_LVT0, LVT0_MASKED | V_APIC_TIMER);

	apic_write(APIC_TMICR, 0xFFFFFFFF);
	t = jiffies();
	while(t == jiffies());
	atm1 = apic_read(APIC_TMCCR);
	t = jiffies();
	while(jiffies() - t < 10);
	atm2 = apic_read(APIC_TMCCR);

	count = (atm1 - atm2) / 10;

	apic_write(APIC_LVT0, LVT0_PERIODIC | V_APIC_TIMER);
	apic_write(APIC_TMICR, count);

	return;
}

/*
 *	set_imcr() schreibt auf das IMCR...
 */

static void set_imcr(uint8_t mode)
{
	asm volatile("mov $0x70,%%al   \n"
	             "outb %%al,$0x22  \n"
	             "mov %%bl,%%al    \n"
	             "outb %%al,$0x23  \n"
	             ::"b" (mode):"eax");
}

/*
 *	enable_apic() aktiviert den APIC...
 */

static void enable_apic()
{
	uint32_t svr, lvt3;

	// APIC aktivieren...

	svr = apic_read(APIC_SVR);
	svr &= 0xFFFFFF0F;
	svr |= APIC_ENABLED | V_SPURIOUS_INT;
	apic_write(APIC_SVR, svr);

	// APIC-Error Interrupt...

	if(get_apic_maxlvt() >= 3)
	{
		lvt3 = apic_read(APIC_LVT3);
		lvt3 &= 0xFFFFFF00;
		lvt3 |= V_APIC_ERROR;
		apic_write(APIC_LVT3, lvt3);
	}

	return;
}

/*
 *	ap_apic_init() initialiesiert den APIC beim AP...
 */

void ap_apic_init()
{
	// APIC aktivieren...

	enable_apic();

	// APIC-Timer initialisieren...

	setup_apic_timer();

	return;
}

/*
 *	apic_init() initialiesiert den APIC...
 */

void apic_init(void *apic, bool imcr)
{
	uint32_t lvt1, lvt2;

	// remap APIC...

	apic_ptr = apic;
	v_apic_ptr = ioremap(apic_ptr, APIC_SIZE);
	v_apic_eoi_ptr = (void *) (((uintptr_t) v_apic_ptr) + APIC_EOI);

	// IMCR auf APIC-Mode setzten...

	if(imcr == true) set_imcr(APIC_MODE);

	// APIC aktivieren...

	enable_apic();

	// Virtual Wire Mode...

	lvt1 = apic_read(APIC_LVT1);
	lvt1 &= 0xFFFE00FF;
	lvt1 |= 0x00005700;
	apic_write(APIC_LVT1, lvt1);

	lvt2 = apic_read(APIC_LVT2);
	lvt2 &= 0xFFFE00FF;
	lvt2 |= 0x00005400;
	apic_write(APIC_LVT2, lvt2);

	return;
}

