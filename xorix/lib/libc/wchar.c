
/*
wchar.c @ Xorix Operating System
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

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <wctype.h>
#include <wchar.h>
#include <limits.h>

wchar_t *wcscat(wchar_t *__restrict s1, const wchar_t *__restrict s2)
{
	wchar_t *t = s1;
	
	while(*s1 != L'\0')
	{
		s1++;
	}
	
	while(*s2 != L'\0')
	{
		*s1 = *s2;
		s1++;
		s2++;
	}
	
	*s1 = L'\0';
	
	return t;
};

int wcscmp(const wchar_t *s1, const wchar_t *s2)
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

wchar_t *wcscpy(wchar_t *__restrict s1, const wchar_t *__restrict s2)
{
	wchar_t *t = s1;

	while(*s2 != L'\0')
	{
		*s1 = *s2;
		s1++;
		s2++;
	}
	
	*s1 = L'\0';
	
	return t;
}

size_t wcslen(const wchar_t *s)
{
	const wchar_t *t = s;
	
	while(*s != L'\0')
	{
		s++;
	}
	
	return s - t;
}

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n)
{
	const wchar_t *_s = s;
	
	while(n--)
	{
		if(*_s == c) return (wchar_t *) _s;
		_s++;
	}
	
	return NULL;
}

int wmemcmp(const wchar_t *s1, const wchar_t *s2, size_t n)
{
	const wchar_t *_s1 = s1;
	const wchar_t *_s2 = s2;
	
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

wchar_t *wmemcpy(wchar_t *__restrict s1, const wchar_t *__restrict s2, size_t n)
{
	wchar_t *d = s1;
	const wchar_t *s = s2;
	
	while(n--)
	{
		*d++ = *s++;
	}
	
	return s1;
}

wchar_t *wmemmove(wchar_t *s1, const wchar_t *s2, size_t n)
{
	wchar_t *d;
	const wchar_t *s;
	
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

wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n)
{
	wchar_t *d = s;
	
	while(n--)
	{
		*d++ = c;
	}
	
	return s;
}

wint_t btowc(int c)
{
	if(c == EOF) return WEOF;
	return (unsigned char) c;
}

int wctob(wint_t c)
{
	if(c > UCHAR_MAX) return EOF;
	return c;
}

#define _MINUS	1
#define _PLUS	2
#define _SPACE	4
#define _SHARP	8
#define _ZERO	16
#define _SIGN	32
#define _LARGE	64

#define _H	1
#define _HH	2
#define _L	3
#define _LL	4
#define _J	5
#define _Z	6
#define _T	7

static wchar_t *number(wchar_t *s, wchar_t *e, long long num, int base, int flags, int field_width, int precision)
{
	const wchar_t small_digits[] = L"0123456789abcdefx";
	const wchar_t large_digits[] = L"0123456789ABCDEFX";
	const wchar_t *digits;
	unsigned long long u_num;
	wchar_t tmp[66];
	wchar_t sign;
	int f, p, i, z;
	
	if(flags & _LARGE) digits = large_digits;
	else digits = small_digits;
	
	if(flags & _SIGN)
	{
		if(num < 0)
		{
			u_num = -num;
			sign = '-';
		}
		else
		{
			u_num = num;
			if(flags & _PLUS) sign = '+';
			else if(flags & _SPACE) sign = ' ';
			else sign = 0;
		}
	}
	else
	{
		u_num = num;
		sign = 0;
	}
	
	i = 0;
	
	if(u_num == 0) 
	{
		tmp[i] = '0';
		i++;
	}
	else while(u_num != 0)
	{
		tmp[i] = digits[u_num % base];
		u_num /= base;
		i++;
	}
	
	f = field_width;
	
	if(i >= precision)
	{
		p = i;
		if((flags & _SHARP) && base == 8) p++;
	}
	else p = precision;
	
	z = p;
	
	if(flags & _MINUS)
	{
		if(sign) {if(s <= e) *s++ = sign; z++;}
		else if((flags & _SHARP) && base == 16)
		{
			z += 2;
			if(s <= e) *s++ = L'0';
			if(s <= e) *s++ = digits[16];
		}
	}
	else
	{
		if((flags & _ZERO) && precision < 0)
		{
			if(sign) {f--; if(s <= e) *s++ = sign;}
			else if((flags & _SHARP) && base == 16)
			{
				f -= 2;
				if(s <= e) *s++ = L'0';
				if(s <= e) *s++ = digits[16];
			}
			while(f-- > p) if(s <= e) *s++ = L'0';
		}
		else
		{
			if(sign)
			{
				while(f-- > p + 1) if(s <= e) *s++ = L' ';
				if(s <= e) *s++ = sign;
			}
			else if((flags & _SHARP) && base == 16)
			{
				while(f-- > p + 2) if(s <= e) *s++ = L' ';
				if(s <= e) *s++ = L'0';
				if(s <= e) *s++ = digits[16];
			}
			else
			{
				while(f-- > p) if(s <= e) *s++ = L' ';
			}
		}
	}
	
	while(p-- > i) if(s <= e) *s++ = L'0';
	while(i > 0) {i--; if(s <= e) *s++ = tmp[i];}
	
	if(flags & _MINUS)
	{
		while(z++ < field_width) if(s <= e) *s++ = L' ';
	}
	
	return s;
}

int vswprintf(wchar_t *s, size_t n, const wchar_t *format, va_list arg)
{
	long long num;
	wchar_t *e;
	wchar_t *t;
	int base;
	int flags;
	int field_width;
	int precision;
	int modifier;
	const wchar_t *error_format;
	const wchar_t *wcs, *_wcs;
	const char *str, *_str;
	size_t len;
	bool error;
	
	error = false;
	
	t = s;
	
	e = s + n - 1;
	if(e < s - 1) e = (void *) -1;
	
	while(*format)
	{
		if(*format != L'%')
		{
			if(s <= e) *s++ = *format;
			format++;
			continue;
		}
		
		error_format = format + 1;
		if(error) goto do_error;
		
		flags = 0;
		repeat:
		switch(*++format)
		{
		case L'-':
			flags |= _MINUS;
			goto repeat;
		case L'+':
			flags |= _PLUS;
			goto repeat;
		case L' ':
			flags |= _SPACE;
			goto repeat;
		case L'#':
			flags |= _SHARP;
			goto repeat;
		case L'0':
			flags |= _ZERO;
			goto repeat;
		case L'\'':
			goto repeat;
		case L'\0':
			goto do_error;
		}
		
		if(iswdigit(*format))
		{
			field_width = 0;
			do
			{
				field_width = (field_width * 10) + (*format - L'0');
				format++;
			}
			while(iswdigit(*format));
		}
		else if(*format == L'*')
		{
			field_width = va_arg(arg, int);
			if(field_width < 0)
			{field_width = -field_width; flags |= _MINUS;}
			format++;
		}
		else field_width = 0;
		
		if(*format == L'.')
		{
			format++;
			
			if(iswdigit(*format))
			{
				precision = 0;
				do
				{
					precision = (precision * 10) + (*format - L'0');
					format++;
				}
				while(iswdigit(*format));
			}
			else if(*format == L'*')
			{
				precision = va_arg(arg, int);
				format++;
			}
			else precision = -1;
		}
		else precision = -1;
		
		switch(*format)
		{
		case L'h':
			format++;
			if(*format == L'h') {format++; modifier = _HH;}
			else {modifier = _H;}
			break;
		case L'l':
			format++;
			if(*format == L'l') {format++; modifier = _LL;}
			else modifier = _L;
			break;
		case L'j':
			format++;
			modifier = _J;
			break;
		case L'z':
			format++;
			modifier = _Z;
			break;
		case L't':
			format++;
			modifier = _T;
			break;
		default:
			modifier = 0;
		}
		
		switch(*format)
		{
		case L'%':
			if(modifier) goto do_error;
			if(s <= e) *s++ = L'%';
			break;
		case L's':
			switch(modifier)
			{
			case 0:
				_str = str = va_arg(arg, char *);
				
				if(precision < 0) while(*_str) _str++;
				else while(*_str && precision--) _str++;
				len = _str - str;
				
				if(!(flags & _MINUS))
				{
					while(field_width > len)
					{if(s <= e) *s++ = L' '; field_width--;}
				}
				
				while(str < _str)
				{if(s <= e) *s++ = *str++;}
				
				while(field_width-- > len)
				{if(s <= e) *s++ = L' ';}
				
				break;
			case _L:
				_wcs = wcs = va_arg(arg, wchar_t *);
				
				if(precision < 0) while(*_wcs) _wcs++;
				else while(*_wcs && precision--) _wcs++;
				len = _wcs - wcs;
				
				if(!(flags & _MINUS))
				{
					while(field_width > len)
					{if(s <= e) *s++ = L' '; field_width--;}
				}
				
				while(wcs < _wcs)
				{if(s <= e) *s++ = *wcs++;}
				
				while(field_width-- > len)
				{if(s <= e) *s++ = L' ';}
				
				break;
			default:
				goto do_error;
			}
			break;
		case L'c':
			switch(modifier)
			{
			case 0:
				if(!(flags & _MINUS))
				{
					while(field_width > 1)
					{if(s <= e) *s++ = L' '; field_width--;}
				}
				
				if(s <= e) *s++ = (unsigned char) va_arg(arg, int);
				else va_arg(arg, int);
				
				while(field_width > 1)
				{if(s <= e) *s++ = L' '; field_width--;}
				break;
			case _L:
				if(!(flags & _MINUS))
				{
					while(field_width > 1)
					{if(s <= e) *s++ = L' '; field_width--;}
				}
				
				if(s <= e) *s++ = (wchar_t) va_arg(arg, wint_t);
				else va_arg(arg, wint_t);
				
				while(field_width > 1)
				{if(s <= e) *s++ = L' '; field_width--;}
				break;
			default:
				goto do_error;
			}
			
			break;
		case L'n':
			switch(modifier)
			{
			case 0:   *(va_arg(arg, int *)) = s - t; break;
			case _H:  *(va_arg(arg, short *)) = s - t; break;
			case _HH: *(va_arg(arg, int *)) = s - t; break;
			case _L:  *(va_arg(arg, long *)) = s - t; break;
			case _LL: *(va_arg(arg, long long *)) = s - t; break;
			case _J:  *(va_arg(arg, intmax_t *)) = s - t; break;
			case _Z:  *(va_arg(arg, size_t *)) = s - t; break;
			case _T:  *(va_arg(arg, ptrdiff_t *)) = s - t; break;
			default:  goto do_error;
			}
			break;
		case L'p':
			if(modifier) goto do_error;
			s = number(s, e, (uintptr_t) va_arg(arg, void *), 16, _SHARP, 0, -1);
			break;
		case L'd':
		case L'i':
			flags |= _SIGN;
		case L'u':
			base = 10;
			goto do_number;
		case L'X':
			flags |= _LARGE;
		case L'x':
			base = 16;
			goto do_number;
		case L'o':
			base = 8;
		do_number:
			if(modifier == 0)
			{
				if(flags & _SIGN) num = va_arg(arg, int);
				else num = va_arg(arg, unsigned int);
			}
			else if(modifier == _H)
			{
				if(flags & _SIGN) num = (short) va_arg(arg, int);
				else num = (unsigned short) va_arg(arg, unsigned int);
			}
			else if(modifier == _HH)
			{
				if(flags & _SIGN) num = (signed char) va_arg(arg, int);
				else num = (unsigned char) va_arg(arg, unsigned int);
			}
			else if(modifier == _L)
			{
				if(flags & _SIGN) num = va_arg(arg, long);
				else num = va_arg(arg, unsigned long);
			}
			else if(modifier == _LL)
			{
				if(flags & _SIGN) num = va_arg(arg, long long);
				else num = va_arg(arg, unsigned long long);
			}
			else if(modifier == _J)
			{
				if(flags & _SIGN) num = va_arg(arg, intmax_t);
				else num = va_arg(arg, uintmax_t);
			}
			else if(modifier == _Z)
			{
				if(flags & _SIGN) num = (ptrdiff_t) va_arg(arg, size_t);
				else num = va_arg(arg, size_t);
			}
			else if(modifier == _T)
			{
				if(flags & _SIGN) num = va_arg(arg, ptrdiff_t);
				else num = (size_t) va_arg(arg, ptrdiff_t);
			}
			
			s = number(s, e, num, base, flags, field_width, precision);
			break;
		default:
			goto do_error;
		}
		
		format++;
		continue;
		
		do_error:
		error = true;
		if(s <= e) *s++ = L'%';
		format = error_format;
		continue;
	}
	
	if(s <= e)
	{
		*s = L'\0';
		return s - t;
	}
	
	return -1;
}

int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...)
{
	va_list arg;
	int r;
	
	va_start(arg, format);
	r = vswprintf(s, n, format, arg);
	va_end(arg);
	
	return r;
}

#undef _MINUS
#undef _PLUS
#undef _SPACE
#undef _SHARP
#undef _ZERO
#undef _SIGN
#undef _LARGE

#undef _H
#undef _HH
#undef _L
#undef _LL
#undef _J
#undef _Z
#undef _T



