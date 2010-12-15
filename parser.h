/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

#ifndef NODE_H
#define NODE_H
#include "node/node.h"
#endif

typedef struct Tree Expr;

Expr *parse(char *exp);
Expr *expr_copy(Expr *orig);

Expr *expr_subexpr(Expr *expr);
Expr *expr_next(Expr *expr);
char *expr_getword(Expr *expr);

int expr_isemptylist(Expr *expr);
int expr_islist(Expr *expr);
int expr_isword(Expr *expr);
int expr_len(Expr *expr);

void expr_free(Expr *e);
