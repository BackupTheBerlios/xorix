
/*
kernel/page.c @ Xorix Operating System
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

#include "cpu.h"
#include "memory.h"
#include "page.h"

bool use_pae;
bool use_pge;
bool use_pse;

uint32_t kernel_cr3;

void pageing_init()
{
	cpu_info_t *c;

	// Features...

	c = get_bsp_cpu_info();

	if(c->feature_flags & IDF_PAE) use_pae = true;
	else use_pae = false;

	if(c->feature_flags & IDF_PGE) use_pge = true;
	else use_pge = false;

	if(c->feature_flags & IDF_PSE) use_pse = true;
	else use_pse = false;

	// Physical Address Extension (PAE) ?

	if(use_pae == true)
	{
		pae_pageing_init();
		return;
	}

	// Normales Pageing !

	pg_pageing_init();

	return;
}
