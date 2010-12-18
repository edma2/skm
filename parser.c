/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

#include "parser.h"

#define STATE_BEGIN 		0
#define STATE_OPEN_PAREN 	1
#define STATE_CLOSE_PAREN 	2
#define STATE_PROC 		3
#define STATE_ARG 		4
#define STATE_END 		5
#define STATE_ERROR 		6
#define MAX_WORD 		20
#define TYPE_PROC 		0
#define TYPE_ARG 		1

static Tree *branchup(Tree *t);
static Tree *branchdown(Tree *t);
static int addword(Tree *t, void *data, int type);
static int expr_copy_helper(Expr *old, Expr *new);

/* copy elements of exp buffer into a tree */
Expr *parse(char *exp) {
	Tree *root;
	int state = STATE_BEGIN;	
	int layer = 0;
	char *ptr = exp;
	char buf[MAX_WORD];
	int i = 0; 		

	/* check if malloc succeeded */
	if ((root = treenew(NULL)) == NULL)
		return NULL;
	/* initialize buffer */
	memset(buf, 0, MAX_WORD);
	state = STATE_BEGIN;
	do {
		if (state == STATE_BEGIN) {
			/* go to first open paren */
			if (*ptr == '(') {
				layer++;
				state = STATE_OPEN_PAREN;
			}
		} else if (state == STATE_OPEN_PAREN) {
			if (*ptr == ')') {
				/* experimental: empty lists */
				layer--;
				root = branchdown(root);
				state = STATE_CLOSE_PAREN;
			//	state = STATE_ERROR;
			//	printf("error: premature closing paren\n");
			} else if (*ptr == '(') {
				layer++;
				root = branchup(root);
				state = STATE_OPEN_PAREN;
			} else if (*ptr != ' ') {
				buf[i++] = *ptr;
				state = STATE_PROC;
			}
		} else if (state == STATE_CLOSE_PAREN) {
			if (layer <= 0) {
				state = STATE_ERROR;
				printf("error: excess close paren\n");
			} else if (*ptr == ')') {
				/* go to next buf */
				layer--;
				root = branchdown(root);
				state = STATE_CLOSE_PAREN;
			} else if (*ptr == '(') {
				layer++;
				root = branchup(root);
				state = STATE_OPEN_PAREN;
			} else if (*ptr != ' ') {
				buf[i++] = *ptr;
				state = STATE_ARG;
			}
		} else if (state == STATE_PROC || STATE_ARG) {
			if (buf[0] != '\0' && (*ptr == ' ' || *ptr == ')')) {
				/* terminate buf */
				buf[i] = '\0';
				if (state == STATE_PROC) {
					/* malloc() failed somewhere */
					if (addword(root, strdup(buf), TYPE_PROC) < 0) {
						state = STATE_ERROR;
						printf("error: memory error\n");
					}
				} else {
					if (addword(root, strdup(buf), TYPE_ARG) < 0) {
						state = STATE_ERROR;
						printf("error: memory error\n");
					}
				}
				/* reset buf */
				buf[0] = '\0';
				i = 0;
				/* read next buf or close paren */
				state = STATE_ARG;
			}
			if (*ptr == ')') {
				layer--;
				root = branchdown(root);
				state = STATE_CLOSE_PAREN;
			} else if (*ptr == '(') {
				layer++;
				root = branchup(root);
				state = STATE_OPEN_PAREN;
			} else if (*ptr != ' ')
				buf[i++] = *ptr;
		}
		ptr++;
	} while (*ptr != '\0' && state != STATE_ERROR);

	/* not a function call, return value instead */
	if (state != STATE_ERROR && state == STATE_BEGIN) {
		/* cut out spaces */
		for (ptr = exp; *ptr != '\0'; ptr++) {
			if (*ptr == ' ') {
				*ptr = '\0';
				break;
			}
		}
		treesetdatum(root, strdup(exp));
	} else if (state != STATE_ERROR && layer > 0) {
		printf("error: excess open paren\n");
		state = STATE_ERROR;
	} 
	if (state == STATE_ERROR) {
		expr_free(root);
		root = NULL;
	}

	return root;
}

/* return the next word or sub-expression in the expression */
Expr *expr_next(Expr *expr) {
	if (expr == NULL)
		return NULL;
	return treenext(expr);
}

/* return the first word or sub-expression of the parent expression */
Expr *expr_subexpr(Expr *parent) {
	if (parent == NULL)
		return NULL;
	/* if the expression is just a single word
	 * return null */
	if (expr_isword(parent))
		return NULL;
	return treefirstchild(parent);
}

/* if the expression consists of a single
 * root node pointing to a string it is a word */
int expr_isword(Expr *expr) {
	/* we need this here because an empty list is
	 * also a tree, so we check if it points to a word */
	if (expr_getword(expr) != NULL)
		return treeisleaf(expr);
	return 0;
}

/* if it's not a word, its a list */
int expr_islist(Expr *expr) {
	return !expr_isword(expr);
}

/* if it's an empty list then the expression will be 
 * a single root with a NULL pointer to its data */
int expr_isemptylist(Expr *expr) {
	if (expr_getword(expr) == NULL)
		return treeisleaf(expr);
	return 0;
}

/* return the word pointed to by the datum of expr */
char *expr_getword(Expr *expr) {
	if (expr == NULL)
		return NULL;
	return (char *)expr->data;
}

/* return the number of words or sub-expressions in the expression */
int expr_len(Expr *expr) {
	if (expr == NULL)
		return -1;
        if (expr_isword(expr))
                return -1;
	return listsize(expr->children);
}

/* copy the expression tree as well as allocate duplicated strings */
Expr *expr_copy(Expr *orig) {
	Tree *clone;

	if (orig == NULL)
		return NULL;
        if (expr_isword(orig))
                return treenew(strdup((char *)orig->data));
	if ((clone = treenew(NULL)) == NULL)
		return NULL;
	if (expr_copy_helper(orig, clone) < 0) {
		treefree(clone);
		return NULL;
	}

	return clone;
}

/* free an expression */
void expr_free(Expr *e) {
	treetraverse(e, treefreedata);
	treefree(e);
}

/* helper for cloneexpr */
static int expr_copy_helper(Expr *old, Expr *new) {
	Node *oldp;
	Expr *newp;
	char *word;

	if (old == NULL || new == NULL)
		return 0;

	/* add all children */
	for (oldp = listfirst(old->children); oldp != NULL; oldp = oldp->next) {
		word = (char *)((Expr *)oldp->data)->data;
		if (word != NULL)
			word = strdup(word);
		/* throw malloc error */
		if ((newp = treeaddchild(new, word)) == NULL)
			return -1;
		/* do this recursively for the new child as well */
		if (expr_copy_helper((Expr *)oldp->data, newp) < 0)
			return -1;
	}

	return 0;
}

/* go up a level */
static Tree *branchup(Tree *t) {
	return treeaddchild(t, NULL);
}

/* go down a level */
static Tree *branchdown(Tree *t) {
	if (t->parent == NULL)
		return t;
	return treeparent(t);
}

/* add a procedure or argument */
static int addword(Tree *t, void *data, int type) {
	if (treeaddchild(t, data) == NULL)
		return -1;
	return 0;
}
