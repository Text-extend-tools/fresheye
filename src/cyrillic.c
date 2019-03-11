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
 * $Id: cyrillic.c,v 1.1.1.1 2000/10/17 01:16:55 vadimp Exp $
 */

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "cyrillic.h"

static int* code_table = NULL;
static int encoding = CYR_CP_UNDEFINED;

extern int code_table_koi8_r [];
extern int code_table_cp866 [];
extern int code_table_cp1251 [];
extern int code_table_mac [];
extern int code_table_iso_8859_5 [];
extern int code_table_unicode [];
extern int ord2chr [] [ 6 ];

static int* select_codepage ( int codepage );

#define CODE_TABLE( x ) (code_table [ (x) - EXTENDED_ASCII_OFFSET ])
#define CODE_TABLE_EX( x, cp ) (select_codepage ( cp ) [ (x) - EXTENDED_ASCII_OFFSET ])

static int* select_codepage ( int codepage ) {

  switch ( codepage ) {
		
    case CYR_CP_KOI8_R:
      return code_table_koi8_r;
			
    case CYR_CP_CP866:
      return code_table_cp866;
		
    case CYR_CP_CP1251:
      return code_table_cp1251;
			
    case CYR_CP_MAC:
      return code_table_mac;
			
    case CYR_CP_ISO_8859_5:
      return code_table_iso_8859_5;

    case CYR_CP_UNICODE:
      return code_table_unicode;

    }	

  return NULL;
	
}

int cyr_set_default_codepage ( int codepage ) {

  switch ( codepage ) {
		
    case CYR_CP_KOI8_R:
      code_table = select_codepage ( encoding = CYR_CP_KOI8_R );
      break;
			
    case CYR_CP_CP866:
      code_table = select_codepage ( encoding = CYR_CP_CP866 );
      break;
		
    case CYR_CP_CP1251:
      code_table = select_codepage ( encoding = CYR_CP_CP1251 );
      break;
			
    case CYR_CP_MAC:
      code_table = select_codepage ( encoding = CYR_CP_MAC );
      break;
			
    case CYR_CP_ISO_8859_5:
      code_table = select_codepage ( encoding = CYR_CP_ISO_8859_5 );
      break;

    case CYR_CP_UNICODE:
      code_table = select_codepage ( encoding = CYR_CP_UNICODE );
      break;

    default:
      code_table = select_codepage ( encoding = CYR_CP_UNDEFINED );
			
    }	

  return code_table != NULL;
		
}

int cyr_get_default_codepage ( void ) {

  return encoding;
	
}

int cyr_ord ( cyr_letter letter ) {

  assert ( cyr_isletter ( letter ) );
	
  return CODE_TABLE ( letter );
   	
}

int cyr_ord_ex ( cyr_letter letter, int codepage ) {

  assert ( cyr_isletter_ex ( letter, codepage ) );
			
  return select_codepage ( codepage ) != NULL ? 
    CODE_TABLE_EX ( letter, codepage ) : CYR_NON_LETTER;

}

cyr_letter cyr_chr ( int index ) {

  assert ( code_table != NULL );
  assert ( index >= 0 && index < CYR_CHARACTER_COUNT );

  return ord2chr [ index ] [ encoding - 1 ];
}

cyr_letter cyr_chr_ex ( int index, int codepage ) {
  
  return ord2chr [ index ] [ codepage - 1 ];

}

int cyr_isletter ( cyr_letter letter ) {

	return letter >= EXTENDED_ASCII_OFFSET 
		&& CODE_TABLE ( letter ) != CYR_NON_LETTER;
	
}

int cyr_isletter_ex ( cyr_letter letter, int codepage ) {

	return select_codepage ( codepage ) ? 
		letter >= EXTENDED_ASCII_OFFSET 
		&& CODE_TABLE_EX ( letter, codepage ) != CYR_NON_LETTER : 0;
	
}

int cyr_iscap ( cyr_letter letter ) {

  assert ( cyr_isletter ( letter ) );

  return CODE_TABLE ( letter ) < CYR_LETTER_COUNT; 
	
}

int cyr_iscap_ex ( cyr_letter letter, int codepage ) {

  assert ( cyr_isletter_ex ( letter, codepage ) );
	
  return select_codepage ( codepage ) != NULL ? 
    CODE_TABLE_EX ( letter, codepage ) < CYR_LETTER_COUNT : 0;
	
}

int cyr_islow ( cyr_letter letter ) {
	
  assert ( cyr_isletter ( letter ) );
	
  return CODE_TABLE ( letter ) >= CYR_LETTER_COUNT;
	
}

int cyr_islow_ex ( cyr_letter letter, int codepage ) {

  assert ( cyr_isletter_ex ( letter, codepage ) );
	
  return select_codepage ( codepage ) != NULL ?
    CODE_TABLE_EX ( letter, codepage ) >= CYR_LETTER_COUNT : 0;
	
}

cyr_letter cyr_downc ( cyr_letter letter ) {

  assert ( cyr_isletter ( letter ) );

  return cyr_islow ( letter ) ? letter : 
    cyr_chr ( CODE_TABLE ( letter ) + CYR_LETTER_COUNT );	
	
}	

cyr_letter cyr_downc_ex ( cyr_letter letter, int codepage ) {

	 assert ( cyr_isletter_ex ( letter, codepage ) );
	
	if ( select_codepage ( codepage ) == NULL )
		return 0;
	
	return cyr_islow_ex ( letter, codepage ) ? letter : 
		cyr_chr_ex ( CODE_TABLE_EX ( letter, codepage ) + 
		CYR_LETTER_COUNT, codepage );	
	
}

cyr_letter cyr_upc ( cyr_letter letter ) {

  assert ( cyr_isletter ( letter ) );

  return cyr_iscap ( letter ) ? letter : 
    cyr_chr ( CODE_TABLE ( letter ) - CYR_LETTER_COUNT );	
	
}

cyr_letter cyr_upc_ex ( cyr_letter letter, int codepage ) {

	assert ( cyr_isletter_ex ( letter, codepage ) );
	
	if ( select_codepage ( codepage ) == NULL )
		return 0;
	
	return cyr_iscap_ex ( letter, codepage ) ? letter :
		cyr_chr_ex ( CODE_TABLE_EX ( letter, codepage ) - 
		CYR_LETTER_COUNT, codepage );
	
}

int cyr_codepage_by_name ( const char* s ) {

	static const struct {
		const char* name;
		int codepage;
	} codepages [] = {
		{ "koi8-r", CYR_CP_KOI8_R },
		{ "cp866", CYR_CP_CP866 },
		{ "cp1251", CYR_CP_CP1251 },
		{ "mac", CYR_CP_MAC },
		{ "iso8859-5", CYR_CP_ISO_8859_5 },
		{ NULL, 0 }
	};
	
	int i;

	for ( i = 0; codepages [ i ].name; i ++ )
		if ( !strcmp ( s, codepages [ i ].name ) )
			return codepages [ i ].codepage;
	
	return CYR_CP_UNDEFINED;
}

int translate_special_character ( int dst_codepage, int src_codepage, int ch )
{
	static const int xlat [ 6 ] [ 6 ] = 
	{
		{ '"', 0xab, '"', 0xc7, '"', 0xab }, /* opening Russian quote */
		{ '"', 0xbb, '"', 0xc8, '"', 0xbb }, /* closing Russian quote */
		{ 0xbe, 0xb9, 0xfc, 0xdc, 0xf0, 0x2116 }, /* numero sign */
		{ '-', 0x96, '-', 0xd0, '-', 0x2013 }, /* en-dash */
		{ '-', 0x97, '-', 0xd1, '-', 0x2014 }, /* em-dash */
		{ 0x9a, 0xa0, 0xff, 0xca, 0xa0, 0xa0 } /* non-breaking space */
	};
	int i;
	
	if ( ch < 0x80 )
		return ch;
	
	for ( i = 0; i < 6; i ++ )
		if ( ch == xlat [ i ] [ src_codepage - 1 ] )
			return xlat [ i ] [ dst_codepage - 1 ];
	
	return ch;
}
