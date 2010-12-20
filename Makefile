skm: eval.c skm.c parser.c node/list.c node/tree.c node/node.c
	gcc -Wall -o skm eval.c skm.c parser.c node/tree.c node/list.c node/node.c
