
/*
string.h @ Xorix Operating System
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

#ifndef _STRING_H
#define _STRING_H

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef __size_t_defined
#define __size_t_defined 1
typedef unsigned int size_t;
#endif

static inline int strcmp(const char *s1, const char *s2)
{
	while(*s1 == *s2 && *s1 != 0)
	{
		s1++;
		s2++;
	}
	
	if(*s1 < *s2) return -1;
	else if (*s1 > *s2) return 1;
	
	return 0;
}

static inline size_t strlen(const char *s)
{
	const char *t = s;
	
	while(*s != '\0')
	{
		s++;
	}
	
	return s - t;
}

static inline char *strcat(char *s1, const char *s2)
{
	char *t = s1;
	
	while(*s1 != '\0')
	{
		s1++;
	}
	
	while(*s2 != '\0')
	{
		*s1 = *s2;
		s1++;
		s2++;
	}
	
	*s1 = '\0';
	
	return t;
}

static inline char *strcpy(char *s1, const char *s2)
{
	char *t = s1;

	while(*s2 != '\0')
	{
		*s1 = *s2;
		s1++;
		s2++;
	}
	
	*s1 = '\0';
	
	return t;
}

static inline char *strchr(const char *s, int c)
{
	while(*s != '\0')
	{
		if(*s == c) return (char *) s;
		s++;
	}
	
	if(c == '\0') return (char *) s;
	
	return NULL;
}

static inline char *strrchr(const char *s, int c)
{
	char *p = NULL;
	
	while(*s != '\0')
	{
		if(*s == c) p = (char *) s;
		s++;
	}
	
	if(c == '\0') p = (char *) s;
	
	return p;
}

static inline void *memchr(const void *s, int c, size_t n)
{
	const unsigned char *_s = s;
	
	while(n--)
	{
		if(*_s == c) return (void *) _s;
		_s++;
	}
	
	return NULL;
}

static inline int memcmp(const char *s1, const void *s2, size_t n)
{
	const unsigned char *_s1 = s1;
	const unsigned char *_s2 = s2;
	
	while(n--)
	{
		if(*_s1 != *_s2)
		{
			if(*_s1 < *_s2) return -1;
			else return 1;
		}
		
		_s1++;
		_s2++;
	}
	
	return 0;
}

static inline void *memcpy(void *s1, const void *s2, size_t n)
{
	char *d = s1;
	const char *s = s2;
	
	while(n--)
	{
		*d++ = *s++;
	}
	
	return s1;
}

static inline void *memmove(void *s1, const void *s2, size_t n)
{
	char *d;
	const char *s;
	
	if(s1 < s2)
	{
		d = s1;
		s = s2;
		
		while(n--)
		{
			*d++ = *s++;
		}
	}
	else
	{
		d = s1 + n;
		s = s2 + n;
		
		while(n--)
		{
			*--d = *--s;
		}
	}
	
	return s1;
}

static inline void *memset(void *s, int c, size_t n)
{
	unsigned char *d = s;
	
	while(n--)
	{
		*d++ = c;
	}
	
	return s;
}

#endif





