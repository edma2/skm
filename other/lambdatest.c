#include "lambda.h"

 int main(void) {
	Environment *env = Environment_new();
	Frame *fptr;
	Binding *bind;
	List *params = List_new();
	List *args = List_new();
	Expression *expr;
	Lambda *proc;

	/* extend environment and add bindings */
	Environment_attach(env);
	fptr = (Frame *)List_first(env)->data;
	/* a -> 7 
	 * b -> 9 */
	bind = Binding_new("a", strdup("6"));
	Frame_bind(fptr, bind);
	bind = Binding_new("b", strdup("9"));
	Frame_bind(fptr, bind);

	/* prep lambda struct */
	expr = Expression_parse("(+ a 4 (* 8 b b))");
	List_append(params, "a");
	List_append(params, "b");
	proc = Lambda_new(NULL, expr, params);

	/* look up values to replace in environment */
	bind = Environment_find(env, "a");
	List_append(args, bind->value);
	bind = Environment_find(env, "b");
	List_append(args, bind->value);

	printf("before replace: \n");
	Tree_print(expr, 0);

	Lambda_replace(proc, args);

	printf("after replace: \n");
	Tree_print(expr, 0);

	return 0;
}

