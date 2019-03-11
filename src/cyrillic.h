/*
 * Fresh Eye, a program for Russian writing style checking
 * Copyright (C) 1999  OnMind Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: cyrillic.h,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

#define CYR_LETTER_COUNT		33
#define CYR_CHARACTER_COUNT		CYR_LETTER_COUNT * 2
#define CYR_NON_LETTER			CYR_CHARACTER_COUNT + 1
#define EXTENDED_ASCII_OFFSET	0x80

enum 
{
  CYR_CP_UNDEFINED,
  CYR_CP_KOI8_R,
  CYR_CP_CP1251,
  CYR_CP_CP866,
  CYR_CP_MAC,
  CYR_CP_ISO_8859_5,
  CYR_CP_UNICODE
};

typedef unsigned char cyr_letter;

int cyr_set_default_codepage ( int codepage );
int cyr_get_default_codepage ( void );
int cyr_ord ( cyr_letter letter );
int cyr_ord_ex ( cyr_letter letter, int codepage );
cyr_letter cyr_chr ( int index );
cyr_letter cyr_chr_ex ( int index, int codepage );
int cyr_isletter ( cyr_letter letter );
int cyr_isletter_ex ( cyr_letter letter, int codepage );
int cyr_iscap ( cyr_letter letter );
int cyr_iscap_ex ( cyr_letter letter, int codepage );
int cyr_islow ( cyr_letter letter );
int cyr_islow_ex ( cyr_letter letter, int codepage );
cyr_letter cyr_downc ( cyr_letter letter );
cyr_letter cyr_downc_ex ( cyr_letter letter, int codepage );
cyr_letter cyr_upc ( cyr_letter letter );
cyr_letter cyr_upc_ex ( cyr_letter letter, int codepage );
int cyr_codepage_by_name ( const char* s ); 
int translate_special_character ( int dst_codepage, int src_codepage, int ch );
