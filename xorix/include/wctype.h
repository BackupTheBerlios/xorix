
/*
wctype.h @ Xorix Operating System
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

#ifndef _WCTYPE_H
#define _WCTYPE_H

#ifndef WEOF
#define WEOF 0xFFFFFFFF
#endif

#ifndef __wint_t_defined
#define __wint_t_defined 1
typedef unsigned long wint_t;
#endif

typedef unsigned int wctype_t;
typedef unsigned int wctrans_t;

wctype_t wctype(const char *property);
wctrans_t wctrans(const char *property);

static inline int iswalnum(wint_t c)
{
	return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9');
}

static inline int iswalpha(wint_t c)
{
	return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}

static inline int iswblank(wint_t c)
{
	return c == L'\t' || c == L' ';
}

static inline int iswcntrl(wint_t c)
{
	return c >= L'\a' && c <= L'\r';
}

static inline int iswdigit(wint_t c)
{
	return c >= L'0' && c <= L'9';
}

static inline int iswxdigit(wint_t c)
{
	return (c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'F') || (c >= L'a' && c <= L'f');
}

static inline int iswpunct(wint_t c)
{
	return (c >= L'!' && c <= L'#') || (c >= L'%' && c <= L'/') || (c >= L':' && c <= L'?') || (c >= L'[' && c <= L'_') || (c >= L'{' && c <= L'~');
}

static inline int iswspace(wint_t c)
{
	return c == L' ' || c == L'\t' || c == L'\n' || c == L'\r' || c == L'\v' || c == L'\f';
}

static inline int iswgraph(wint_t c)
{
	return iswalnum(c) || iswpunct(c);
}

static inline int iswprint(wint_t c)
{
	return c == L' ' || iswgraph(c);
}

static inline int iswlower(wint_t c)
{
	return c >= L'a' && c <= L'z';
}

static inline int iswupper(wint_t c)
{
	return c >= L'A' && c <= L'Z';
}

static inline wint_t towlower(wint_t c)
{
	if(iswupper(c)) return c + 32;
	return c;
}

static inline wint_t towupper(wint_t c)
{
	if(iswlower(c)) return c - 32;
	return c;
}

static inline int iswctype(wint_t c, wctype_t category)
{
	switch(category)
	{
	case 1: return iswalnum(c);
	case 2: return iswalpha(c);
	case 3: return iswdigit(c);
	case 4: return iswxdigit(c);
	case 5: return iswprint(c);
	case 6: return iswspace(c);
	case 7: return iswlower(c);
	case 8: return iswupper(c);
	case 9: return iswgraph(c);
	case 10: return iswpunct(c);
	case 11: return iswblank(c);
	case 12: return iswcntrl(c);
	}
	
	return 0;
}

static inline wint_t towctrans(wint_t c, wctrans_t category)
{
	switch(category)
	{
	case 1: return towlower(c);
	case 2: return towupper(c);
	}
	
	return c;
}

#endif






