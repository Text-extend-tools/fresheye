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
 * $Id: context.c,v 1.4 2002/06/21 00:53:13 vadimp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "config.h"
#include "fe.h"
#include "reader.h"
#include "context.h"
#include "wrappers.h"
#include "cyrillic.h"
#include "util.h"

static word* wrd_init ( line* line, int start, int length, 
		int delimiters, int proper ) {

	word* this = NULL;
	
	assert ( line );
	assert ( start >= 0 );
	assert ( length > 0 );

	this = xmalloc ( sizeof ( word ) );
	this -> line = line;
	this -> line -> refcnt ++;
	this -> position = start;
	this -> next = NULL;
	this -> text = strndup ( line -> text + start, length );
	unify_word ( this -> text );
	this -> logical = xmalloc ( length + 1 );
	convert_to_logical ( this -> logical, this -> text );
	this -> length = length;
	this -> delimiters = delimiters;
	this -> proper = proper;
	
	return this;
}

static void wrd_free ( word* this ) {

	if ( this == NULL )
		return;

	if ( this -> text )
		free ( this -> text );

	if ( this -> logical )
		free ( this -> logical );
	
	wrd_free ( this -> next );
	free ( this );
}

static __inline void wrd_free_one ( word* this ) {
	this -> next = NULL;
	wrd_free ( this );
}

static line* line_init ( const char* text, unsigned long number ) {

	line* this = NULL;
	
	assert ( text );
	assert ( number > 0 );
	
	this = xmalloc ( sizeof ( line ) );
	this -> text = xstrdup ( text );
	this -> length = strlen ( text );
	this -> number = number;
	this -> refcnt = 0;
	this -> next = NULL;

	return this;
}

static void line_free ( line* this ) {

	if ( this == NULL )
		return;

	if ( this -> text )
		free ( this -> text );
	line_free ( this -> next );
	free ( this );
}

static __inline void line_free_one ( line* this ) {
	this -> next = NULL;
	line_free ( this );
}

static context* ctx_fetch_line ( context* this ) {
	
	line* l = NULL;
	const char* s = NULL;
	
	assert ( this );
	
	s = rdr_gets ( this -> rdr );
	if ( s == NULL )
		return NULL;	/* EOF */
	this -> total_lines ++;
	l = line_init ( s, this -> total_lines );
	this -> cp = l -> text;
	if ( this -> lines_head == NULL ) 
		this -> lines_tail = this -> lines_head = l;
	else
		this -> lines_tail = this -> lines_tail -> next = l;

	this -> line_cnt ++;
	
	return this;
}

static context* ctx_remove_topmost_word ( context* this ) {

	word* w = NULL;
	line* l = NULL;
	
	assert ( this );
	assert ( this -> words_head );

	/* Detach topmost word from the list and free it */
	w = this -> words_head;
	this -> words_head = this -> words_head -> next;
	w -> line -> refcnt --;
	wrd_free_one ( w );
	this -> word_cnt --;
	
	/* Check if some unreferenced lines can be removed */
	l = this -> lines_head;
	while ( l && !l -> refcnt ) {
		line* tmp = l -> next;
		line_free_one ( l );
		this -> line_cnt --;
		l = tmp;
	}
	this -> lines_head = l;
	
	return this;
}

static word* ctx_make_new_word ( context* this ) {

	int start;
	int end;
	
	start = this -> cp - this -> lines_tail -> text;
	while ( cyr_isletter ( *(this -> cp) ) )
		this -> cp ++;
	end = this -> cp - this -> lines_tail -> text;
	return wrd_init ( this -> lines_tail, start, end - start, 
				this -> delimiters, this -> proper );
}

static int count_delimiters ( context* this ) {

	int res = 0;

	if ( this -> immediate_new && this -> new_sentence )
		this -> immediate_new = this -> new_sentence = 0;

	switch ( *(this -> cp) ) {
		
		case ' ':
			if ( !this -> spaces ) {
				res ++;
				this -> spaces = 1;
      			}
			break;
			
		case ',':
			res += 2;
			break;
			
		case '.':
			res += 4;
			this -> new_sentence = 1;
			break;
			
		case ';':
			res += 3;
			break;
			
		case ':':
			res += 3;
			break;
			
		case '!':
			res += 4;
			this -> new_sentence = 1;
			break;
			
		case '?':
			res += 4;
			this -> new_sentence = 1;
			break;
			
		case ')':
			res += 3;
			break;
			
		case '(':
			res += 3;
			break;
			
		case '"':
			res += 3;
			this -> new_sentence = 1;
			/* To switch new_sentence off if the word doesn't
			 * follow immediately */
			this -> immediate_new = 1; 
			break;
			
		case '-':
			if ( this -> spaces )
				res += 3;
			else
				res ++;
			break;

		case '^':
			break;
		
		default:
			if ( !this -> spaces ) {
				res ++;
				this -> spaces = 1;
			}
			break;
	}

  return res;

}

/*
 * Skips to a word or end-of-string in the current string.
 * Returns nonzero if a word found
 */
__inline int ctx_skip_to_word ( context* this ) {

	assert ( this );
	
	while ( this -> cp 
		&& *(this -> cp) 
		&& !cyr_isletter ( *(this -> cp) ) ) {
		this -> delimiters += count_delimiters ( this );
		this -> cp ++; /* skip to a letter or end-of-string */
	}

	return this -> cp && *this -> cp;
}

/*
 * Searches for next Russian word in input. If a new word found, this -> cp
 * points to its first letter.
 * Returns this if successful, otherwise (EOF or I/O error) NULL;
 */
__inline context* ctx_search_for_word ( context* this ) {

	int newline = 0;
	
	while ( !ctx_skip_to_word ( this ) ) {
		if ( ctx_fetch_line ( this ) == NULL )
			return NULL;
		if ( !newline )
			newline = 1;
		else
			this -> par = 1;
	}
	
	return this;
}

context* ctx_init ( const char* path, const int width, int codepage ) {
	
	context* this = NULL;

	assert ( path );
	assert ( width > 0 );

	this = xmalloc ( sizeof ( context ) );
	memset ( this, 0, sizeof ( context ) );

	this -> width = width;
	this -> path = xstrdup ( path );
	this -> f = xfopen ( path, "r" );
	/* The only reasonable guess about maximal line length is 80 */
	this -> rdr = rdr_init ( this -> f, 80, codepage );
	
	this -> new_sentence = 1;
	
	return this;	
}

void ctx_free ( context* this ) {

	assert ( this );
	
	if ( this -> rdr )
		rdr_free ( this -> rdr );
	
	xfclose ( this -> f );
	
	if ( this -> cur_word )
		wrd_free ( this -> cur_word );
	
	if ( this -> words_head )
		wrd_free ( this -> words_head );
	
	if ( this -> lines_head )
		line_free ( this -> lines_head );

	free ( this );
}

/*
 * Shifts context by feeding the current word into word list and fetching a
 * new word from the source file.
 * Returns its actual parameter if successful, otherwise NULL.
 */
context* ctx_shift ( context* this ) {

	word* w = NULL;
	
	assert ( this );

	this -> proper = this -> spaces = this -> delimiters = 0;
	
	if ( ctx_search_for_word ( this ) == NULL )
		return NULL;	/* EOF or I/O error */

	if ( this -> par ) {
		this -> delimiters += 8;
		this -> par = 0;
	}

	this -> immediate_new = 0;
	if ( this -> new_sentence )
		this -> new_sentence = 0;
	else
		this -> proper = cyr_isletter ( *(this -> cp) )
				 && cyr_isletter ( *(this -> cp + 1) )
				 && cyr_iscap ( *(this -> cp) )
		     		 && cyr_islow ( *(this -> cp + 1) );

	w = ctx_make_new_word ( this );
	if ( this -> cur_word ) 
		this -> cur_word -> delimiters = w -> delimiters;
	this -> total_words ++;

	/* Place current word to the end of the list of words */
	if ( this -> words_head == NULL )
		this -> words_tail = this -> words_head = this -> cur_word;
	else
		this -> words_tail = 
			this -> words_tail -> next = this -> cur_word;

	/* Increase context width, remove topmost word if 
	 * context's capacity is exceeded */
	if ( this -> word_cnt ++ == this -> width )
		ctx_remove_topmost_word ( this );

	/* Make new word current */
	this -> cur_word = w;
	
	return this;
}

context* ctx_skip_lines ( context* this, unsigned long n ) {

	if ( !(this -> cp = rdr_skip ( this -> rdr, n )) )
		return NULL;
	this -> total_lines += n;
	this -> cp = NULL;

	return this;
}

