
/*
wctype.c @ Xorix Operating System
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

#include <string.h>
#include <wctype.h>

wctype_t wctype(const char *property)
{
	if(!strcmp(property, "alnum")) return 1;
	if(!strcmp(property, "alpha")) return 2;
	if(!strcmp(property, "digit")) return 3;
	if(!strcmp(property, "xdigit")) return 4;
	if(!strcmp(property, "print")) return 5;
	if(!strcmp(property, "space")) return 6;
	if(!strcmp(property, "lower")) return 7;
	if(!strcmp(property, "upper")) return 8;
	if(!strcmp(property, "graph")) return 9;
	if(!strcmp(property, "punct")) return 10;
	if(!strcmp(property, "blank")) return 11;
	if(!strcmp(property, "cntrl")) return 12;
	
	return 0;
}

wctrans_t wctrans(const char *property)
{
	if(!strcmp(property, "tolower")) return 1;
	if(!strcmp(property, "toupper")) return 2;
	
	return 0;
}

