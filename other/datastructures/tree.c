/* Uses node and list api */

#include "datastructures.h"

Tree *Tree_new(void *data) {
	Tree *t = malloc(sizeof(Tree));

	if ((t = malloc(sizeof(Tree))) == NULL)
		return NULL;
	if ((t->children = List_new()) == NULL)
		return NULL;
	t->datum = data; 
	t->parent = NULL;

	return t;
}

/* set the data */
void Tree_set(Tree *t, void *data) {
	if (t == NULL)
		return;
	t->datum = data;
}

/* branch out at parent tree (add child) */
Tree *Tree_add_child(Tree *parent, void *data) {
	Tree *t = Tree_new(data);

	if (t == NULL || parent == NULL)
		return NULL;
	/* connect parent */
	t->parent = parent;
	/* add to list */
	if (List_append(parent->children, t) == NULL)
		return NULL;

	return t;
}

/* add a sibling */
Tree *Tree_add_sibling(Tree *t, void *data) {
	if (t == NULL || t->parent == NULL)
		return NULL;

	return Tree_add_child(t->parent, data);
}

/* return the parent */
Tree *Tree_parent(Tree *t) {
	if (t == NULL)
		return NULL;
	return t->parent;
}

/* return the first child of parent */
Tree *Tree_first_child(Tree *parent) {
	if (parent == NULL)
		return NULL;
	return List_first(parent->children)->data;
}

/* return the next tree on the same level */
Tree *Tree_next_sibling(Tree *t) {
	Node *ptr;

	if (t == NULL || t->parent == NULL)
		return NULL;
	/* get the pointer to the first node */
	for (ptr = List_first(t->parent->children); ptr != NULL; ptr = ptr->next) {
		if (ptr->data == t)
			break;
	}
	if (ptr == NULL || ptr->next == NULL)
		return NULL;
	return ptr->next->data;
}

/* depth first preorder traversal */
void Tree_traverse(Tree *t, void (*func)(void *datum)) {
	Node *ptr;

	if (t == NULL)
		return;
	/* visit current node */
	(*func)(t->datum);
	/* visit children */
	for (ptr = List_first(t->children); ptr != NULL; ptr = ptr->next)
		Tree_traverse((Tree *)ptr->data, func);
}

/* return the tree whose datum matches (dfs) */
Tree *Tree_find(Tree *haystack, void *needle, int (*search)(void *h, void *n)) {
	Node *ptr;
	Tree *tmp;

	if (haystack == NULL || needle == NULL)
		return NULL;

	/* visit current datum */
	if ((*search)(haystack->datum, needle) == 0)
		return haystack;
	/* visit children */
	for (ptr = List_first(haystack->children); ptr != NULL; ptr = ptr->next) {
		if ((tmp = Tree_find((Tree *)ptr->data, needle, search)) != NULL)
			return tmp;
	}

	/* nothing found */
	return NULL;
}

/* clone a tree's nodes */
Tree *Tree_clone(Tree *orig) {
	Tree *clone;

	if (orig == NULL)
		return NULL;
	if ((clone = Tree_new(orig->datum)) == NULL)
		return NULL;
	if (Tree_clone_helper(orig, clone) < 0) {
		Tree_free_nodes(clone);
		return NULL;
	}

	return clone;
}

/* orig is the tree whose children is to be added to tree new */
int Tree_clone_helper(Tree *old, Tree *new) {
	Node *oldptr;
	Tree *newptr;

	if (old == NULL || new == NULL)
		return 0;

	/* add all children */
	for (oldptr = List_first(old->children); oldptr != NULL; oldptr = oldptr->next) {
		/* throw malloc error */
		if ((newptr = Tree_add_child(new, ((Tree *)oldptr->data)->datum)) == NULL)
			return -1;
		/* do this recursively for the new child as well */
		if (Tree_clone_helper((Tree *)oldptr->data, newptr) < 0)
			return -1;
	}

	return 0;
}


/* 0 if match found */
int Tree_search_string(void *datum, void *str) {
	if (datum == NULL || str == NULL)
		return -1;
	return strcmp((char *)datum, (char *)str);
}

/* directly free all the datum in a tree */
void Tree_free_data(Tree *t) {
	/* free all the datum in t */
	Tree_traverse(t, free_data);
}

/* recursively free all the nodes in a tree */
void Tree_free_nodes(Tree *t) {
	Node *ptr;

	if (t == NULL)
		return;
	/* call this recursively on its children */
	for (ptr = List_first(t->children); ptr != NULL; ptr = ptr->next)
		Tree_free_nodes((Tree *)ptr->data);
	/* free children and self */
	List_free_nodes(t->children);
	free(t);
}

void Tree_free(Tree *t) {
	Tree_free_data(t);
	Tree_free_nodes(t);
}

/* print a tree recursively, revealing its heiarchal structure */
void Tree_print(Tree *t, int depth) {
	Node *ptr;
	int d;

	if (t == NULL)
		return;

	for (d = depth; d > 0; d--)
		printf("- ");
	printf("\\");
	if (t->datum == NULL)
		printf("NULL\n");
	else
		printf("%s\n", (char *)(t->datum));
	for (ptr = t->children->head; ptr != NULL; ptr = ptr->next)
		Tree_print((Tree *)ptr->data, depth + 1);
}
