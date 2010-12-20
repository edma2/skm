/* skm - scheme interpreter
 * author: Eugene Ma (edma2) */

#ifndef DS_H
#define DS_H
#include "ds/ds.h"
#endif

typedef Tree Expr;

Expr *parse(char *exp);
Expr *expr_copy(Expr *orig);
Expr *expr_next(Expr *expr);
Expr *expr_child(Expr *expr);
char *expr_get_word(Expr *expr);
void expr_free(Expr *e);
int expr_is_emptylist(Expr *expr);
int expr_is_list(Expr *expr);
int expr_is_word(Expr *expr);
int expr_len(Expr *expr);
