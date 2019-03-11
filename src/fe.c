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
 * $Id: fe.koi8-r,v 1.3 2002/06/29 05:22:14 vadimp Exp $
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <assert.h>

#include "config.h"
#include "fe.h"
#include "cyrillic.h"
#include "avl.h"
#include "wrappers.h"
#include "reader.h"
#include "context.h"
#include "util.h"
#include "ui.h"

const char *program_name = "Fresh Eye </> Свежий Взгляд";
const char *rule = 
  "==================================================================";

unsigned long cries; /* Count of issued messages */
unsigned long ogos; /* Count of recorded messages */ 

int twosigmasqr;             /* coefficient for Gaussian distribution */
unsigned long int first_line; /* first line of file to process */
int not_all_words_counted;

int context_size = 15;
int sensitivity_threshold = 500;
int wordcount_use_coefficient = 50;
int quiet_logging = 0;
int dump_wordcount = 0;
int exclude_proper_names = 1;
int resume_processing = 0;	/* -1 means "ask user" */
int cancel_processing = 0; /* Cancel processing gracefully */
int yes_to_all = 0; /* Accept all suggestions automatically */
char* log_path = NULL;
avl* tree = NULL;
FILE* log_file = NULL;

int input_codepage =  CYR_CP_DEFAULT;
int output_codepage = CYR_CP_DEFAULT;

int sqr (int x);
int wordcmp ( const char * s1, int len1, const char * s2, int len2 );
int checkvoc ( char *w1, char *w2 );
int implen ( int x );
int infor_same ( char *a, char *b );
int infor_diff ( char *a, char *b );
int simwords ( const word *a, const word *b );
unsigned long int inf_w ( char *w, int len );
int show ( context* ctx, word* w, int bad, int sim, int dist );
int check ( context* ctx );
void write_log_header ( context* ctx, FILE* f );
char* underline ( char* s, const word* w );
char* prepare_offscreen_buffer ( const line* l );
void write_log_entry ( context* ctx, word* w, int bad, int sim, int dist,
		                       FILE* f );
void write_log_footer ( context* ctx, FILE* f );
unsigned long check_log ( const char* logpath, const char* path );
void set_count_coefficient ( avl* t, node* n, void* user_data );
void print_node ( avl* t, node* n, void* user_data );
avl* wordcount ( avl* tree, FILE* f );
word* patch_proper_name ( word* w );
void validate_globals ( void ); 
int init ( void );
void cleanup ( void ); 
int process_file ( const char* path ); 

__inline int sqr (int x)
{

  return x * x;

}

/*
 * Smart comparision routine. Words having common stem considered to be equal
 * strings
 */
int wordcmp ( const char* s1, int len1, const char* s2, int len2 )
{
  int l0 = 0;
  int l1 = len1;
  int l2 = len2;

  if ( *s1 != *s2 
       || abs ( l1 - l2 ) > 2 
       || min ( l1, l2 ) <= 2 )
    return strcmp ( (const char *) s1, (const char *) s2 );

  while ( s1 [ l0 ] && s2 [ l0 ] && s1 [ l0 ] == s2 [ l0 ] )
    l0 ++;

  if ( abs ( l1 - l2 ) == 2
       || (abs ( l1 - l2 ) == 1 && min ( l1, l2 ) <= 3) )
    if ( s1 [ l0 ] && s2 [ l0 ] )
      return s1 [ l0 ] - s2 [ l0 ];

  return l0 > min ( l1, l2 ) / 2 ? 0 : s1 [ l0 ] - s2 [ l0 ];  
}

int checkvoc (char *w1, char *w2)
{

  register int t;

  for (t = 0; t < VOCSIZE; t ++)
    if ((!strcmp (w1, voc [t] [0])) && (!strcmp (w2, voc [t] [1])))
      return 1;

  return 0;
}

/*
 * Psych. importance of the word x ch. long big for small words, then slowly
 * lagging  behind the real length
 */
int implen (int x)
{
  if (x == 2)
  	return 5;
  return (x - sqr ((x - 1) / 6) + (int) (4.1 / (float) x));

}

/*
 * Calculates average quantity of information in the letters common to
 * the two words.
 */
int infor_same ( char *a, char *b )
{
  int count = 0;
  int res = 0;
  int beg = 1;
  char *p, *pp = a;

  while (*pp) {                      
    if ( (p = strchr (b, *pp)) != NULL ) {   /* bipresent letters */
      if (beg && (p == b))
	res += inf_letters [ (int) *pp ] [ 1 ];  /* beginning of the word */
      else
	res += inf_letters [ (int) *pp ] [ 0 ];  /* elsewhere */
      count ++;
    }
    beg = 0;
    pp ++;
  }
	return count ? (res / count) : 0;
}

/*
 * Calculates total quantity of information in differing letters of the
 * two words.
 */
int infor_diff ( char *a, char *b )
{
  int count = 0;
  int res = 0;
  char *p, *pp = a;

  pp = a;                     /* letters in a only */
  while (*pp) {
    if ((p = strchr (b, *pp)) == 0) {
      if (pp == a)
    res += inf_letters [ (int) *pp ] [ 1 ]; /* in the beginning of the word */
      else
    res += inf_letters [ (int) *pp ] [ 0 ]; /* elsewhere */
      count ++;
    }
    pp ++;
  }

  pp = b;                     /* letters in b only */
  while (*pp) {
    if ((p = strchr (a, *pp)) == 0) {
      if (pp == b)
    res += inf_letters [ (int) *pp ] [ 1 ]; /* in the beginning of the word */
      else
    res += inf_letters [ (int) *pp ] [ 0 ]; /* elsewhere */
      count ++;
    }
    pp ++;
  }

	return count ? res : 0;
}

/*
 * Calculates similarity of words.
 */
int simwords (const word *a, const word *b)
{
	register char *tx, *ty, *ta, *tb;
	char* parta = NULL;
	unsigned long int res = 0; /* value to be returned */
	unsigned long int resa = 0;
	int partlen;
	long int prir;
	int rever = 0;
	int dist;
	int dissimilarity_threshold = 24000; /* how much total information in 
					   differing letters reduces res
					   to zero; larger values make res
					   more tolerant to differences */
	
	if (checkvoc (a -> logical, b -> logical)) /* an exception? */
		return (0);

	if (infor_diff ( a -> logical, b -> logical ) >= dissimilarity_threshold) 
 			/* too many too rare dissimilar letters? */
                return (0);

	if ( a -> length > b -> length ) {  /* swap strings so a is always the longest */
		const word* tmp = a;
	  	a = b;
		b = tmp;
		rever = 1;
	}
  
	parta = xmalloc ( a -> length + 1 );
	
	for ( partlen = 1; partlen <= a -> length; partlen ++, resa = 0 ) {
		for ( ta = a -> logical;  
		      (a -> length - (int) (ta - a -> logical)) >= partlen;  
		      ta ++) {
			strncpy ( parta, ta, partlen );
			parta [ partlen ] = '\0';
      
			for ( tb = b -> logical;  
			      partlen <= (b -> length - 
				      (int) (tb - b -> logical));  
			      tb ++) {

        			for ( prir = 0, tx = parta, ty = tb; 
				      *tx != 0; 
				      tx ++, ty ++)
					prir += 
						sim_ch [ (int) *tx ] [ (int) *ty ];

				if ( !prir ) 
					continue;

				if ( ta > a -> logical )
					prir -= (prir * 
						(int) (ta - a -> logical)) / 
						(3 * a -> length);
				
				if ( tb > b -> logical )
					prir -= (prir * 
						(int) (tb - b -> logical)) / 
						(3 * b -> length);

				dist = rever ? 
					(b -> length - 
					 	(int) (tb - b -> logical + 
						partlen)) + 
						(int) (ta - a -> logical)
					: (a -> length - 
						(int) (ta - a -> logical + 
						partlen)) + 
						(int) (tb - b -> logical);
			
				if ( dist < 3 )
					prir += ((prir * (2 - dist)) / 3);

				if ( (unsigned long int) prir > resa)
					resa = prir;
			}
		}
		if (resa / partlen > 6) {
			prir = resa;
			dist = 3 * (a -> length + b -> length) / 8 + 1;
			res += resa + prir * 
				(partlen - min(dist, a -> length)) / (2 * dist);
		}
	}

	free ( parta );
	
	for (partlen = 1, resa = 0;  partlen <= a -> length;  partlen ++)
		resa += 9 * partlen;

	res = ((res * infor_same ( a -> logical, b -> logical )) / resa);
	      /* allowing for the info contained in the common letters */
	res = (res * (dissimilarity_threshold - infor_diff ( a -> logical, b -> logical )))/dissimilarity_threshold;
	      /* decreasing by a coefficient depending on infor_diff */

	res -= (res * (b -> length - a -> length)) / (2 * b -> length);
	      /* decreasing if words are too different in length */
	return (int) (res * a -> length * b -> length / 
			(implen (a -> length) * implen (b -> length)));
	      /* finally, taking into account the psychological length */
}

/* 
 * Returns information quantity in a word basing on the wordcount 
 */

__inline unsigned long int inf_w (char *w, int len )
{
  node* n = avl_lookup ( tree, w, len );
  return n ? n -> coefficient : 1000;
}

void help ( void )
{
	printf ( "\n" 
		"Нажмите одну из клавиш (буквы латинские), затем 'Enter':\n\n"
		"'Y': записать найденный контекст в лог-файл (%s);\n"
		"'N': пропустить это место и продолжить проверку;\n"
		"'S': остановить работу программы;\n"
		"'C': показать контекст ещё раз;\n"
		"'H': вывести эту подсказку.\n\n"
		"Нажатие одной лишь клавиши 'Enter' равнозначно выбору"
			" опции,\n"
		"указанной в квадратных скобках.\n",
		log_path );
}

int show ( context* ctx, word* w, int bad, int sim, int dist )
{
	int answer = 'C'; /* Context */

	cries ++;
	
	if ( quiet_logging )
		yes_to_all = 1;
	
	if ( !yes_to_all ) {
		while ( answer == 'C' ) {
			write_log_entry ( ctx, w, bad, sim, dist, stdout );
			while ( (answer = ask ( "\nЗапомнить",
					"NYASCH?\n" )) == 'H'
				|| answer == '?' || answer == '\0' )
			help ();
		}
	}
	
	if ( answer == 'S' ) {
		cancel_processing = 1;
		return 1;
	}
	
	if ( yes_to_all || answer == 'Y' || answer == 'A' ) {
		ogos ++;
		write_log_entry ( ctx, w, bad, sim, dist, log_file );
		if ( answer == 'A' )
			yes_to_all = 1;
	}

	return 0;
}

int check ( context* ctx )
{
	int similarity;
	int badness;
	double dal;
	long dist;
	word* w = NULL;
	word* cw = ctx -> cur_word;

	for ( w = ctx -> words_head; w; w = w -> next ) {

		word* tmp = NULL;
	
		if ( !(similarity = simwords ( w, cw )) )
			continue;

		dist = 0;
			
		for ( tmp = w; tmp; tmp = tmp -> next )
			dist += tmp -> delimiters;

		for ( tmp = w -> next; tmp; tmp = tmp -> next )
			dist += tmp -> length / 3 + 1;

		if ( wordcount_use_coefficient ) {
			dist *= 2000;
			dist /= inf_w ( w -> logical, w -> length ) + 
				inf_w ( cw -> logical, w -> length );
		}

		dal = exp (((double) (- dist * dist)) / ((double) twosigmasqr));
		badness = (int) (((float) similarity) * dal);
		
		if ( badness > sensitivity_threshold )
			if ( show ( ctx, w, badness, similarity, (int) dist ) )
				return 1;
	}
  
    return 0;
}

void write_log_header ( context* ctx, FILE* f )
{
	time_t now = time ( NULL );

	fprintf ( f, "\n\n%s  v"VERSION"\tФайл: %s%s %s\n",
	    program_name, ctx -> path, strlen ( ctx -> path ) > 8 ? " " : "\t",
	    ctime ( &now ) );
	fprintf ( f, "%s\n\n", rule );

}

/*
 * Puts caret ('^') characters to an off-screen buffer to underline w.
 */
char* underline ( char* s, const word* w )
{	
	char* pos = s + w -> position;
	int n = w -> length;
	
	while ( n -- )
		*pos ++ = '^';
	return s;
}

/*
 * Prepares off-screen buffer for "underlining" of words. 
 * It allocates a string of an appropriate length, fills it with spaces and
 * then copies all tabulation characters from the original string.
 * The string returned by this function must be freed by caller.
 */
char* prepare_offscreen_buffer ( const line* l )
{
	char* buf = xmalloc ( l -> length + 1 );
	memset ( buf, ' ', l -> length );
	buf [ l -> length ] = '\0';
	strccpy ( buf, l -> text, '\t' );

	return buf;
}

void write_log_entry ( context* ctx, word* w, int bad, int sim, int dist, 
		       FILE* f )
{
	line* l = w -> line; 

	fputc ( '\n', f );
	while ( l ) {
		fprintf ( f, "%s\n", l -> text );
		if ( w -> line == l || ctx -> cur_word -> line == l ) {
			
			char* buf = prepare_offscreen_buffer ( l );
			
			if ( w -> line == l )
				underline ( buf, w );
			if ( ctx -> cur_word -> line == l )
				underline ( buf, ctx -> cur_word );
			fprintf ( f, "%s\n", buf );
			free ( buf );
		}
		l = l -> next;
	}
	
	fprintf ( f, "                        "
		     "line %lu   sim = %u  dist = %u  badness = %u\n", 
		  ctx -> total_lines, sim, dist, bad );
}

void write_log_footer ( context* ctx, FILE* f )
{
	fprintf ( f, "%s\n", rule );
	fprintf ( f, "Строк: %lu  Начало: %lu  "
		     "Слов: %lu      Срабатываний: %lu  Записано: %lu\n",
		  ctx -> total_lines, first_line + 1, 
		  ctx -> total_words, cries, ogos );
}

unsigned long check_log ( const char* logpath, const char* path )
{	
	static const char* filename_tag = "Файл:";
	static const char* lines_format = "Строк: %lu";
	
	FILE* f = NULL;
	reader* r = NULL;
	const char* s = NULL;
	unsigned long last_checked_line = 0;
		
	if ( (f = fopen ( logpath, "r" )) == NULL )
		return 0L;

	r = rdr_init ( f, 80, output_codepage );

	/* Search for log header with the given file name in it */
	while ( (s = rdr_gets ( r )) != NULL ) {

		char* tagpos;
		char* file_name;
		
		if ( strncmp ( s, program_name, strlen ( program_name ) ) )
			continue;
		tagpos = strstr ( s, filename_tag );
		file_name = tagpos + strlen ( filename_tag ) + 1;
		if ( tagpos && !strncmp ( file_name, path, strlen ( path ) ) )
			break;
	}
		
	if ( s ) { /* Log header with the given filename found */
		s = rdr_skip ( r, 2 ); /* Jump to the first rule */
		while ( s && (s = rdr_gets ( r )) && strcmp ( s, rule ) )
			; /* Does nothing */
		if ( s && (s = rdr_gets ( r )) ) 
			sscanf ( s, lines_format, &last_checked_line );
	}

	rdr_free ( r );
	xfclose ( f );

	return last_checked_line;
}

void set_count_coefficient ( avl* t, node* n, void* user_data )
{
	unsigned long word_count = * (unsigned long *) user_data;
	
	if ( n -> count == 1 )
		n -> coefficient = 1000;
	else {
		double tmp = ((double) n -> count) / ((double) word_count);
	       /*
		* Decreasing the second 8.0 will sharpen the dependance
		* on the -c coefficient
		*/
		n -> coefficient = (unsigned long) (1 - log (tmp) * 1000 /
			((8.0 + ((float) wordcount_use_coefficient) / 8.0) * 
			log (2)));
		if ( n -> coefficient > 1000 )
			n -> coefficient = 1000;
	}
}

void print_node ( avl* t, node* n, void* user_data )
{	
	char* s = n -> key;
	FILE* f = (FILE *) user_data;
	
	fputc ( '"', f );
	while ( *s )
		fputc ( cyr_chr ( *s ++ - 1 + CYR_LETTER_COUNT ), f );
	fputc ( '"', f );
	fprintf ( f, "\t%lu\t%lu\n", n -> count, n -> coefficient );
}

avl* wordcount ( avl* tree, FILE* f )
{
	word_reader* wr = NULL;
	const char* w = NULL;
	unsigned long word_count = 0;
	assert ( tree );

	wr = wrr_init ( f, input_codepage );
	
	while ( (w = wrr_getw ( wr )) != NULL ) {
		
		int length = wrr_get_word_length ( wr );
		char* buf = strndup ( w, length );
		node* n = NULL;

		word_count ++;
		unify_word ( buf );
		convert_to_logical ( buf, buf );
		
		if ( (n = avl_lookup ( tree, buf, length )) != NULL )
			n -> count ++;
		else
			if ( avl_insert ( tree, buf, length ) == NULL ) {
				free ( buf );
				not_all_words_counted = 1;
				break;
			}

		free ( buf );
	}
	
	wrr_free ( wr );
	avl_foreach ( tree, set_count_coefficient, &word_count );
	return tree;
}

__inline word* patch_proper_name ( word* w )
{	
	strcpy ( w -> text, "о" );
	*w -> logical = 15;
	w -> logical [ 1 ] = 0;
	w -> length = 1;

	return w;
}

void validate_globals ( void )
{	
	if ( context_size < 2 )
		fatal_error ( "bad context size specified", 0 );
	
	if ( sensitivity_threshold < 1 || sensitivity_threshold > 1000 )
		fatal_error ( "bad sensitivity threshold specified", 0 );
	
	if ( wordcount_use_coefficient < 0 || wordcount_use_coefficient > 100 )
		fatal_error ( "bad wordcount use coefficient specified", 0 );
	
	if ( !(resume_processing == -1 
	       || resume_processing == 0 
	       || resume_processing == 1) )
		fatal_error ( "cannot determine if resuming is desired", 0 );

	if ( !wordcount_use_coefficient && dump_wordcount )
		fatal_error ( "you must enable wordcount to dump it", 0 );
	
	if ( input_codepage == CYR_CP_UNDEFINED )
		fatal_error ( "invalid input code page specified", 0 );

	if ( output_codepage == CYR_CP_UNDEFINED )
		fatal_error ( "invalid output code page specified", 0 );
	
	if ( !log_path )
		log_path = xstrdup ( "fresheye.log" );
}

/*
 * Does various initialization things.
 */
int init ( void )
{
	int i;

	cyr_set_default_codepage ( CYR_CP_DEFAULT );

	for ( i = 0; i < VOCSIZE; i ++ ) {
		convert_to_logical ( voc [ i ] [ 0 ], voc [ i ] [ 0 ] );
		convert_to_logical ( voc [ i ] [ 1 ], voc [ i ] [ 1 ] );
	}

	twosigmasqr = 2 * sqr (context_size * 4);

	return 0;
}

void cleanup ( void )
{
	free ( log_path );
}

void count_unique_nodes ( avl* t, node* n, void* user_data )
{
	if ( n -> count == 1 )
		(* (size_t *) user_data) ++;
}

void map_node ( avl* tree, node* n, void* data )
{
	node*** ppp = (node ***) data;
	**ppp = n;
	(*ppp) ++;
}

void map_non_unique_node ( avl* tree, node* n, void* data )
{
	node*** ppp = (node ***) data;
	if ( n -> count == 1 )
		return;
	**ppp = n;
	(*ppp) ++;
}

static int cmp_nodes ( const void* a, const void* b )
{
	return (*(const node **) b) -> count - (*(const node **) a) -> count;
}

void do_dump_wordcount ( avl* tree, FILE* f )
{
	node** table = NULL;
	node** pp = NULL;
	size_t table_size = 0;

	assert ( tree );
	assert ( f );
	
	/* Save some memory - try to count only non-unique nodes */
	avl_foreach ( tree, count_unique_nodes, &table_size );
	table_size = tree -> count - table_size ?
		tree -> count - table_size : tree -> count;
	
	pp = table = xmalloc ( sizeof ( node *) * table_size );
	avl_foreach ( tree, tree -> count == table_size ?
		map_node : map_non_unique_node, &pp );
	qsort ( table, table_size, sizeof ( node *), cmp_nodes );
	
	fprintf ( log_file, "=== WORDCOUNT\nWords listed: %u\n", table_size );
	for ( pp = table; pp < table + table_size; pp ++ )
		print_node ( tree, *pp, f );
	fprintf ( log_file, "=== END WORDCOUNT\n\n");
	free ( table );
}

int process_file ( const char* path )
{	
	context* ctx = NULL;
	FILE* f = NULL;
	
	cries = 0;
	ogos = 0;
	first_line = 0;
	yes_to_all = 0;
	
	if ( wordcount_use_coefficient ) {
		f = xfopen ( path, "r" );
		if ( (tree = avl_init ( wordcmp )) == NULL ) 
			fatal_error ( "memory allocation error", 0 );
		if ( !wordcount ( tree, f ) )
			fatal_error ( "memory allocation error", 0 );
		if ( not_all_words_counted )
			fprintf ( stderr, 
				"fe: warning: only %lu words counted\n",
				tree -> count );
		xfclose ( f );
	}

	ctx = ctx_init ( path, context_size, input_codepage );
	
	log_file = xfopen ( log_path, "a" );
	write_log_header ( ctx, log_file );
	
	if ( wordcount_use_coefficient && dump_wordcount )
		do_dump_wordcount ( tree, log_file );
	
	if ( (first_line = check_log ( log_path, path )) != 0 ) {
		int answer = 0;
		if ( resume_processing ) {
			printf ( "!  Файл %s проверен до строки %lu.\n"
				"   Продолжить (Y) или начать сначала (N)? ", 
				path, first_line );
			answer = whatkey ( "NY" );
		}
		if ( answer  == 'Y' )
			ctx_skip_lines ( ctx, first_line + 1 );
		else
			first_line = 0;
	}
	while ( ctx_shift ( ctx ) ) {
		if ( exclude_proper_names && ctx -> cur_word -> proper ) 
			patch_proper_name ( ctx -> cur_word );
		if ( check ( ctx ) )
			break;
	}
	
	write_log_footer ( ctx, log_file );
	xfclose ( log_file );
	
	ctx_free ( ctx );
	if ( wordcount_use_coefficient ) {
		avl_free ( tree );
		tree = NULL;
	}

	return 0;
}

int main ( int argc, char* argv [] )
{
	int argument_index = parse_command_line ( argc, argv );
	
	if ( argument_index >= argc )
		fatal_error (
			"Please specify a file to process (try --help)", 0 );

	validate_globals ();
	init ();
	
	while ( !cancel_processing && argument_index < argc )
		process_file ( argv [ argument_index ++ ] );

	cleanup ();
	return 0;
}
