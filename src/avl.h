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
 * $Id: avl.h,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

#define key_t char*
#define AVL_MAX_HEIGHT	41	/* why this? a small exercise */
#define heightof(tree)	((tree) == NULL ? 0 : (tree)->height)

typedef int (*cmpfn) ( const key_t, int, const key_t, int );

typedef struct node {
	key_t key;
	int length;
	unsigned long count;
	unsigned long coefficient;
	struct node* left;
	struct node* right;
	unsigned char height;
} node;

typedef struct avl {
	node* root;
	unsigned long count;
	cmpfn cmp;
} avl;


/* Public interface -- avl.c */
avl* avl_init ( cmpfn cmp );
void avl_free ( avl* this );
node* avl_insert ( avl* this, const char* s, int len );
node* avl_lookup ( avl* this, const char* s, int len );
void avl_print ( avl* this, FILE* f );
void avl_foreach ( avl* this, void (*func) ( avl*, node*, void* ), 
		void* user_data );

/* Low-level AVL functions -- avl_low.c */	
void insert (node* new_node, node** ptree, cmpfn cmp );
void delete (node* node_to_delete, node** ptree, cmpfn cmp );
