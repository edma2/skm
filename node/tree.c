/* node - data structure implementations
 * author: Eugene Ma (edmaa2) */
#include "node.h"

static int treecopy_helper(Tree *old, Tree *new);

/* return a new Tree */
Tree *
treenew(void *data) {
	Tree *t;

	if ((t = malloc(sizeof(Tree))) == NULL)
		return NULL;

	if ((t->children = listnew()) == NULL)
		return NULL;
	t->data = data; 
	t->parent = NULL;

	return t;
}

/* set the data */
void 
treesetdatum(Tree *t, void *data) {
	if (t == NULL)
		return;
	t->data = data;
}

/* remove a tree from its parent */
void 
treeremove(Tree *t) {
	/* remove linkages if needed */
	if (t->parent != NULL)
		listremove(t->parent->children, t);
	treefree(t);
}

/* branch out at parent tree, return child) */
Tree *
treeaddchild(Tree *parent, void *data) {
	Tree *t = treenew(data);

	if (t == NULL || parent == NULL)
		return NULL;
	/* connect parent */
	t->parent = parent;
	/* add to list */
	if (listappend(parent->children, t) == NULL)
		return NULL;

	return t;
}

/* add a sibling */
Tree * 
treeaddsib(Tree *t, void *data) {
	if (t == NULL || t->parent == NULL)
		return NULL;

	return treeaddchild(t->parent, data);
}

/* return the parent */
Tree *
treeparent(Tree *t) {
	if (t == NULL)
		return NULL;
	return t->parent;
}

/* return the first child of parent */
Tree *
treefirstchild(Tree *parent) {
	if (parent == NULL || listsize(parent->children) == 0)
		return NULL;
	return listfirst(parent->children)->data;
}

/* return the next tree on the same level */
Tree *
treenext(Tree *t) {
	Node *p;

	if (t == NULL || t->parent == NULL)
		return NULL;
	/* get the pointer to the first node */
	for (p = listfirst(t->parent->children); p != NULL; p = p->next) {
		if (p->data == t)
			break;
	}
	if (p == NULL || p->next == NULL)
		return NULL;
	return p->next->data;
}

/* depth first postorder traversal */
void
treetraverse(Tree *t, void (*func)(Tree *t)) {
	Node *p, *tmp;

	if (t == NULL)
		return;
	/* visit children */
	for (p = listfirst(t->children); p != NULL; p = tmp) {
		/* save next node in case we remove it */
		tmp = p->next;
		treetraverse((Tree *)p->data, func);
	}
	/* visit current tree */
	(*func)(t);
}

/* free data only */
void 
treefreedata(Tree *t) {
	free(t->data);
}

/* is this tree a leaf? */
int 
treeisleaf(Tree *t) {
	if (t == NULL)
		return -1;
	return (listsize(t->children) == 0);
}

/* return the tree whose data matches (dfs) */
Tree *
treesearch(Tree *haystack, void *needle, int (*search)(void *h, void *n)) {
	Node *p;
	Tree *tmp;

	if (haystack == NULL || needle == NULL)
		return NULL;

	/* visit current data */
	if ((*search)(haystack->data, needle) == 0)
		return haystack;
	/* visit children */
	for (p = listfirst(haystack->children); p != NULL; p = p->next) {
		if ((tmp = treesearch((Tree *)p->data, needle, search)) != NULL)
			return tmp;
	}

	/* nothing found */
	return NULL;
}

/* clone a tree's nodes */
Tree *treecopy(Tree *orig) {
	Tree *clone;

	if (orig == NULL)
		return NULL;
	if ((clone = treenew(orig->data)) == NULL)
		return NULL;
	if (treecopy_helper(orig, clone) < 0) {
		treefree(clone);
		return NULL;
	}

	return clone;
}

/* orig is the tree whose children is to be added to tree new */
static int treecopy_helper(Tree *old, Tree *new) {
	Node *oldp;
	Tree *newp;

	if (old == NULL || new == NULL)
		return 0;

	/* add all children */
	for (oldp = listfirst(old->children); oldp != NULL; oldp = oldp->next) {
		/* throw malloc error */
		if ((newp = treeaddchild(new, ((Tree *)oldp->data)->data)) == NULL)
			return -1;
		/* do this recursively for the new child as well */
		if (treecopy_helper((Tree *)oldp->data, newp) < 0)
			return -1;
	}

	return 0;
}

/* 0 if match found */
int 
treesearchstring(void *data, void *str) {
	if (data == NULL || str == NULL)
		return -1;
	return strcmp((char *)data, (char *)str);
}

/* recursively free all the nodes in a tree */
void 
treefree(void *t) {
	List *children;

	if (t == NULL)
		return;
	children = ((Tree *)t)->children;
	listtraverse(children, treefree);
	listfree(children);
	free(t);
}

/* print a tree recursively, revealing its heiarchal structure */
void 
treeprint(Tree *t, int depth) {
	Node *p;
	int d;

	if (t == NULL) {
		printf("MISSING TREE\n");
		return;
	}

	for (d = depth; d > 0; d--)
		printf("- ");
	printf("\\");
	if (t->data == NULL)
		printf("NULL\n");
	else
		printf("%s\n", (char *)(t->data));
	for (p = t->children->head; p != NULL; p = p->next)
		treeprint((Tree *)p->data, depth + 1);
}
