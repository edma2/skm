/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

/* TODO: 
 * Expressions need to be encapsulated more (hide tree implementation)
 * Fix garbage collection of frames (replace FRAME_SAVE with count?)
 * Change frame state with creation/destruction of lambdas
 * Support empty lists e.g. () 
 */

#include "parser.h"
#ifndef NODE_H
#define NODE_H
#include "node/node.h"
#endif

/* constants */
#define SYMBOL_MAX 	30
#define UNBOUND_LAMBDA 	0
#define BOUND_LAMBDA 	1
#define RETVAL_ATOM 	0
#define RETVAL_LAMBDA 	2
#define RETVAL_ERROR 	-1

/* structs */
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
Frame *frame_new(void);
Bind *bind_new(char *symbol, void *value, int type);
Lambda *lambda_new(Env *env, Expr *body, Expr *params);

Env *env_extend(Env *env, Frame *f);
Bind *bind_add(Env *env, Bind *bind);
void bind_remove(Frame *f, Bind *bind);

Env *env_parent(Env *env);
Frame *env_frame(Env *env);
int env_isglobal(Env *env);

Bind *env_search(Env *env, char *symbol);
Bind *frame_search(Frame *f, char *symbol);
int bind_match(void *bind, void *symbol);

/* garbage collection */
void env_sweep_frames(Env *env);
void env_sweep_lambdas(Env *env);
void lambda_check_remove(Lambda *b);

void env_print(Env *env);
void frame_print(Env *env);
void bind_print(Bind *bind);
void lambda_print(Lambda *b);

void bind_free(Bind *bind);
void frame_free(Frame *f);
void lambda_free(Lambda *b);
