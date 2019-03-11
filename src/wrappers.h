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
 * $Id: wrappers.h,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

/*
 * realloc (3) wrapper. On errors it prints error message and calls exit (3).
 */
void* xrealloc ( void* block, size_t size );

/*
 * malloc (3) wrapper. On errors it prints error message and calls exit (3).
 */
void* xmalloc ( size_t size );

/*
 * ftell (3) wrapper. On system errors it prints error message and calls
 * exit (3).
 */
long int xftell ( FILE* f );

/*
 * fseek (3) wrapper. On system errors it prints error message and calls
 * exit (3).
 */
int xfseek ( FILE *f, long offset, int whence );

/*
 * fopen (3) wrapper. On system errors it prints error message and calls 
 * exit (3).
 */
FILE* xfopen ( const char* path, const char* mode );

/*
 * fclose (3) wrapper. On system errors it prints error message and calls 
 * exit (3).
 */
int xfclose ( FILE* stream );

/*
 * strdup (3) wrapper. On memory allocation errors it prints error message and
 * calls exit (3).
 */
char* xstrdup ( const char* s );
