/* @(#)nls.h	1.2 00/04/26 2000 J. Schilling */
/*
 *	Modifications to make the code portable Copyright (c) 2000 J. Schilling
 *	Thanks to Georgy Salnikov <sge@nmr.nioch.nsc.ru>
 *
 *	Code taken from the Linux kernel.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _NLS_H
#define _NLS_H

#include <unls.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT

#define CONFIG_NLS_CODEPAGE_437
#define CONFIG_NLS_CODEPAGE_737
#define CONFIG_NLS_CODEPAGE_775
#define CONFIG_NLS_CODEPAGE_850
#define CONFIG_NLS_CODEPAGE_852
#define CONFIG_NLS_CODEPAGE_855
#define CONFIG_NLS_CODEPAGE_857
#define CONFIG_NLS_CODEPAGE_860
#define CONFIG_NLS_CODEPAGE_861
#define CONFIG_NLS_CODEPAGE_862
#define CONFIG_NLS_CODEPAGE_863
#define CONFIG_NLS_CODEPAGE_864
#define CONFIG_NLS_CODEPAGE_865
#define CONFIG_NLS_CODEPAGE_866
#define CONFIG_NLS_CODEPAGE_869
#define CONFIG_NLS_CODEPAGE_874
#define CONFIG_NLS_ISO8859_1
#define CONFIG_NLS_ISO8859_2
#define CONFIG_NLS_ISO8859_3
#define CONFIG_NLS_ISO8859_4
#define CONFIG_NLS_ISO8859_5
#define CONFIG_NLS_ISO8859_6
#define CONFIG_NLS_ISO8859_7
#define CONFIG_NLS_ISO8859_8
#define CONFIG_NLS_ISO8859_9
#define CONFIG_NLS_ISO8859_14
#define CONFIG_NLS_ISO8859_15
#define CONFIG_NLS_KOI8_R

#define CONFIG_NLS_MAC_ROMAN

extern int init_nls_iso8859_1	__PR((void));
extern int init_nls_iso8859_2	__PR((void));
extern int init_nls_iso8859_3	__PR((void));
extern int init_nls_iso8859_4	__PR((void));
extern int init_nls_iso8859_5	__PR((void));
extern int init_nls_iso8859_6	__PR((void));
extern int init_nls_iso8859_7	__PR((void));
extern int init_nls_iso8859_8	__PR((void));
extern int init_nls_iso8859_9	__PR((void));
extern int init_nls_iso8859_14	__PR((void));
extern int init_nls_iso8859_15	__PR((void));
extern int init_nls_cp437	__PR((void));
extern int init_nls_cp737	__PR((void));
extern int init_nls_cp775	__PR((void));
extern int init_nls_cp850	__PR((void));
extern int init_nls_cp852	__PR((void));
extern int init_nls_cp855	__PR((void));
extern int init_nls_cp857	__PR((void));
extern int init_nls_cp860	__PR((void));
extern int init_nls_cp861	__PR((void));
extern int init_nls_cp862	__PR((void));
extern int init_nls_cp863	__PR((void));
extern int init_nls_cp864	__PR((void));
extern int init_nls_cp865	__PR((void));
extern int init_nls_cp866	__PR((void));
extern int init_nls_cp869	__PR((void));
extern int init_nls_cp874	__PR((void));
extern int init_nls_koi8_r	__PR((void));

extern int init_nls_mac_roman	__PR((void));

#endif	/* _NLS_H */
