skm: eval.c skm.c parser.c ds/list.c ds/tree.c
	gcc -Wall -o skm eval.c skm.c parser.c ds/tree.c ds/list.c
