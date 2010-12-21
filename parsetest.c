#include <stdio.h>
#include "parser.h"

int main(void) {
        Expr *expr = parse("(print \"hello world\")");
        tree_print(expr);
        return 0;
}
