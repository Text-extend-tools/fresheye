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
 * $Id: avl.c,v 1.1.1.1 2000/10/17 01:16:56 vadimp Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "avl.h"

node* node_init ( const char* s, int len ) {
	
	node* this = NULL;

	assert ( s );
	
	if ( (this = (node *) malloc ( sizeof ( node ) )) == NULL )
		return NULL;
	
	if ( (this -> key = strdup ( s )) == NULL ) {
		free ( this );
		return NULL;
	}
	
	this -> length = len;
	this -> right = this -> left = NULL;
	this -> height = 1;
	this -> count = 1;
	this -> coefficient = 0;

	return this;
}

void node_free ( node* this ) {
	if ( this ) {
		if ( this -> key )
			free ( (node *) this -> key );
		node_free ( this -> left );
		node_free ( this -> right );
		free ( this );
	}
}

void node_print ( node* n, FILE* f ) {
	if ( n ) {
		node_print ( n -> left, f );
		fprintf ( f, "%s\t%lu\n", n -> key, n -> count );
		node_print ( n -> right, f );
	}
}

avl* avl_init ( cmpfn cmp ) {

	avl* this = NULL;

	assert ( cmp );
	
	if ( (this = (avl *) malloc ( sizeof ( avl ) )) == NULL )
		return NULL;

	this -> root = NULL;
	this -> count = 0;
	this -> cmp = cmp;

	return  this;
}

void avl_free ( avl* this ) {
	
	assert ( this );
	
	node_free ( this -> root );
	free ( this );	
}

static node* lookup ( node* n, const key_t key, int len, cmpfn cmp ) {
	
	while ( n ) {

		int cmpres = cmp ( key, len, n -> key, n -> length );
		if ( !cmpres )
			break;
		n = cmpres < 0 ? n -> left : n -> right;
	}

	return n;
}

node* avl_insert ( avl* this, const char* s, int len ) {
	
	node* new_node = NULL;
	
	assert ( this );
	assert ( s );

	if ( (new_node = node_init ( s, len )) == NULL )
		return NULL;
	
	insert ( new_node, &this -> root, this -> cmp );
	this -> count ++;

	return new_node;
}

node* avl_lookup ( avl* this, const char* s, int len ) {
	
	assert ( this );
	assert ( this -> cmp );
	assert ( s );
	
	return lookup ( this -> root, s, len, this -> cmp );
}

void avl_print ( avl* this, FILE* f ) {
	assert ( this );
	node_print ( this -> root, f );
}

void node_foreach ( avl* t, node* n, void (*func) ( avl*, node*, void * ),
		void* user_data ) {
  if ( n )
    {
      node_foreach ( t, n -> left, func, user_data );
      func ( t, n, user_data );
      node_foreach ( t, n -> right, func, user_data );
    }
}

void avl_foreach ( avl* this, void (*func) ( avl*, node*, void* ), 
		void* user_data ) {

  assert ( this );
  node_foreach ( this, this -> root, func, user_data );
}
