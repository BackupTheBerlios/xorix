
/*
stdarg.h @ Xorix Operating System
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

#ifndef _STDARG_H
#define _STDARG_H

#ifndef __va_list_defined
#define __va_list_defined 1
typedef void *va_list;
#endif

#define va_start(AP, LASTPAR) \
	(AP = ((void *) &(LASTPAR) + (((sizeof(LASTPAR) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))))

#define va_end(AP)

#define va_arg(AP, TY) \
	(AP += (((sizeof(TY) + sizeof(int) - 1) / sizeof(int)) * sizeof(int)), *((TY *) (AP - (((sizeof(TY) + sizeof(int) - 1) / sizeof(int)) * sizeof(int)))))

#endif

