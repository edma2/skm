/* Tree data structure : 
 * Each struct Tree represents a node in the heirarchy
 * the children field is a List of nodes, each of which
 * point to a tree.
 * A particular tree's child tree can be accessed by getting
 * its first child and getting next the next child repeatedly.
 */
#include "datastructures.h"

struct Tree {
	void *datum;				/* current datum */
	List *children;				/* list of child trees */
	struct Tree *parent;			/* pointer to parent tree */
};
typedef struct Tree Tree;

Tree *Tree_new(void *data);			/* return an orphaned tree */
Tree *Tree_parent(Tree *t);			/* return parent */
Tree *Tree_first_child(Tree *parent);		/* return first child */
Tree *Tree_next_sibling(Tree *t); 		/* return next sibling */
Tree *Tree_add_child(Tree *parent, void *data); /* add a child to parent */
Tree *Tree_add_sibling(Tree *t, void *data);	/* add sibling */
Tree *Tree_find(Tree *haystack, void *needle, int (*search)(void *h, void *n));
int Tree_search_string(void *datum, void *str); /* example search function */
Tree *Tree_clone(Tree *orig);
int Tree_clone_helper(Tree *orig, Tree *new);
void Tree_traverse(Tree *t, void (*func)(void *datum));
void Tree_set(Tree *t, void *data); 		/* t->datum = data */
void Tree_free(Tree *t);			/* recursively free a tree */
void Tree_free_data(Tree *t);
void Tree_free_nodes(Tree *t);
void Tree_print(Tree *t, int depth);    	/* print an informative diagram */

