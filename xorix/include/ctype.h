
/*
ctype.h @ Xorix Operating System
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

#ifndef _CTYPE_H
#define _CTYPE_H

static inline int isalnum(int c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

static inline int isalpha(int c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int isblank(int c)
{
	return c == '\t' || c == ' ';
}

static inline int iscntrl(int c)
{
	return c >= '\a' && c <= '\r';
}

static inline int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

static inline int isxdigit(int c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

static inline int ispunct(int c)
{
	return (c >= '!' && c <= '#') || (c >= '%' && c <= '/') || (c >= ':' && c <= '?') || (c >= '[' && c <= '_') || (c >= '{' && c <= '~');
}

static inline int isspace(int c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static inline int isgraph(int c)
{
	return isalnum(c) || ispunct(c);
}

static inline int isprint(int c)
{
	return c == ' ' || isgraph(c);
}

static inline int islower(int c)
{
	return c >= 'a' && c <= 'z';
}

static inline int isupper(int c)
{
	return c >= 'A' && c <= 'Z';
}

static inline int tolower(int c)
{
	if(isupper(c)) return c + 32;
	return c;
}

static inline int toupper(int c)
{
	if(islower(c)) return c - 32;
	return c;
}

#endif

