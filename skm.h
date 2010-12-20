/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

/* TODO: 
 * Support empty lists e.g. () 
 */

#include "parser.h"
#ifndef DS_H
#define DS_H
#include "ds/ds.h"
#endif

#define SYMBOL_MAX 	30
#define UNBOUND_LAMBDA 	0
#define BOUND_LAMBDA 	1
#define RETVAL_ATOM 	0
#define RETVAL_LAMBDA 	2
#define RETVAL_ERROR 	-1

typedef struct Tree Env;	
typedef struct {
	List *bindings;
	int lambda_count;
} Frame;
typedef struct {
	char symbol[SYMBOL_MAX];
	void *value;
	int type;
} Bind;
typedef struct {
	Env *env;
 	Expr *body;
	Expr *param;
	int bind_count;
} Lambda;

Env *env_new(void);
Env *env_extend(Env *env, Frame *f);
Env *env_parent(Env *env);
Frame *env_frame(Env *env);
Bind *env_search(Env *env, char *symbol);
void env_print(Env *env);
void env_sweep_frames(Env *env);
void env_sweep_lambdas(Env *env);
int env_is_global(Env *env);

Frame *frame_new(void);
Bind *frame_search(Frame *f, char *symbol);
void frame_free(Frame *f);
void frame_print(Env *env);

Bind *bind_new(char *symbol, void *value, int type);
Bind *bind_add(Env *env, Bind *bind);
void bind_remove(Frame *f, Bind *bind);
void bind_print(Bind *bind);
void bind_free(Bind *bind);
int bind_match(void *bind, void *symbol);

Lambda *lambda_new(Env *env, Expr *body, Expr *params);
void lambda_check_remove(Lambda *b);
void lambda_print(Lambda *b);
void lambda_free(Lambda *b);
