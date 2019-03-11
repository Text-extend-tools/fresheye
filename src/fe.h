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
 * $Id: fe.h,v 1.3 2002/06/27 00:44:00 vadimp Exp $
 */

#if !defined ( _WIN32 ) || defined ( __GNUC__ )
#define min( a, b ) ((a) < (b) ? (a) : (b))
#endif

#define MAXWLEN 20  /* maximum word length to be stored in wordcount */
#define MAXWIDTH 30 /* maximum number of words checked (length of context) */
#define CONTEXT_LINES 7     /* number of lines stored */
#define VOCSIZE 65  /* number of exceptions */

extern short int sim_ch [34] [34]; /* letters' similarity map */
extern int inf_letters [34] [3];   /* quantity of information in letters */
extern char voc [VOCSIZE] [2][20];    /* exceptions vocabulary */

extern int context_size;
extern int sensitivity_threshold;
extern int wordcount_use_coefficient;
extern int quiet_logging;
extern int dump_wordcount;
extern int exclude_proper_names;
extern int resume_processing;	/* -1 means "ask user" */
extern int cancel_processing;
extern int yes_to_all;
extern char* log_path;
extern int input_codepage;
extern int output_codepage;
int wordcmp ( const char* s1, int len1, const char* s2, int len2 );
