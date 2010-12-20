/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */
#include "skm.h"

static void env_sweep_frames_helper(Env *env);
static void env_sweep_lambdas_helper(Env *env);
static void bind_reset(Env *env, char *symbol);
static void bind_free_helper(void *data);
static void bind_print_helper(void *data);
static int lambda_isbound(Lambda *b);

/************************************************/
/*************    Environments    ***************/
/************************************************/

/* return a new environment with empty frame */
Env *env_new(void) {
	Frame *f = frame_new();
	if (f == NULL)
		return NULL;
	return tree_new(f);
}

/* return 1 if this environment has no parent */
int env_is_global(Env *env) {
	return env_parent(env) == NULL;
}

/* extend environment with frame */
Env *env_extend(Env *env, Frame *f) {
	if (f == NULL)
		return NULL;
	return tree_insert_child(env, f);
}

/* return parent environment */
Env *env_parent(Env *env) {
	return tree_parent(env);
}

/* lookup a symbol */
Bind *env_search(Env *env, char *symbol) {
	Bind *bind;

	if (symbol == NULL)
		return NULL;
	for (; env != NULL; env = env_parent(env)) {
		/* found a binding with matching symbol */
		if ((bind = frame_search(env_frame(env), symbol)))
			return bind;
	}
	return NULL;
}

Frame *frame_new(void) {
	Frame *f = malloc(sizeof(Frame));

	if (f == NULL)
		return NULL;
	/* create an empty list of bindings */
	f->bindings = list_new();
	if (f->bindings == NULL) {
		free(f);
		return NULL;
	}
	/* mark the frame as unsaved */
	f->lambda_count = 0;
	return f;
}

Bind *frame_search(Frame *f, char *symbol) {
	/* return a matching Bind if found
	 * NULL otherwise */
	Node *ptr = list_search(f->bindings, symbol, bind_match);
	return (ptr) ? (Bind *)ptr->data : NULL;
}

int bind_match(void *bind, void *symbol) {
	if (bind == NULL || symbol == NULL)
		return -1;
	/* return 0 if match is found */
	return strcmp(((Bind *)bind)->symbol, symbol);
}

/* Call this after we create a binding, and add it to
 * the environment */
Bind *bind_add(Env *env, Bind *new) {
	Bind *old;
	Frame *f;

	if (env == NULL || new == NULL)
		return NULL;
	f = env_frame(env);
	/* check for existing binding in current frame only */
	if ((old = frame_search(f, new->symbol)))
		bind_remove(f, old);
	return (Bind *)list_append(f->bindings, new);
}

/* Remove binding from frame and call bind_free */
void bind_remove(Frame *f, Bind *bind) {
	/* remove old binding from list 
	 * and free the bind data struct */
	list_remove(f->bindings, bind);
	bind_free(bind);
}

/* Return the top-most frame of the environment */
Frame *env_frame(Env *env) {
	if (env == NULL)
		return NULL;
	return (Frame *)env->data;
}

/* Create a new binding given a pointer to a lambda
 * or a string, coupled with a symbol and value type */
Bind *bind_new(char *symbol, void *value, int type) {
	Bind *bind = malloc(sizeof(Bind));

	if (bind == NULL)
		return NULL;
	strncpy(bind->symbol, symbol, SYMBOL_MAX);
	if (type == RETVAL_LAMBDA) {
		/* if the value we're trying to bind to is
		 * a lambda, we should increase the counter 
		 * field of the lambda structure */
		((Lambda *)value)->bind_count++;
		/* we should also increase the frame counter */
		env_frame(((Lambda *)value)->env)->lambda_count++;
	} else if (type == RETVAL_ATOM) {
		/* or else we should just allocate a new string
		 * in memory */
		value = strdup((char *)value);
	}
	bind->value = value;
	bind->type = type;
	return bind;
}

/************************************************/
/****************   Lambda   ********************/
/************************************************/

/* Returns BOUND_LAMBDA if lambda should stick around in memory */
static int lambda_isbound(Lambda *b) {
	if (b == NULL)
		return -1;
	/* check if lambda is unbound */
	return (b->bind_count) ? BOUND_LAMBDA : UNBOUND_LAMBDA;
}

/* Create a new lambda given its parameters and body
 * expression, connect it to the given environment */
Lambda *lambda_new(Env *env, Expr *body, Expr *param) {
	Lambda *b;

        b = malloc(sizeof(Lambda));
	if (b == NULL)
		return NULL;
	/* non-primitive procedure */
	if (body != NULL) {
		/* copy expressions */
		body = expr_copy(body);
		param = expr_copy(param);
		if (body == NULL || param == NULL) {
			lambda_free(b);
			return NULL;
		}
	} 
	/* all ready */
	b->body = body;
	b->param = param;
	b->env = env;
	b->bind_count = 0;
	return b;
}

void lambda_free(Lambda *b) {
	/* free all memory except for environment */
	expr_free(b->body);
	expr_free(b->param);
	free(b);
}

/************************************************/
/************* Garbage Collection ***************/
/************************************************/

/* the intent of this function is to 
   free lambdas that are present in the environment */
static void bind_reset(Env *env, char *symbol) {
        Bind *zero;

        zero = bind_new(symbol, "0", RETVAL_ATOM);
        if (zero == NULL)
                return;
        bind_add(env, zero);
}

/* free a lambda if unbound */
void lambda_check_remove(Lambda *b) {
        if (b == NULL)
                return;
        if (lambda_isbound(b) == UNBOUND_LAMBDA)
                lambda_free(b);
}

/* set every binding to an arbitrary string: this
   removes all lambdas and frames */
void env_sweep_lambdas(Env *env) {
        tree_traverse(env, env_sweep_lambdas_helper);
}

/* tree traversal helper function */
static void env_sweep_lambdas_helper(Env *env) {
        Node *p;
        Frame *f = env_frame(env);
        List *garbage = list_new();

        /* collect all the lambda symbols */
        for (p = list_first(f->bindings); p; p = p->next) {
                if (((Bind *)p->data)->type == RETVAL_LAMBDA)
                        list_append(garbage, (char *)((Bind *)p->data)->symbol);
        }
        /* for each symbol make a meaningless binding 
           freeing lambdas in the process */
        for (p = list_first(garbage); p; p = p->next)
                bind_reset(env, (char *)p->data);
        list_free(garbage);
}

/* free all frames without save marker */
void env_sweep_frames(Env *env) {
	tree_traverse(env, env_sweep_frames_helper);
}

/* tree traversal helper function */
static void env_sweep_frames_helper(Env *env) {
	/* don't free the global environment either */
	if (env_is_global(env))
		return;
	/* if frame has at least one lambda that is 
	 * associated with it, don't free it! */
	if (env_frame(env)->lambda_count > 0)
		return;
	frame_free(env_frame(env));
	tree_free(env);
}

/* Remove a bind and check both lambda and frame 
 * for garbage collection by modifying and examining
 * the count */
void bind_free(Bind *bind) {
	Lambda *b;

	if (bind == NULL)
		return;
	if (bind->type == RETVAL_LAMBDA) {
		/* if we try to free a binding whose
		 * value points to a lambda we should 
		 * either decrease the count of the lambda
		 * or free it if this count becomes 0 */
		b = (Lambda *)bind->value;
		b->bind_count--;
                env_frame(b->env)->lambda_count--;
		if (lambda_isbound(b) == UNBOUND_LAMBDA)
			lambda_free(b);
	} else {
		/* free the allocated string in memory */
		free(bind->value);
	}
	free(bind);
}

void frame_free(Frame *f) {
	if (f == NULL)
		return;
	/* free all bindings */
	list_traverse(f->bindings, bind_free_helper);
	list_free(f->bindings);
	free(f);
}

/* List traversal helper function */
static void bind_free_helper(void *data) {
	/* cast the void pointer
	 * to a Bind pointer */
	bind_free((Bind *)data);
}

/************************************************/
/************* Debugging Utilities **************/
/************************************************/

/* print all frames in environment */
void env_print(Env *env) {
	tree_traverse(env, frame_print);
}

/* print the top level frame of the environment */
void frame_print(Env *env) {
	printf("lambda count: %d\n", env_frame(env)->lambda_count);
	printf("-----------------------------\n");
	if (!list_size(env_frame(env)->bindings))
		printf("[empty]\n");
	else
		list_traverse(env_frame(env)->bindings, bind_print_helper);
	printf("-----------------------------\n\n");
}

static void bind_print_helper(void *data) {
	/* this helper function casts the void pointer
	 * to a Bind so we can pass it to list traverse */
	bind_print((Bind *)data);
}

void bind_print(Bind *bind) {
	/* print some useful information about a binding */
	if (bind == NULL)
		return;
	if (bind->type != RETVAL_LAMBDA) {
		printf("[%s -> %s]\n", bind->symbol, (char *)bind->value);
	} else {
		printf("[%s -> ", bind->symbol);
		lambda_print((Lambda *)bind->value);
	}
}

void lambda_print(Lambda *b) {
	/* print the number of symbols binded to this 
	 * lambda and its address in memory */
	printf("[#proc %d (%p)]\n", b->bind_count, b);
}
