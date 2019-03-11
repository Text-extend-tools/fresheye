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
 * $Id: wrappers.c,v 1.1.1.1 2000/10/17 01:16:59 vadimp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "wrappers.h"
#include "util.h"

void* xrealloc ( void* block, size_t size ) {

	void* p = realloc ( block, size );

	if ( p == NULL ) 
		fatal_error ( "memory allocation error", 0 );

	return p;
}

void* xmalloc ( size_t size ) {

	void* p = malloc ( size );

	if ( p == NULL ) 
		fatal_error ( "memory allocation error", 0 );

	return p;
}

long int xftell ( FILE* f ) {

	long int pos = ftell ( f );

	if ( pos == -1L ) 
		fatal_error ( "ftell () failed", 1 );

	return pos;
}

int xfseek ( FILE *f, long offset, int whence ) {

	if ( fseek ( f, offset, whence ) == -1 )
		fatal_error ( "fseek () failed", 1 );

	return 0;
}

FILE* xfopen ( const char* path, const char* mode ) {

	const char* mode_name = "undefined access mode";
	char* message = NULL;
		
	FILE* fp = fopen ( path, mode );

	if ( fp != NULL )
		return fp;
	
	message = xmalloc ( strlen ( path ) + 80 );
	
	switch ( tolower ( *mode ) ) {
		case 'r':
			mode_name = "reading";
			break;

		case 'a':
		case 'w':
			mode_name = "writing";
			break;

		default:
			;	/* does nothing */
	}
	
	sprintf ( message, "cannot open '%s' for %s", 
			path, mode_name );
	fatal_error ( message, 1 );

	/* the following is very unlikely to happen, but who knows ... */
	free ( message ); 

	return NULL;
}

int xfclose ( FILE* stream ) {
	
	if ( fclose ( stream ) == EOF ) 
		fatal_error ( "fclose () failed", 1 );

	return 0;
}

char* xstrdup ( const char* s ) {

	char* tmp = strdup ( s );
	
	if ( tmp == NULL )
		fatal_error ( "memory allocation error", 0 );

	return tmp;
}
