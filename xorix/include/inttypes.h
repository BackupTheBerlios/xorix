
/*
inttypes.h @ Xorix Operating System
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

#ifndef _INTTYPES_H
#define _INTTYPES_H

#define PRId8		"hhd"
#define PRIi8		"hhi"
#define PRIu8		"hhu"
#define PRIo8		"hho"
#define PRIx8		"hhx"
#define PRIX8		"hhX"

#define PRIdLEAST8	"hhd"
#define PRIiLEAST8	"hhi"
#define PRIuLEAST8	"hhu"
#define PRIoLEAST8	"hho"
#define PRIxLEAST8	"hhx"
#define PRIXLEAST8	"hhX"

#define PRIdFAST8	"hhd"
#define PRIiFAST8	"hhi"
#define PRIuFAST8	"hhu"
#define PRIoFAST8	"hho"
#define PRIxFAST8	"hhx"
#define PRIXFAST8	"hhX"

#define PRId16		"hd"
#define PRIi16		"hi"
#define PRIu16		"hu"
#define PRIo16		"ho"
#define PRIx16		"hx"
#define PRIX16		"hX"

#define PRIdLEAST16	"hd"
#define PRIiLEAST16	"hi"
#define PRIuLEAST16	"hu"
#define PRIoLEAST16	"ho"
#define PRIxLEAST16	"hx"
#define PRIXLEAST16	"hX"

#define PRIdFAST16	"d"
#define PRIiFAST16	"i"
#define PRIuFAST16	"u"
#define PRIoFAST16	"o"
#define PRIxFAST16	"x"
#define PRIXFAST16	"X"

#define PRId32		"ld"
#define PRIi32		"li"
#define PRIu32		"lu"
#define PRIo32		"lo"
#define PRIx32		"lx"
#define PRIX32		"lX"

#define PRIdLEAST32	"ld"
#define PRIiLEAST32	"li"
#define PRIuLEAST32	"lu"
#define PRIoLEAST32	"lo"
#define PRIxLEAST32	"lx"
#define PRIXLEAST32	"lX"

#define PRIdFAST32	"ld"
#define PRIiFAST32	"li"
#define PRIuFAST32	"lu"
#define PRIoFAST32	"lo"
#define PRIxFAST32	"lx"
#define PRIXFAST32	"lX"

#define PRId64		"lld"
#define PRIi64		"lli"
#define PRIu64		"llu"
#define PRIo64		"llo"
#define PRIx64		"llx"
#define PRIX64		"llX"

#define PRIdLEAST64	"lld"
#define PRIiLEAST64	"lli"
#define PRIuLEAST64	"llu"
#define PRIoLEAST64	"llo"
#define PRIxLEAST64	"llx"
#define PRIXLEAST64	"llX"

#define PRIdFAST64	"lld"
#define PRIiFAST64	"lli"
#define PRIuFAST64	"llu"
#define PRIoFAST64	"llo"
#define PRIxFAST64	"llx"
#define PRIXFAST64	"llX"

#define PRIdMAX		"lld"
#define PRIiMAX		"lli"
#define PRIuMAX		"llu"
#define PRIoMAX		"llo"
#define PRIxMAX		"llx"
#define PRIXMAX		"llX"

#define PRIdPTR		"d"
#define PRIiPTR		"i"
#define PRIuPTR		"u"
#define PRIoPTR		"o"
#define PRIxPTR		"x"
#define PRIXPTR		"X"

#define SCNd8		"hhd"
#define SCNi8		"hhi"
#define SCNu8		"hhu"
#define SCNo8		"hho"
#define SCNx8		"hhx"
#define SCNX8		"hhX"

#define SCNdLEAST8	"hhd"
#define SCNiLEAST8	"hhi"
#define SCNuLEAST8	"hhu"
#define SCNoLEAST8	"hho"
#define SCNxLEAST8	"hhx"
#define SCNXLEAST8	"hhX"

#define SCNdFAST8	"hhd"
#define SCNiFAST8	"hhi"
#define SCNuFAST8	"hhu"
#define SCNoFAST8	"hho"
#define SCNxFAST8	"hhx"
#define SCNXFAST8	"hhX"

#define SCNd16		"hd"
#define SCNi16		"hi"
#define SCNu16		"hu"
#define SCNo16		"ho"
#define SCNx16		"hx"
#define SCNX16		"hX"

#define SCNdLEAST16	"hd"
#define SCNiLEAST16	"hi"
#define SCNuLEAST16	"hu"
#define SCNoLEAST16	"ho"
#define SCNxLEAST16	"hx"
#define SCNXLEAST16	"hX"

#define SCNdFAST16	"d"
#define SCNiFAST16	"i"
#define SCNuFAST16	"u"
#define SCNoFAST16	"o"
#define SCNxFAST16	"x"
#define SCNXFAST16	"X"

#define SCNd32		"ld"
#define SCNi32		"li"
#define SCNu32		"lu"
#define SCNo32		"lo"
#define SCNx32		"lx"
#define SCNX32		"lX"

#define SCNdLEAST32	"ld"
#define SCNiLEAST32	"li"
#define SCNuLEAST32	"lu"
#define SCNoLEAST32	"lo"
#define SCNxLEAST32	"lx"
#define SCNXLEAST32	"lX"

#define SCNdFAST32	"ld"
#define SCNiFAST32	"li"
#define SCNuFAST32	"lu"
#define SCNoFAST32	"lo"
#define SCNxFAST32	"lx"
#define SCNXFAST32	"lX"

#define SCNd64		"lld"
#define SCNi64		"lli"
#define SCNu64		"llu"
#define SCNo64		"llo"
#define SCNx64		"llx"
#define SCNX64		"llX"

#define SCNdLEAST64	"lld"
#define SCNiLEAST64	"lli"
#define SCNuLEAST64	"llu"
#define SCNoLEAST64	"llo"
#define SCNxLEAST64	"llx"
#define SCNXLEAST64	"llX"

#define SCNdFAST64	"lld"
#define SCNiFAST64	"lli"
#define SCNuFAST64	"llu"
#define SCNoFAST64	"llo"
#define SCNxFAST64	"llx"
#define SCNXFAST64	"llX"

#define SCNdMAX		"lld"
#define SCNiMAX		"lli"
#define SCNuMAX		"llu"
#define SCNoMAX		"llo"
#define SCNxMAX		"llx"
#define SCNXMAX		"llX"

#define SCNdPTR		"d"
#define SCNiPTR		"i"
#define SCNuPTR		"u"
#define SCNoPTR		"o"
#define SCNxPTR		"x"
#define SCNXPTR		"X"

#endif

