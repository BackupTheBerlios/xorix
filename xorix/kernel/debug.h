
/*
kernel/debug.h @ Xorix Operating System
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

#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef ASM

#define __BUG(l) kernel_bug(__FILE__, #l)
#define _BUG(l) __BUG(l)
#define BUG() _BUG(__LINE__)

void wprintk(const wchar_t *format, ...);
void kernel_bug(const char *file, const char *line);
void kernel_panic(const char *s);

#endif

#endif


