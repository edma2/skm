#include "parser.h"

int main(void) {
	Expr *e = parse("5");

	treeprint(e, 0);

	return 0;
}
