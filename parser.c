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

static Tree *up(Tree *t);
static Tree *down(Tree *t);
static int expr_insert_word(Tree *t, void *data, int type);
static void expr_copy_helper(Expr *copy);
static void expr_free_helper(Expr *e);

/* copy elements of exp buffer into a tree */
Expr *parse(char *exp) {
	Tree *root;
	int state = STATE_BEGIN;	
	int layer = 0;
	char *ptr = exp;
	char buf[MAX_WORD];
	int i = 0; 		

	/* check if malloc succeeded */
	if ((root = tree_new(NULL)) == NULL)
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
				layer--;
				root = down(root);
				state = STATE_CLOSE_PAREN;
			} else if (*ptr == '(') {
				layer++;
				root = up(root);
				state = STATE_OPEN_PAREN;
			} else if (*ptr != ' ') {
				buf[i++] = *ptr;
				state = STATE_PROC;
			}
		} else if (state == STATE_CLOSE_PAREN) {
			if (layer <= 0) {
				state = STATE_ERROR;
				printf("error: mismatched parens\n");
			} else if (*ptr == ')') {
				/* go to next buf */
				layer--;
				root = down(root);
				state = STATE_CLOSE_PAREN;
			} else if (*ptr == '(') {
				layer++;
				root = up(root);
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
					if (expr_insert_word(root, strdup(buf), TYPE_PROC) < 0) {
						state = STATE_ERROR;
						printf("error: memory error\n");
					}
				} else {
					if (expr_insert_word(root, strdup(buf), TYPE_ARG) < 0) {
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
				root = down(root);
				state = STATE_CLOSE_PAREN;
			} else if (*ptr == '(') {
				layer++;
				root = up(root);
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
		tree_set_data(root, strdup(exp));
	} else if (state != STATE_ERROR && layer > 0) {
		printf("error: mismatched parens\n");
		state = STATE_ERROR;
	} 
	if (state == STATE_ERROR) {
		expr_free(root);
		root = NULL;
	}

	return root;
}

/* go up a level */
static Tree *up(Tree *t) {
	return tree_insert_child(t, NULL);
}

/* go down a level */
static Tree *down(Tree *t) {
	if (t->parent == NULL)
		return t;
	return tree_parent(t);
}

/* add a procedure or argument */
static int expr_insert_word(Tree *t, void *data, int type) {
	if (tree_insert_child(t, data) == NULL)
		return -1;
	return 0;
}

/************************************************/
/****************   Expr API  *******************/
/************************************************/

/* return the next word or sub-expression in the expression */
Expr *expr_next(Expr *expr) {
	if (expr == NULL)
		return NULL;
	return tree_next(expr);
}

/* return the first word or sub-expression of the parent expression */
Expr *expr_child(Expr *parent) {
	if (parent == NULL)
		return NULL;
	/* if the expression is just a single word
	 * return null */
	if (expr_is_word(parent))
		return NULL;
	return tree_child(parent);
}

/* if the expression consists of a single
 * root node pointing to a string it is a word */
int expr_is_word(Expr *expr) {
	/* we need this here because an empty list is
	 * also a tree, so we check if it points to a word */
	if (expr_get_word(expr) != NULL)
		return tree_is_leaf(expr);
	return 0;
}

/* if it's not a word, its a list and contains
   a NULL root tree with one or more children trees */
int expr_is_list(Expr *expr) {
	return !expr_is_word(expr);
}

/* if it's an empty list then the expression will be 
 * a single root with a NULL pointer to its data */
int expr_is_emptylist(Expr *expr) {
	if (expr_get_word(expr) == NULL)
		return tree_is_leaf(expr);
	return 0;
}

/* return the word pointed to by the datum of expr */
char *expr_get_word(Expr *expr) {
	if (expr == NULL)
		return NULL;
	return (char *)expr->data;
}

/* return the number of words or sub-expressions in the expression */
int expr_len(Expr *expr) {
	if (expr == NULL)
		return -1;
        if (expr_is_word(expr))
                return -1;
	return tree_count_children(expr);
}

Expr *expr_copy(Expr *orig) {
        Expr *copy;

        if (orig == NULL)
                return NULL;
        copy = tree_copy(orig);
        tree_traverse(copy, expr_copy_helper);

        return copy;
}

/* replace each data with a newly allocated string */
static void expr_copy_helper(Expr *copy) {
        if (copy->data == NULL)
                return;
        copy->data = strdup((char *)copy->data);
}

/* free an expression */
void expr_free(Expr *e) {
	tree_traverse(e, expr_free_helper);
	tree_free(e);
}

/* wrapper */
static void expr_free_helper(Expr *e) {
        free(e->data);
}
