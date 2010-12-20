/* node.c - a singly linked list implementation
 * badly written by Eugene Ma */

#include "datastructures.h"

/* Node_insert(): Insert a new node with data after p, return new node */
Node *Node_insert(Node *p, void *data) {
	Node *tmp = malloc(sizeof(Node));

	/* malloc() failed */	
	if (tmp == NULL)
		return NULL;
	tmp->data = data;
	if (p == NULL) /* create new head */
		tmp->next = NULL;
	else {
		tmp->next = p->next;
		p->next = tmp;
	}

	return tmp;
}

/* Node_create(): Same as Node_insert, but use NULL for p to create new head */
inline Node *Node_create(void *data) {
	return Node_insert(NULL, data);
}

/* Node_remove(): Remove node pointed to by n, is chained to head
 * works for any node after head */
void Node_remove(Node *head, Node *n) {
	if (head == NULL || n == NULL || head == n)
		return;
	while (head->next != n)
		head = head->next;
	head->next = n->next;
	/* freeing data should be user implemented! */
	free(n);
}
