
/*
stdlib.h @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

#ifndef _STDLIB_H
#define _STDLIB_H

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef __size_t_defined
#define __size_t_defined 1
typedef unsigned int size_t;
#endif

#ifndef __wchar_t_defined
#define __wchar_t_defined 1
typedef unsigned long wchar_t;
#endif

#define MB_CUR_MAX 6

size_t wcstombs(char *__restrict s, const wchar_t *__restrict wcs, size_t n);

int mbtowc(wchar_t *__restrict pwc, const char *__restrict s, size_t n);
size_t mbstowcs(wchar_t *__restrict wcs, const char *__restrict s, size_t n);

#endif



