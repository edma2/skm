#include <stdio.h> /* for printf in List_print() */
#include <string.h>
#include <stdlib.h>

struct Node {
	void *data;
	struct Node *next; 
};
typedef struct Node Node;
struct List {
	Node *head;			/* first item in list */ 
	int length;			/* length */
};
typedef struct List List;
struct Tree {
	void *datum;				/* current datum */
	List *children;				/* list of child trees */
	struct Tree *parent;			/* pointer to parent tree */
};
typedef struct Tree Tree;

/* node */
Node *Node_insert(Node *p, void *data); 	/* insert after node p */
inline Node *Node_create(void *data); 		/* create unconnected node */
void Node_remove(Node *head, Node *n); 		/* remove and free node pointed by n head && n != NULL */

/* list */
List *List_new(void); 					/* instantiate empty list (head points to NULL) */
List *List_clone(List *lst);				/* clone an existing list */
Node *List_last(List *lst); 				/* return the last Node in List */
Node *List_first(List *lst);				/* return the first Node in List */
Node *List_append(List *lst, void *data);		/* add to back */
Node *List_prepend(List *lst, void *data);		/* add to front */
void free_data(void *data);				/* free data */
void List_remove_first(List *lst); 			/* remove and free the first node */
void List_remove_last(List *lst);			/* remove and free the last node */
void List_free(List *lst);				/* remove and free all nodes and data */
void List_free_data(List *lst);				/* free all data */
void List_free_nodes(List *lst);			/* remove and free all nodes */
void List_print(List *lst); 				/* debug utility */
int List_string_match(void *data, void *str);		/* return 0 if data matches string */
void List_traverse(List *lst, void (*func)(void *data));/* traverse list from front to back */
Node *List_search(List *haystack, void *needle, int (*search)(void *h, void *n)); 
							/* return ptr when seach function returns 0 */

/* tree */
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
