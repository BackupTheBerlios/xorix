
/*
kernel/smpboot.h @ Xorix Operating System
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

#ifndef _SMPBOOT_H
#define _SMPBOOT_H

#define SMPBOOT_PAGE	0x7
#define SMPBOOT_OFFSET	0x7000

#ifndef ASM
extern void smpboot_start;
extern void smpboot_end;
extern void *stack_for_ap;
#endif

#endif
