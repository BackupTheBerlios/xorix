
/*
kernel/console.c @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>
Copyright (C) 2001 Gregor Weckbecker <gregW@gmx.de>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <xorix.h>
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>

#include "cpu.h"
#include "console.h"

static volatile char *video_ram = (char *) 0xB8000;
static int cursor_pos = -1;

/*
 *	get_cursor() liest die Cursor-Position aus.
 */

static inline int get_cursor()
{
	int cursor_pos;
	
	OUTB(0x0E, 0x3D4);
	cursor_pos = INB(0x3D5) << 8;
	OUTB(0x0F, 0x3D4);
	cursor_pos = (cursor_pos + INB(0x3D5)) << 1;

	return cursor_pos;
}

/*
 *	set_cursor() setzt die Cursor-Position.
 */

static inline void set_cursor(int cursor_pos)
{
	cursor_pos >>= 1;
	OUTB(0x0F, 0x3D4);
	OUTB(cursor_pos & 0xFF, 0x3D5);
	OUTW(0x0E, 0x3D4);
	OUTB(cursor_pos >> 8, 0x3D5);

	return;
}

/*
 *	write_wc() gibt ein einzelnes Zeichen auf den Bildschirm
 *	aus.
 */

static inline int write_wc(int cursor_pos, wchar_t wc)
{
	unsigned int i;

	// Zeichen ins Video RAM schreiben...

	switch(wc)
	{
	case L'\n':
		cursor_pos = cursor_pos / 160 * 160 + 160;
		break;
	case L'\r':
		break;
	case L'\t':
		cursor_pos = cursor_pos / 8 * 8 + 8;
		break;
	default:
		if((video_ram[cursor_pos] = wctob(wc)) == EOF)
		{
			video_ram[cursor_pos] = '?';
		}
		cursor_pos += 2;
	}

	// Bildschirm voll?

	if(cursor_pos >= 4000)
	{
		cursor_pos = 4000 - 160;

		for(i = 0; i < 4000 - 160; i++)
		{
			video_ram[i] = video_ram[i+160];
		}

		for(i = i; i < 4000; i += 2)
		{
			video_ram[i] = ' ';
			video_ram[i+1] = 0x07;
		}
	}

	return cursor_pos;
}

void console_wprint(const wchar_t *s)
{
	const wchar_t *p;
	va_list arg;
	int r;

	if(cursor_pos == -1)
	{cursor_pos = get_cursor();}

	for(p = s; *p; p++)
	{
		cursor_pos = write_wc(cursor_pos, *p);
	}

	set_cursor(cursor_pos);

	return;
}

void panic_clear()
{
	unsigned int i;

	for(i = 0; i < 4000; i += 2)
	{
		video_ram[i] = ' ';
		video_ram[i + 1] = 0x07;
	}

	set_cursor(0);

	return;
}

unsigned int panic_new_line()
{
	unsigned int pos;
	unsigned int i;

	for(i = 0; i < 4000 - 160; i++)
	{
		video_ram[i] = video_ram[i+160];
	}

	for(i = i; i < 4000; i += 2)
	{
		video_ram[i] = ' ';
		video_ram[i+1] = 0x07;
	}

	pos = 4000 - 160;
	set_cursor(pos);

	return pos;
}

unsigned int panic_print(unsigned int pos, const char *s)
{
	while(*s != '\0')
	{
		if(pos + 1 < 4000)
		{
			video_ram[pos] = *s;
			video_ram[pos + 1] = 0x07;
			pos += 2;
		}

		s++;
	}

	set_cursor(pos);

	return pos;
}
