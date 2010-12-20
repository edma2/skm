#include "skm.h"

/* registers */
float reg_a = 0;
float reg_b = 0;
float reg_c = 0;
float reg_ans = 0;

/* store result in result, tree is preserved */
int eval(Expression *e, float *result) {
	Tree *tptr = Tree_first_child(e);
	Node *ptr;			
	List *arglist;		/* store argument list */
	char *proc; 		/* point to our procedure */
	float *n;		/* store our argument values so we can pass it to append() */

	/* check if malloc succeeded */
	if ((arglist = List_new()) == NULL) {
		printf("error: memory error\n");
		return EVAL_ERR;
	}
	/* fetch the procedure (string) */
	proc = tptr->datum;
	/* fetch arguments (floats) */
	for (tptr = Tree_next_sibling(tptr); tptr != NULL; tptr = Tree_next_sibling(tptr)) {
		if ((n = malloc(sizeof(float))) == NULL) {
			printf("error: memory error\n");
			return EVAL_ERR;
		}
		/* find real value if needed */
		if (tptr->datum == NULL) {
			if (eval(tptr, result) == EVAL_ERR)
				return EVAL_ERR;
			*n = *result;
		}
		else {
			/* fetch register */
			if (!(strcmp(tptr->datum, "a")))
				*n = reg_a;
			else if (!(strcmp(tptr->datum, "b")))
				*n = reg_b;
			else if (!(strcmp(tptr->datum, "c")))
				*n = reg_c;
			else if (!(strcmp(tptr->datum, "ans")))
				*n = reg_ans;
			else
				*n = atof(tptr->datum);
		}
		List_append(arglist, n);
	}

	if (proc == NULL) {
		printf("error: missing procedure\n");
		return EVAL_ERR;
	}
	/* elementary arithmetic */
	else if (!(strcmp(proc, "*"))) {
		if (arglist->length == 0) {
			printf("error: wrong number of arguments\n");
			return EVAL_ERR;
		}
		*result = *((float *)(List_first(arglist)->data));
		for (ptr = List_first(arglist)->next; ptr != NULL; ptr = ptr->next)
			*result *= *((float *)ptr->data);
	}
	else if (!(strcmp(proc, "+"))) {
		if (arglist->length == 0) {
			printf("error: wrong number of arguments\n");
			return EVAL_ERR;
		}
		*result = *((float *)(List_first(arglist)->data));
		for (ptr = List_first(arglist)->next; ptr != NULL; ptr = ptr->next)
			*result += *((float *)ptr->data);
	}
	else if (!(strcmp(proc, "-"))) {
		if (arglist->length == 0) {
			printf("error: wrong number of arguments\n");
			return EVAL_ERR;
		}
		*result = *((float *)(List_first(arglist)->data));
		for (ptr = List_first(arglist)->next; ptr != NULL; ptr = ptr->next)
			*result -= *((float *)ptr->data);
	}
	else if (!(strcmp(proc, "/"))) {
		if (arglist->length == 0) {
			printf("error: wrong number of arguments\n");
			return EVAL_ERR;
		}
		*result = *((float *)(List_first(arglist)->data));
		for (ptr = List_first(arglist)->next; ptr != NULL; ptr = ptr->next)
			*result /= *((float *)ptr->data);
	}
	/* store or show register */
	else if (!(strcmp(proc, "a"))) {
		if (arglist->length > 0)
			reg_a = *((float *)(List_first(arglist)->data));
		*result = reg_a;
	}
	else if (!(strcmp(proc, "b"))) {
		if (arglist->length > 0)
			reg_b = *((float *)(List_first(arglist)->data));
		*result = reg_b;
	}
	else if (!(strcmp(proc, "c"))) {
		if (arglist->length > 0)
			reg_c = *((float *)(List_first(arglist)->data));
		*result = reg_c;
	}
	else if (!(strcmp(proc, "ans")) && (arglist->length == 0))
		*result = reg_ans;
	/* quit */
	else if (!(strcmp(proc, "exit")))
		return EVAL_EXIT;
	else {
		printf("error: unsupported procedure \"%s\"\n", proc);
		return EVAL_ERR;
	}

	/* free the list we used */
	List_free(arglist);

	return EVAL_SUCCESS;
}
