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
 * $Id: util.c,v 1.3 2002/06/21 00:53:13 vadimp Exp $
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include "config.h"
#include "util.h"
#include "wrappers.h"
#include "cyrillic.h"

void fatal_error ( const char* message, const int use_errno ) {

	assert ( message );

	if ( use_errno )
		fprintf ( stderr, PACKAGE": %s: %s\n", message, 
				strerror ( errno ) );
	else
		fprintf ( stderr, "fe: %s\n", message );

	exit ( -1 );
}

__inline int lnum (const char *c) {  /* letter number in the alphabet */
  return cyr_ord ( *c ) % CYR_LETTER_COUNT;
}

char* strndup ( const char* s, size_t n ) {
	
	char* dst = xmalloc ( n + 1 );
	char* cp = dst;
	
	while ( n -- && *s )
		*cp ++ = *s ++;
	*cp = '\0';

	return dst;
}

__inline char* unify_word ( char* s ) {

	char* cp = s;
	
	while ( *cp ) {
		*cp = cyr_downc ( *cp );
		cp ++;

	}

	return s;
}

/*
 * Converts src from native cyrillic encoding into "logical" one and places the
 * result into dst. Logical encoding starts from 1 -- first capital letter of
 * the Russian alphabet -- and ends at 64 -- last small letter of the Russian 
 * alphabet.
 * Returns dst.
 * Obviously, you can't use this function if src contains non-cyrillic 
 * characters. 
 */

char* convert_to_logical ( char* dst, const char* src ) {

	char* p = dst;
	
	while ( *src ) 
		*p ++  = lnum ( src ++ ) + 1;
	*p = '\0';

	return dst;
}

char* convert_to_physical ( char* dst, const char* src ) {

	char* p = dst;

	while ( *src )
		*p ++ = cyr_chr ( *src ++ - 1 + CYR_LETTER_COUNT );
	*p = '\0';

	return dst;
}

char* strccpy ( char* dst, const char* src, char ch ) {

	char* cp = dst;
	
	while ( *src && *cp ) {
		if ( *src == ch )
			*cp = ch;
		src ++;
		cp ++;
	}

	return dst;
}

char* recode_cyrillics ( char* dst, const char* src, 
		int dst_codepage, int src_codepage ) {
	
	char* cp = dst;

	while ( *src ) {
		if ( !cyr_isletter_ex ( *src, src_codepage ) )
			*cp = *src;
		else {
			int ord = cyr_ord_ex ( *src, src_codepage );
			*cp = cyr_chr_ex ( ord, dst_codepage );
		}
		src ++;
		cp ++;
	}
	*cp = '\0';
	
	return dst;
}
