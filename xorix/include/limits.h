
/*
limits.h @ Xorix Operating System
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

#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT	8
#define CHAR_MAX	0x7F
#define CHAR_MIN	(-0x7F - 1)

#define SCHAR_MAX	0x7F
#define SCHAR_MIN	(-0x7F - 1)

#define UCHAR_MAX	0xFF

#define SHRT_MAX	0x7FFF
#define SHRT_MIN	(-0x7FFF - 1)
#define USHRT_MAX	0xFFFF

#define INT_MAX		0x7FFFFFFF
#define INT_MIN		
#define UINT_MAX	0xFFFFFFFF

#define LONG_MAX	0x7FFFFFFF
#define LONG_MIN	(-0x7FFFFFFF - 1)
#define ULONG_MAX	0xFFFFFFFF

#define LLONG_MAX	0x7FFFFFFFFFFFFFFF
#define LLONG_MIN	(-0x7FFFFFFFFFFFFFFF - 1)
#define ULLONG_MAX	0xFFFFFFFFFFFFFFFF

#define MB_LEN_MAX	6

#endif

