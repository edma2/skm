/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

#ifndef NODE_H
#define NODE_H
#include "node/node.h"
#endif
#include <string.h>

typedef Tree Expr;

Expr *parse(char *exp);
Expr *expr_copy(Expr *orig);

Expr *expr_next(Expr *expr);
Expr *expr_child(Expr *expr);
char *expr_get_word(Expr *expr);

int expr_is_emptylist(Expr *expr);
int expr_is_list(Expr *expr);
int expr_is_word(Expr *expr);
int expr_len(Expr *expr);

void expr_free(Expr *e);
