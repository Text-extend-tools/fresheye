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
 * $Id: reader.c,v 1.3 2002/06/27 00:44:00 vadimp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "config.h"
#include "cyrillic.h"
#include "reader.h"
#include "wrappers.h"
#include "util.h"
#include "fe.h"

/*
 * Allocates and initializes a reader. Buffer length must be at least 2
 * characters long (a char + terminating zero).
 *
 * Returns pointer to allocated and initialized reader if successful,
 * otherwise returns NULL.
 */
reader* rdr_init ( FILE* f, const int len, int codepage ) {

	reader* this = NULL;

	assert ( len > 1 );
	assert ( f );

	this = (reader *) xmalloc ( sizeof ( reader ) );
	this -> s = (char *) xmalloc ( len );
	this -> f = f;
	this -> len = len;
	this -> codepage = codepage;
	return this;
}

/*
 * Deallocates a reader
 */
void rdr_free ( reader* this ) {

	assert ( this );

	if ( this -> s )
		free ( this -> s );

	free ( this );
}

/*
 * Seeks file f for a newline character or EOF starting from the current 
 * position.
 * 
 * Returns position of newline or EOF if successful, otherwise -1L
 */
static long int seek_newline_or_eof ( FILE* f ) {
	
	int ch;

	assert ( f );
	
	while ( (ch = fgetc ( f )) != EOF && ch != '\n' )
		;	/* does "nothing" */

	if ( ch == EOF && !feof ( f ) ) {
		perror ( "fe: fgetc () failed" );
		exit ( -1 );
	}

	return xftell ( f );
}

/*
 * Resizes a reader's buffer to len characters. __BROKEN_REALLOC__ must be
 * defined on platforms where realloc (3) damages the contents of the block
 * of memory which is being re-allocated.
 *
 * Returns this if successful, otherwise NULL.
 */
static reader* resize_buffer ( reader* this, int len ) {

#ifdef __BROKEN_REALLOC__
	char* s = NULL;
#endif

	assert ( this );
	assert ( this -> s );
	assert ( len > 0 );

#ifdef __BROKEN_REALLOC__
	s = xmalloc ( len );
	strcpy ( s, this -> s );
	free ( this -> s );
	this -> s = s;
#else
	this -> s = xrealloc ( this -> s, len );
#endif
	this -> len = len;

	return this;
}

/*
 * Chops newline character off a string.
 *
 * Returns s if successful, otherwise (s isn't terminated with '\n') NULL.
 */
static char* chop ( char* s ) {

	char* tmp;

	assert ( s );

	tmp = strchr ( s, '\0' );
	if ( tmp [ -1 ] == '\n' ) {
		tmp [ -1 ] = '\0';
		return s;
	}

	return NULL;
}

/*
 * Causes reader to read the next string from the input file. If a reader's
 * buffer is shorter than the current string, it resizes the buffer and reads
 * up remaining characters.
 *
 * Returns pointer to the internal string buffer if successful, otherwise NULL.
 * It is advised to check input file for EOF condition when NULL is returned.
 */
const char* rdr_gets ( reader* this ) {

	long int eolpos; /* Newline character's position in file */
	long int curpos; /* Current position in file */
	long int gap;    /* Buffer's growth value */
	int oldlen;	 /* Old length reader's buffer */

	assert ( this );

	if ( fgets ( this -> s, this -> len, this -> f ) == NULL )
		return NULL;

	if ( chop ( this -> s ) ) {
		if ( this -> codepage != CYR_CP_DEFAULT )
			recode_cyrillics ( this -> s, this -> s, 
					CYR_CP_DEFAULT,	this -> codepage );
		return this -> s;
	}

	if ( feof ( this -> f ) )
		/* '\n' isn't the last character in the input file, it's Ok. */
		return this -> s;

	/* The buffer is shorter than the current line. */
	curpos = xftell ( this -> f );
	eolpos = seek_newline_or_eof ( this -> f ) - 1;
	gap = eolpos - curpos + 1;
	oldlen = this -> len;

	resize_buffer ( this, (int) (this -> len + gap ) );
	xfseek ( this -> f, curpos, SEEK_SET );

	if ( fgets ( this -> s + oldlen - 1, (int) gap + 1,
				this -> f ) == NULL )
		return NULL;

	chop ( this -> s ); /* It may or may not be terminated with '\n' */

	if ( this -> codepage != CYR_CP_DEFAULT )
		recode_cyrillics ( this -> s, this -> s, CYR_CP_DEFAULT,
				this -> codepage );
	return this -> s;
}

const char* rdr_skip ( reader* this, unsigned long n ) {

	while ( n -- )
		rdr_gets ( this );
	return this -> s;
}

word_reader* wrr_init ( FILE* f, int codepage ) {
	
	word_reader* this = NULL;

	assert ( f );

	this = xmalloc ( sizeof ( word_reader ) );
	memset ( this, 0, sizeof ( word_reader ) );

	this -> r = rdr_init ( f, 80, codepage );
	
	return this;
}

void wrr_free ( word_reader* this ) {
	rdr_free ( this -> r );
	free ( this );
}

static __inline int wrr_scan_for_letter ( word_reader* this ) {

	while ( this -> cp 
		&& *(this -> cp) 
		&& !cyr_isletter ( *(this -> cp) ) )
		this -> cp ++;
	
	return this -> cp && *(this -> cp);
}

static __inline int wrr_scan_for_non_letter ( word_reader* this ) {

	while ( this -> cp && *(this -> cp) && cyr_isletter ( *(this -> cp) ) )
		this -> cp ++;

	return this -> cp && *(this -> cp);
}

const char* wrr_getw ( word_reader* this ) {

	while ( !wrr_scan_for_letter ( this ) )
		if ( !(this -> cp = rdr_gets ( this -> r )) )
			return NULL;	/* EOF or I/O error */

	this -> w = this -> cp;	/* Store start position */
	wrr_scan_for_non_letter ( this );
	this -> len = this -> cp - this -> w;

	return this -> w;
}

int wrr_get_word_length ( const word_reader* this ) {
	return this -> len;
}
