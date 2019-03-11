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
 * $Id: avl_low.c,v 1.2 2002/06/07 03:45:54 vadimp Exp $
 */

/*
 * Mon Jul 12 18:45:57 IDT 1999, vadimp:
 *
 * This code is taken from Linux kernel 2.2.10 and adopted for the needs of
 * Fresh Eye by Vadim Penzin <penzin@attglobal.net>.
 * Original source (see mm/mmap_avl.c in the Linux kernel source tree) is 
 * written by Bruno Haible <haible@ma2s2.mathematik.uni-karlsruhe.de>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "avl.h"

/*
 * task->mm->mmap_avl is the AVL tree corresponding to task->mm->mmap
 * or, more exactly, its root.
 * A vm_area_struct has the following fields:
 *   left     left son of a tree node
 *   right    right son of a tree node
 *   height   1+max(heightof(left),heightof(right))
 * The empty tree is represented as NULL.
 */

/* Since the trees are balanced, their height will never be large. */
#define AVL_MAX_HEIGHT	41	/* why this? a small exercise */
#define heightof(tree)	((tree) == NULL ? 0 : (tree)->height)
/*
 * Consistency and balancing rules:
 * 1. tree->height == 1+max(heightof(tree->left),heightof(tree->right))
 * 2. abs( heightof(tree->left) - heightof(tree->right) ) <= 1
 * 3. foreach node in tree->left: node->key <= tree->key,
 *    foreach node in tree->right: node->key >= tree->key.
 */

/*
 * Rebalance a tree.
 * After inserting or deleting a node of a tree we have a sequence of subtrees
 * nodes[0]..nodes[k-1] such that
 * nodes[0] is the root and nodes[i+1] = nodes[i]->{left|right}.
 */
static void rebalance (node*** nodeplaces_ptr, int count)
{
	for ( ; count > 0 ; count--) {
		node** nodeplace = *--nodeplaces_ptr;
		node* n = *nodeplace;
		node* nodeleft = n->left;
		node* noderight = n->right;
		int heightleft = heightof(nodeleft);
		int heightright = heightof(noderight);
		if (heightright + 1 < heightleft) {
			/*                                    */
			/*                   *                */
			/*                 /   \              */
			/*              n+2      n            */
			/*                                    */
			node* nodeleftleft = nodeleft->left;
			node* nodeleftright = nodeleft->right;
			int heightleftright = heightof(nodeleftright);
			if (heightof(nodeleftleft) >= heightleftright) {
				/*                                         */
				/*        *                    n+2|n+3     */
				/*      /   \                  /    \      */
				/*   n+2      n      -->      /   n+1|n+2  */
				/*   / \                      |    /    \  */
				/* n+1 n|n+1                 n+1  n|n+1  n */
				/*                                         */
				n->left = nodeleftright; 
				nodeleft->right = n;
				nodeleft->height = 1 + 
					(n->height = 1 + heightleftright);
				*nodeplace = nodeleft;
			} else {
				/*                                      */
				/*       *                     n+2      */
				/*     /   \                 /     \    */
				/*  n+2      n      -->    n+1     n+1  */
				/*  / \                    / \     / \  */
				/* n  n+1                 n   L   R   n */
				/*    / \                               */
				/*   L   R                              */
				/*                                      */
				nodeleft->right = nodeleftright->left;
				n->left = nodeleftright->right;
				nodeleftright->left = nodeleft;
				nodeleftright->right = n;
				nodeleft->height = n->height = heightleftright;
				nodeleftright->height = heightleft;
				*nodeplace = nodeleftright;
			}
		}
		else if (heightleft + 1 < heightright) {
			/* 
			 * similar to the above, 
			 * just interchange 'left' <--> 'right' 
			 */
			node* noderightright = noderight->right;
			node* noderightleft = noderight->left;
			int heightrightleft = heightof(noderightleft);
			if (heightof(noderightright) >= heightrightleft) {
				n->right = noderightleft; 
				noderight->left = n;
				noderight->height = 1 + 
					(n->height = 1 + heightrightleft);
				*nodeplace = noderight;
			} else {
				noderight->left = noderightleft->right;
				n->right = noderightleft->left;
				noderightleft->right = noderight;
				noderightleft->left = n;
				noderight->height = 
					n->height = heightrightleft;
				noderightleft->height = heightright;
				*nodeplace = noderightleft;
			}
		}
		else {
			int height = (heightleft<heightright ? 
					heightright : heightleft) + 1;
			if (height == n->height)
				break;
			n->height = height;
		}
	}
}

/* Insert a node into a tree. */
void insert (node* new_node, node** ptree, cmpfn cmp )
{
	key_t key = new_node->key;
	int len = new_node -> length;
	node** nodeplace = ptree;
	node** stack[AVL_MAX_HEIGHT];
	int stack_count = 0;
	node*** stack_ptr = &stack[0]; /* = &stack[stackcount] */
	
	for (;;) {
		node* n = *nodeplace;
		if (n == NULL)
			break;
		*stack_ptr++ = nodeplace; stack_count++;
		if ( cmp ( key, len, n->key, n -> length ) < 0 )
			nodeplace = &n->left;
		else
			nodeplace = &n->right;
	}
	new_node->left = NULL;
	new_node->right = NULL;
	new_node->height = 1;
	*nodeplace = new_node;
	rebalance(stack_ptr,stack_count);
}

#if 0

/* 
 * We do not need delete functionality in Fresh Eye, keep it here
 * "for completeness".
 */

/* Removes a node out of a tree. */
void delete (node* node_to_delete, node** ptree, cmpfn cmp )
{
	key_t key = node_to_delete->key;
	int len = node_to_delete -> length;
	node** nodeplace = ptree;
	node** stack[AVL_MAX_HEIGHT];
	int stack_count = 0;
	node*** stack_ptr = &stack[0]; /* = &stack[stackcount] */
	node** nodeplace_to_delete;
	
	for (;;) {
		node* n = *nodeplace;
		assert ( n ); /* node_to_delete must be found */
		*stack_ptr++ = nodeplace; stack_count++;
		if ( !cmp ( key, len, n->key, n -> length ) )
			break;
		if ( cmp ( key, len, n->key, n -> length ) < 0 )
			nodeplace = &n->left;
		else
			nodeplace = &n->right;
	}
	nodeplace_to_delete = nodeplace;
	/* Have to t emove node_to_delete = *nodeplace_to_delete. */
	if (node_to_delete->left == NULL) {
		*nodeplace_to_delete = node_to_delete->right;
		stack_ptr--; stack_count--;
	} else {
		node*** stack_ptr_to_delete = stack_ptr;
		node** nodeplace = &node_to_delete->left;
		node* n;
		for (;;) {
			n = *nodeplace;
			if (n->right == NULL)
				break;
			*stack_ptr++ = nodeplace; stack_count++;
			nodeplace = &n->right;
		}
		*nodeplace = n->left;
		/* n replaces node_to_delete */
		n->left = node_to_delete->left;
		n->right = node_to_delete->right;
		n->height = node_to_delete->height;
		*nodeplace_to_delete = n; /* replace node_to_delete */
		*stack_ptr_to_delete = &n->left; /* replace &node_to_delete->left */
	}
	rebalance(stack_ptr,stack_count);
}

#endif
