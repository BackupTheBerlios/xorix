
/*
kernel/apm.h @ Xorix Operating System
Copyright (C) 2001 Ingmar Friedrichsen <ingmar@xorix.org>

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

#ifndef _APM_H
#define _APM_H

#define APM_STATE_APM_ENABLED	0x0000
#define APM_STATE_STANDBY	0x0001
#define APM_STATE_SUSPEND	0x0002
#define APM_STATE_OFF		0x0003

#define APM_DEVICE_ALL		0x0001
#define APM_DEVICE_DISPLAY(d)	(0x0100 | ((d) & 0xff))
#define APM_DEVICE_DISPLAY_ALL	0x01ff
#define APM_DEVICE_STORAGE(d)	(0x0200 | ((d) & 0xff))
#define APM_DEVICE_STORAGE_ALL	0x02ff
#define APM_DEVICE_PARALLEL(d)	(0x0300 | ((d) & 0xff))
#define APM_DEVICE_PARALLEL_ALL	0x03ff
#define APM_DEVICE_SERIAL(d)	(0x0400 | ((d) & 0xff))
#define APM_DEVICE_SERIAL_ALL	0x04ff
#define APM_DEVICE_NETWORK(d)	(0x0500 | ((d) & 0xff))
#define APM_DEVICE_NETWORK_ALL	0x05ff
#define APM_DEVICE_PCMCIA(d)	(0x0600 | ((d) & 0xff))
#define APM_DEVICE_PCMCIA_ALL	0x06ff

#ifndef ASM
void apm_init();
#endif

#endif
