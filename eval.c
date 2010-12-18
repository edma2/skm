/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

/* TODO: better error messages 
         redesign expr interface to car/cdr
         make data structures lighter
         car/cdr lists - e.g. '(1 2 3)
         */

#define _GNU_SOURCE
#include "skm.h"
#include <ctype.h>

#define INPUTMAX 200

typedef struct {
	void *value;
	int type;
} Operand;

/* general */
int eval(Env *env, Expr *expr, void **result);
int apply(Lambda *op, List *operands, void **result);
Bind *lookup(Env *env, char *symbol);
Env *env_setup_call(Lambda *op, List *operands);

/* primitives */
static void init_primitives(Env *env);
static void prim_add(Env *env, char *ident);
static char *prim_get(Lambda *proc);

/* special forms */
int eval_lambda(Env *env, Expr *expr, void **result);
int eval_define(Env *env, Expr *expr, void **result);

/* application */
Lambda *get_operator(Env *env, Expr *expr);
List *get_operands(Env *env, Expr *expr);
Operand *op_new(void *value, int type);
void op_free(Operand *op);
static void op_free_helper(void *data);

/* determine expression type */
int isatom(Expr *expr);
int islist(Expr *expr);
int isemptylist(Expr *expr);
int isnum(char *atom);
int isquoted(char *atom);
int isdefine(Expr *expr);
int islambda(Expr *expr);
int isprim(Lambda *b);

int main(void) {
	Env *global;
	Expr *expr;
	Lambda *b;
	char buf[INPUTMAX];
	void *result = NULL;
	int retval;

	global = env_new();
	if (global == NULL)
		return -1;
	init_primitives(global);
	while (1) {
		/* display useful information and prompt */
		env_print(global);
		printf("skm> ");
		/* get input */
		if (fgets(buf, INPUTMAX, stdin) == NULL)
			break;
		buf[strlen(buf)-1] = '\0';
		/* parse and evaluate, store value in result */
		expr = parse(buf);
		if (expr == NULL) {
			expr_free(expr);
			continue;
		}
		retval = eval(global, expr, &result);
		/* check return value and print output */
		if (retval == RETVAL_NUMBER || retval == RETVAL_LITERAL) {
			printf("%s\n", (char *)result);
		} else if (retval == RETVAL_LAMBDA) {
			b = (Lambda *)result;
			lambda_print(b);
			/* remove lambda if we didn't bind it immediately */
                        lambda_cleanup(b);
		} else if (retval == RETVAL_ERROR) {
			printf("not supported yet\n");
		}
		/* clean up */
		if (retval != RETVAL_LAMBDA && retval != RETVAL_ERROR)
			free(result);
		expr_free(expr);
		env_cleanup(global);
	}
	env_cleanup_all(global);
	return 0;
}

int eval(Env *env, Expr *expr, void **result) {
	Lambda *proc;
	List *operands;
	Bind *bind;
	int retval;
	char *atom;

	if (env == NULL || expr == NULL)
		return RETVAL_ERROR;
	if (isatom(expr)) {
		/* atoms and special forms */
		atom = expr_getword(expr);
		if (isnum(atom)) {
			*result = strdup(atom);
			return RETVAL_NUMBER;
		} else if (isquoted(atom)) {
			*result = strdup(atom + 1);
			return RETVAL_LITERAL;
		} else {
			/* look up the symbol in environment
			 * starting from current frame */
			if ((bind = lookup(env, atom)) == NULL)
				return RETVAL_ERROR;
			if (bind->type != RETVAL_LAMBDA)
				*result = strdup((char *)bind->value);
			else
				*result = bind->value;
			return bind->type;
		}
	} else {
		if (isdefine(expr)) {
			return eval_define(env, expr, result);
		} else if (islambda(expr)) {
			return eval_lambda(env, expr, result);
		} else {
                        /* application */
			proc = get_operator(env, expr);
			if (proc == NULL)
				return RETVAL_ERROR;
			operands = get_operands(env, expr);
			if (operands == NULL)
				return RETVAL_ERROR;
			retval = apply(proc, operands, result);
			/* cleanup */
                        lambda_cleanup(proc);
			listtraverse(operands, op_free_helper);
			listfree(operands);
			return retval;
		}
	}
	return RETVAL_ERROR;
}

/* Return non-zero if the Expression is not a list */
int isatom(Expr *expr) {
	return expr_isword(expr);
}

/* Return non-zero if the Expression is an empty list */
int isemptylist(Expr *expr) {
	return expr_isemptylist(expr);
}

/* Return non-zero if the Expression is a non-empty list */
int islist(Expr *expr) {
	return expr_islist(expr);
}

/* Return non-zero if the string is real number */
int isnum(char *atom) {
	int decimalcount = 0;

	/* error */
	if (atom == NULL || *atom == '\0')
		return 0;
	/* check sign */
	if (*atom == '-')
		atom++;
	/* check each char */
	for (; *atom != '\0'; atom++) {
		if (*atom == '.') {
			if (++decimalcount > 1)
				break;
		} else if (!isdigit(*atom)) {
			break;
		}
	}
	/* return true if we reached the end of the string */
	return (*atom == '\0') ? 1 : 0;
}

/* Return non-zero if the first char is a single quote */
int isquoted(char *atom) {
	if (atom == NULL)
		return 0;
	return (*atom == '\'');
}

/* Return non-zero if the expression is a define evaluation */
int isdefine(Expr *expr) {
	if (expr == NULL)
		return 0;
	/* must consist of at least 3 symbols */
	if (expr_len(expr) != 3)
		return 0;
	/* check the first word of the expression tree */
        if (!isatom(expr_subexpr(expr)))
                return 0;
	return (!strcmp(expr_getword(expr_subexpr(expr)), "define"));
}

/* Return non-zero if the expression is a lambda evaluation */
int islambda(Expr *expr) {
	if (expr == NULL)
		return 0;
	/* must consist of at least 3 symbols */
	if (expr_len(expr) != 3)
		return 0;
        if (!isatom(expr_subexpr(expr)))
                return 0;
	return (!strcmp(expr_getword(expr_subexpr(expr)), "lambda"));
}

/* Returns non-zero if the lambda is a primitive procedure.
 * A primitive operator is implemented by a lambda with
 * a NULL body expression. The primitive's param expression
 * is replaced by a identifying string instead of a set of 
 * parameters */
int isprim(Lambda *b) {
	if (b == NULL)
		return -1;
	return (b->body == NULL);
}

/* Return the primitive procedure's identifier */
static char *prim_get(Lambda *proc) {
	if (proc == NULL)
		return NULL;
	return expr_getword(proc->param);
}

/* Populate the initial environment with primitive procedures */
static void init_primitives(Env *env) {
	if (env == NULL)
		return;
	/* addition */
	prim_add(env, "+");
	/* please add a few more... */
}

/* Add a primitive */
static void prim_add(Env *env, char *ident) {
	Lambda *proc;
	Expr *identifier;
	Bind *bind;

	identifier = parse(ident);
	/* param looks like this:
	 *\+
	 */
	if (identifier == NULL)
		return;
	proc = lambda_new(env, NULL, identifier);
	if (proc == NULL) {
		expr_free(identifier);
		return;
	}
	bind = bind_new(ident, proc, RETVAL_LAMBDA);
	if (bind == NULL) {
		lambda_free(proc);
		return;
	}
	if (bind_add(env, bind) == NULL) {
		bind_free(bind);
		return;
	}
}

/* Evaluate lambda statement */
int eval_lambda(Env *env, Expr *expr, void **result) {
	Expr *param;
	Expr *body;
	Lambda *lambda;

	if (expr == NULL)
		return RETVAL_ERROR;
	/* check for correct format */
	if (expr_len(expr) != 3)
		return RETVAL_ERROR;
	/* extract parameters and body */
	param = expr_next(expr_subexpr(expr));
	body = expr_next(expr_next(expr_subexpr(expr)));
	/* create a lambda from the obtained expressions */
	lambda = lambda_new(env, body, param);
	*result = lambda;
	return RETVAL_LAMBDA;
}

/* Evaluate define statement */
int eval_define(Env *env, Expr *expr, void **result) {
	char *dsymbol, *dvalue;
	int retval;
	Bind *bind;
	Expr *dexpr;

	/* evaluate expression */
	dexpr = expr_next(expr_next(expr_subexpr(expr)));
	retval = eval(env, dexpr, result);
	if (retval == RETVAL_ERROR)
		return RETVAL_ERROR;
	/* get symbol and value */
	dsymbol = expr_getword(expr_next(expr_subexpr(expr)));
	dvalue = *result;
	/* create binding */
	bind = bind_new(dsymbol, dvalue, retval);
	if (bind == NULL)
		return RETVAL_ERROR;
	/* add binding to environment */
	if ((bind_add(env, bind)) == NULL) {
		bind_free(bind);
		return RETVAL_ERROR;
	}
	return retval;
}


/* FINISH */
int apply(Lambda *op, List *operands, void **result) {
	Node *p;
        Env *env;
	float f = 0;

	if (isprim(op)) {
		/* perform addition */
		if (!strcmp(prim_get(op), "+")) {
			for (p = listfirst(operands); p; p = p->next)
				f += atof(((Operand *)p->data)->value);
			asprintf((char **)result, "%f", f);
			return RETVAL_NUMBER;
		} else {
			return RETVAL_ERROR;
		}
	} else {
                env = env_setup_call(op, operands);
                if (env == NULL)
                        return RETVAL_ERROR;
                return eval(env, op->body, result);
	}
	return RETVAL_ERROR;
}

/* set up a lambda call, return pointer to the prepared environment */
Env *env_setup_call(Lambda *op, List *operands) {
        /* bind each operand to a new frame */
        /* extend this frame to the lambda environment */
        Env *env;
        Frame *f;
        Bind *bind;
        Expr *param;
        /* for iterating through operands list */
        Node *p;
        Operand *opand;

        if (op == NULL || operands == NULL)
                return NULL;
        /* check for mis matching number of operands */
        if (expr_len(op->param) != listsize(operands))
                return NULL;
        f = frame_new();
        if (f == NULL)
                return NULL;
        env = env_extend(op->env, f);
        if (env == NULL) {
                frame_free(f);
                return NULL;
        }
        /* go through each parameter and bind an operand to it */
        p = listfirst(operands);
        for (param = expr_subexpr(op->param); param; param = expr_next(param)) {
                if (p == NULL)
                        break;
                opand = (Operand *)p->data;
                bind = bind_new(expr_getword(param), opand->value, opand->type);
                if (bind == NULL)
                        break;
                if (bind_add(env, bind) == NULL)
                        break;
                p = p->next;
        }
        if (param != NULL) {
                frame_free(f);
                return NULL;
        }
        env_print(op->env);
        return env;
}

/* Get operator of an expression, which will
 * always be the first atom in the expression */
Lambda *get_operator(Env *env, Expr *expr) {
	void *proc;
	int retval;

	retval = eval(env, expr_subexpr(expr), &proc);
	if (retval != RETVAL_LAMBDA)
		return NULL;
	return (Lambda *)proc;
}

/* Get the operands of an expression */
List *get_operands(Env *env, Expr *expr) {
	List *operands;
	Operand *op;
	void *result;
	int retval;

	if (expr == NULL)
		return NULL;
	/* make sure it is not an atom or empty list */
	if (isatom(expr) || isemptylist(expr))
		return NULL;
	/* allocate a new list for operands */
	operands = listnew();
	if (operands == NULL)
		return NULL;
	for (expr = expr_next(expr_subexpr(expr)); expr; expr = expr_next(expr)) {
		/* evaluate each sub expression recursively */
		retval = eval(env, expr, &result);
		if (retval == RETVAL_ERROR) {
			listtraverse(operands, op_free_helper);
			listfree(operands);
			return NULL;
		} 
		op = op_new(result, retval);
		if (op == NULL) {
			listtraverse(operands, op_free_helper);
			listfree(operands);
			return NULL;
		}
		listappend(operands, op);
	}

	return operands;
}

Operand *op_new(void *value, int type) {
	Operand *op;

	op = malloc(sizeof(Operand));
	if (op == NULL)
		return NULL;
	op->value = value;
	op->type = type;
	return op;
}

static void op_free_helper(void *data) {
	op_free((Operand *)data);
}

void op_free(Operand *op) {
	if (op->type == RETVAL_LAMBDA)
                lambda_cleanup((Lambda *)op->value);
	else
		free(op->value);
	free(op);
}

Bind *lookup(Env *env, char *symbol) {
        return env_search(env, symbol);
}
