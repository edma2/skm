/* node - data structure implementations
 * author: Eugene Ma (edmaa2) */
#include "node.h"

/* Node_insert(): Insert a new node with data after p, return new node */
Node *
nodeinsert(Node *p, void *data) {
	Node *tmp;

	/* check if malloc succeeded */
	if ((tmp = malloc(sizeof(Node))) == NULL)
		return NULL;
	/* set data pointer */
	tmp->data = data;
	/* create head node */
	if (p == NULL)
		tmp->next = NULL;
	else {
		tmp->next = p->next;
		p->next = tmp;
	}

	return tmp;
}

/* Node_create(): Same as Node_insert, but use NULL for p to create new head */
inline Node *
nodenew(void *data) {
	return nodeinsert(NULL, data);
}

/* Node_remove(): Remove node pointed to by n, is chained to head
 * works for any node after head */
void 
noderemove(Node *head, Node *n) {
        if (head == NULL || n == NULL || head == n)
                return;
        while (head->next != n)
                head = head->next;
        head->next = n->next;
        /* freeing data should be user implemented! */
        free(n);
}
