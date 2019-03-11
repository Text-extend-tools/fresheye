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
 * $Id: context.h,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

/*
 * Context is rather a complex object. Its main goal is to maintain a
 * "sliding window" on the input file. This window has a user-defined size 
 * measured in the number of words that can be seen through it.
 * 
 * Basically context consists of the following:
 * 
 * 	- List of words -- it stores the words that user is able to see through
 * 	  the "sliding window" at any instant of context's life time. 
 * 	  Physically it is a simple FIFO queue implemented as a linked list 
 * 	  of 'word' structures.
 * 	  
 * 	- List of lines -- it stores the lines of input file where the words
 *	  of context (both those found in the list of words and the current 
 *	  word) appear. When a line doesn't contain words but there are words
 *	  (currently available through the context) before and/or after it, the
 *	  line is also kept in this list. Similarly, list of lines is a FIFO
 *	  queue implemented as a linked list of 'line' structures.
 *	  
 * 	- Current word - one 'word' structure representing the current word.
 * 
 * Each 'word' structure points to a 'line' structure so the user can determine
 * the origin of a word. Each 'line' structure contains reference counter 
 * showing the number of words pointing to a specific line. 
 * 
 * The only operation supported by the context is shifting. When context is 
 * being shifted, current word (if any) is inserted at the bottom of the word 
 * list and the next word found in the input file becomes the current. If 
 * context's capacity is exceeded during this operation, the topmost word is 
 * removed from the word list and reference counter of the corresponding line 
 * is decremented. Unreferenced lines (those having reference counter equal to 
 * zero) are *not* removed from the list of lines until they are at the top of 
 * it. Besides shifting and handling words-lines relationship, context does
 * a lot of work by computing everything that context's user may (or may not :)
 * need -- lengths of words and lines, position of words in lines etc. 
 */

typedef struct line {
	char* text; /* Character data as it appears in the input file */
	unsigned long number; /* Line number in the input file */
	int length; /* Line length */
	int refcnt; /* Reference count */
	struct line* next; /* Next line in the list */
} line;

typedef struct word {
	line* line; /* Line on which the word appears */
	char* text; /* The word in unified representation */
	char* logical; /* The word in logical representation */
	int position; /* Start position in the line */
	int length; /* Length of the word */
	int delimiters; /* Count of delimiters coming after this word */
	int proper; /* Proper name */
	struct word* next; /* Next word in the list */
} word;

typedef struct {
	
	int width; /* Maximal number of words in the context */
	
	line* lines_head; /* Head of the list of lines */
	line* lines_tail; /* Tail of the list of lines */
	int line_cnt; /* Count of entries in the list of lines */
	
	struct word* words_head; /* Head of the list of context's words */
	struct word* words_tail; /* Tail of the list of context's words */
	int word_cnt; /* Count of words in the words' list */

	unsigned long total_lines; /* Total line count */
	unsigned long total_words; /* Total word count */
	
	const char* path;
	FILE* f; /* Input file */
	reader* rdr; /* Line reader */
	
	struct word* cur_word; /* Current word */
	const char* cp; /* Current position in the current line */

	/*
	 * Various state flags and counters of context's automaton.
	 * This is taken as is from fe 1.2.1, to ensure that the
	 * functionality remains the same, probably it should be rewritten.
	 */
	int new_sentence; /* Looking for a new sentence */ 
	int immediate_new; /* Immediate speech */
	int proper; /* Proper name was detected */
	int spaces; /* 'Spaces' mode -- running spaces counted as one space */
	int par; /* Last line(s) contain(s) no words */
	int delimiters; /* Delimiters' counter. Passed on to word */
	
} context;

/*
 * Allocates and initializes a new context with the path to the input file and 
 * context width -- maximal number of the words in context.
 * Returns pointer to the new context if successful, otherwise aborts.
 */
context* ctx_init ( const char* path, const int width, int codepage );

/*
 * Frees memory allocated for context
 */
void ctx_free ( context* this );

/*
 * Shifts context.
 * Returns this if successful, otherwise (EOF) NULL.
 */
context* ctx_shift ( context* this );

/*
 * Skips lines in input file.
 * Returns this if successful, otherwise (EOF) NULL.
 */
context* ctx_skip_lines ( context* this, unsigned long n );
