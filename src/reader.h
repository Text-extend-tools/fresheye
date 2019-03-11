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
 * $Id: reader.h,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

typedef struct {
	FILE* f;
	char* s;
	int len;
	int codepage;
} reader;

typedef struct {
	reader* r;
	const char* cp;
	const char* w;
	int len;
} word_reader;

reader* rdr_init ( FILE* f, const int len, int codepage );
void rdr_free ( reader* this );
const char* rdr_gets ( reader* this ); 
const char* rdr_skip ( reader* this, unsigned long n );

word_reader* wrr_init ( FILE* f, int codepage );
void wrr_free ( word_reader* this );
const char* wrr_getw ( word_reader* this );
int wrr_get_word_length ( const word_reader* this );
