#include <stdio.h>
#include "parser.h"

int main(void) {
        Expr *expr = parse("(define foo 9) ");
        tree_print(expr);
        return 0;
}
