
/*
stdlib.c @ Xorix Operating System
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

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

static inline int _wctomb_len(wchar_t wchar)
{
	if(wchar < 0) return -1;
	else if(wchar < 0x80) return 1;
	else if(wchar < 0x800) return 2;
	else if(wchar < 0x10000) return 3;
	else if(wchar < 0x200000) return 4;
	else if(wchar < 0x4000000) return 5;
	else if(wchar <= 0x7FFFFFFF) return 6;
	
	return -1;
}

int wctomb(char *s, wchar_t wchar)
{
	if(s == NULL) return 0;
	
	if(wchar < 0)
	{
		return -1;
	}
	else if(wchar < 0x80)
	{
		s[0] = wchar;
		return 1;
	}
	else if(wchar < 0x800)
	{
		s[0] = 0xC0 | wchar >> 6;
		s[1] = 0x80 | wchar & 0x3F;
		return 2;
	}
	else if(wchar < 0x10000)
	{
		s[0] = 0xE0 | wchar >> 12;
		s[1] = 0x80 | wchar >> 6 & 0x3F;
		s[2] = 0x80 | wchar & 0x3F;
		return 3;
	}
	else if(wchar < 0x200000)
	{
		s[0] = 0xF0 | wchar >> 18;
		s[1] = 0x80 | wchar >> 12 & 0x3F;
		s[2] = 0x80 | wchar >> 6 & 0x3F;
		s[3] = 0x80 | wchar & 0x3F;
		return 4;
	}
	else if(wchar < 0x4000000)
	{
		s[0] = 0xF8 | wchar >> 24;
		s[1] = 0x80 | wchar >> 18 & 0x3F;
		s[2] = 0x80 | wchar >> 12 & 0x3F;
		s[3] = 0x80 | wchar >> 6 & 0x3F;
		s[4] = 0x80 | wchar & 0x3F;
		return 5;
	}
	else if(wchar <= 0x7FFFFFFF)
	{
		s[0] = 0xFC | wchar >> 30;
		s[1] = 0x80 | wchar >> 24 & 0x3F;
		s[2] = 0x80 | wchar >> 18 & 0x3F;
		s[3] = 0x80 | wchar >> 12 & 0x3F;
		s[4] = 0x80 | wchar >> 6 & 0x3F;
		s[5] = 0x80 | wchar & 0x3F;
		return 6;
	}
	else
	{
		return -1;
	}
	
	return -1;
}

size_t wcstombs(char *__restrict s, const wchar_t *__restrict wcs, size_t n)
{
	char *old_s;
	int x, z;
	
	if(s == NULL)
	{
		z = 0;
		
		while(*wcs != L'\0')
		{
			x = _wctomb_len(*wcs++);
			if(x == -1) goto eilseq;
			z += x;
		}
		
		return z;
	}
	
	while(n >= MB_CUR_MAX)
	{
		if(*wcs == L'\0')
		{
			*s = '\0';
			return s - old_s;
		}
		
		x = wctomb(s, *wcs);
		if(x == -1) goto eilseq;
		
		n -= x;
		s += x;
		wcs++;
	}
	
	z = _wctomb_len(*wcs);
	if(z == -1) goto eilseq;
	
	while(n >= z)
	{
		if(*wcs == L'\0')
		{
			*s = '\0';
			return s - old_s;
		}
		
		x = wctomb(s, *wcs);
		if(x == -1) goto eilseq;
		
		n -= x;
		s += x;
		wcs++;
		
		z = _wctomb_len(*wcs);
		if(z == -1) goto eilseq;
	}
	
	return s - old_s;
	
	eilseq:
	errno = EILSEQ;
	return (size_t) -1;
}

int mblen(const char *s, size_t n)
{
	const unsigned char *us = s;
	
	if(us == NULL) return 0;
	
	if(n < 1) goto eilseq;
	
	if((us[0] & 0x80) == 0)
	{
		if(us[0] == L'\0') return 0;
		return 1;
	}
	else if((us[0] & 0xE0) == 0xC0)
	{
		if(n < 2) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		return 2;
	}
	else if((us[0] & 0xF0) == 0xE0)
	{
		if(n < 3) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		return 3;
	}
	else if((us[0] & 0xF8) == 0xF0)
	{
		if(n < 4) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		return 4;
	}
	else if((us[0] & 0xFC) == 0xF8)
	{
		if(n < 5) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		if((us[4] & 0xC0) != 0x80) goto eilseq;
		return 5;
	}
	else if((us[0] & 0xFE) == 0xFC)
	{
		if(n < 6) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		if((us[4] & 0xC0) != 0x80) goto eilseq;
		if((us[5] & 0xC0) != 0x80) goto eilseq;
		return 6;
	}
	
	eilseq:
	errno = EILSEQ;
	return -1;
}

int mbtowc(wchar_t *__restrict pwc, const char *__restrict s, size_t n)
{
	const unsigned char *us = s;
	
	if(s == NULL) return 0;
	
	if(n < 1) goto eilseq;
	
	if((us[0] & 0x80) == 0)
	{
		*pwc = *s;
		
		if(*pwc == L'\0') return 0;
		return 1;
	}
	else if((us[0] & 0xE0) == 0xC0)
	{
		if(n < 2) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		
		*pwc = ((us[0] & 0x1F) << 6) |
		       ((us[1] & 0x3F));
		
		return 2;
	}
	else if((us[0] & 0xF0) == 0xE0)
	{
		if(n < 3) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		
		*pwc = ((us[0] & 0x1F) << 12) |
		       ((us[1] & 0x3F) << 6) |
		       ((us[2] & 0x3F));
		
		return 3;
	}
	else if((us[0] & 0xF8) == 0xF0)
	{
		if(n < 4) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		
		*pwc = ((us[0] & 0x1F) << 18) |
		       ((us[1] & 0x3F) << 12) |
		       ((us[2] & 0x3F) << 6) |
		       ((us[3] & 0x3F));
		
		return 4;
	}
	else if((us[0] & 0xFC) == 0xF8)
	{
		if(n < 5) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		if((us[4] & 0xC0) != 0x80) goto eilseq;
		
		*pwc = ((us[0] & 0x1F) << 24) |
		       ((us[1] & 0x3F) << 18) |
		       ((us[2] & 0x3F) << 12) |
		       ((us[3] & 0x3F) << 6) |
		       ((us[4] & 0x3F));
		
		return 5;
	}
	else if((us[0] & 0xFE) == 0xFC)
	{
		if(n < 6) goto eilseq;
		if((us[1] & 0xC0) != 0x80) goto eilseq;
		if((us[2] & 0xC0) != 0x80) goto eilseq;
		if((us[3] & 0xC0) != 0x80) goto eilseq;
		if((us[4] & 0xC0) != 0x80) goto eilseq;
		if((us[5] & 0xC0) != 0x80) goto eilseq;
		
		*pwc = ((us[0] & 0x1F) << 30) |
		       ((us[1] & 0x3F) << 24) |
		       ((us[2] & 0x3F) << 18) |
		       ((us[3] & 0x3F) << 12) |
		       ((us[4] & 0x3F) << 6) |
		       ((us[5] & 0x3F));
		
		return 6;
	}
	else goto eilseq;
	
	eilseq:
	errno = EILSEQ;
	return -1;
}

size_t mbstowcs(wchar_t *__restrict wcs, const char *__restrict s, size_t n)
{
	wchar_t *old_wcs;
	int x;
	
	old_wcs = wcs;
	
	if(wcs != NULL)
	{
		while(n--);
		{
			x = mbtowc(wcs, s, MB_CUR_MAX);
			if(x <= 0)
			{
				if(x == 0) return wcs - old_wcs;
				else goto eilseq;
			}
			wcs++;
			s += x;
		}
		
		return wcs - old_wcs;
	}
	else
	{
		while(1);
		{
			x = mblen(s, MB_CUR_MAX);
			if(x <= 0)
			{
				if(x == 0) return wcs - old_wcs;
				else goto eilseq;
			}
			wcs++;
			s += x;
		}
	}
	
	eilseq:
	errno = EILSEQ;
	return (size_t) -1;
}

