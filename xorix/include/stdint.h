
/*
stdint.h @ Xorix Operating System
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

#ifndef _STDINT_H
#define _STDINT_H

#define INT8_MAX		0x7F
#define INT_LEAST8_MAX		0x7F
#define INT_FAST8_MAX		0x7F

#define INT8_MIN		(-0x7F - 1)
#define INT_LEAST8_MIN		(-0x7F - 1)
#define INT_FAST8_MIN		(-0x7F - 1)

#define UINT8_MAX		0xFF
#define UINT_LEAST8_MAX		0xFF
#define UINT_FAST8_MAX		0xFF

#define INT16_MAX		0x7FFF
#define INT_LEAST16_MAX		0x7FFF
#define INT_FAST16_MAX		0x7FFFFFFF

#define INT16_MIN		(-0x7FFF - 1)
#define INT_LEAST16_MIN		(-0x7FFF - 1)
#define INT_FAST16_MIN		(-0x7FFFFFFF - 1)

#define UINT16_MAX		0xFFFF
#define UINT_LEAST16_MAX	0xFFFF
#define UINT_FAST16_MAX		0xFFFFFFFF

#define INT32_MAX		0x7FFFFFFF
#define INT_LEAST32_MAX		0x7FFFFFFF
#define INT_FAST32_MAX		0x7FFFFFFF

#define INT32_MIN		(-0x7FFFFFFF - 1)
#define INT_LEAST32_MIN		(-0x7FFFFFFF - 1)
#define INT_FAST32_MIN		(-0x7FFFFFFF - 1)

#define UINT32_MAX		0xFFFFFFFF
#define UINT_LEAST32_MAX	0xFFFFFFFF
#define UINT_FAST32_MAX		0xFFFFFFFF

#define INT64_MAX		0x7FFFFFFFFFFFFFFF
#define INT_LEAST64_MAX		0x7FFFFFFFFFFFFFFF
#define INT_FAST64_MAX		0x7FFFFFFFFFFFFFFF

#define INT64_MIN		(-0x7FFFFFFFFFFFFFFF - 1)
#define INT_LEAST64_MIN		(-0x7FFFFFFFFFFFFFFF - 1)
#define INT_FAST64_MIN		(-0x7FFFFFFFFFFFFFFF - 1)

#define UINT64_MAX		0xFFFFFFFFFFFFFFFF
#define UINT_LEAST64_MAX	0xFFFFFFFFFFFFFFFF
#define UINT_FAST64_MAX		0xFFFFFFFFFFFFFFFF

#define INTMAX_MAX		0x7FFFFFFFFFFFFFFF
#define INTMAX_MIN		(-0x7FFFFFFFFFFFFFFF - 1)
#define UINTMAX_MAX		0xFFFFFFFFFFFFFFFF

#define INTPTR_MAX		0x7FFFFFFF
#define INTPTR_MIN		(-0x7FFFFFFF - 1)
#define UINTPTR_MAX		0xFFFFFFFF

#define PTRDIFF_MAX		0x7FFFFFFF
#define PTRDIFF_MIN		(-0x7FFFFFFF - 1)

#ifndef WCHAR_MAX
#define WCHAR_MAX		0xFFFFFFFF
#endif

#ifndef WCHAR_MIN
#define WCHAR_MIN		0x00000000
#endif

#define WINT_MAX		0xFFFFFFFF
#define WINT_MIN		0x00000000

#define SIZE_MAX		0xFFFFFFFF

#define SIG_ATOMIC_MAX		0x7FFFFFFF
#define SIG_ATOMIC_MIN		(-0x7FFFFFFF - 1)

typedef signed char int8_t;
typedef signed char int_least8_t;
typedef signed char int_fast8_t;

typedef unsigned char uint8_t;
typedef unsigned char uint_least8_t;
typedef unsigned char uint_fast8_t;

typedef short int16_t;
typedef short int_least16_t;
typedef int int_fast16_t;

typedef unsigned short uint16_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_fast16_t;

typedef long int32_t;
typedef long int_least32_t;
typedef long int_fast32_t;

typedef unsigned long uint32_t;
typedef unsigned long uint_least32_t;
typedef unsigned long uint_fast32_t;

typedef long long int64_t;
typedef long long int_least64_t;
typedef long long int_fast64_t;

typedef unsigned long long uint64_t;
typedef unsigned long long uint_least64_t;
typedef unsigned long long uint_fast64_t;

typedef long long intmax_t;
typedef unsigned long long uintmax_t;

typedef int intptr_t;
typedef unsigned int uintptr_t;

#endif


