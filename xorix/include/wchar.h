
/*
wchar.h @ Xorix Operating System
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

#ifndef _WCHAR_H
#define _WCHAR_H

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX 0xFFFFFFFF
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN 0x00000000
#endif

#ifndef WEOF
#define WEOF 0xFFFFFFFF
#endif

#ifndef __size_t_defined
#define __size_t_defined 1
typedef unsigned int size_t;
#endif

#ifndef __wchar_t_defined
#define __wchar_t_defined 1
typedef unsigned long wchar_t;
#endif

#ifndef __wint_t_defined
#define __wint_t_defined 1
typedef unsigned long wint_t;
#endif

#ifndef __va_list_defined
#define __va_list_defined 1
typedef void *va_list;
#endif

wint_t btowc(int c);
int wctob(wint_t c);

wchar_t *wcscat(wchar_t *__restrict s1, const wchar_t *__restrict s2);
int wcscmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *wcscpy(wchar_t *__restrict s1, const wchar_t *__restrict s2);
size_t wcslen(const wchar_t *s);
wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n);
int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n);
wchar_t *wmemcpy(wchar_t *__restrict s1, const wchar_t *__restrict s2, size_t n);
wchar_t *wmemmove(wchar_t *s1, const wchar_t *s2, size_t n);
wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n);

int vswprintf(wchar_t *s, size_t n, const wchar_t *format, va_list arg);
int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...);

#endif

