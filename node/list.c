/* node - data structure implementations
 * author: Eugene Ma (edmaa2) */

#include "node.h"

/* create an empty list */
List *
listnew(void) {
	List *tmp = malloc(sizeof(List));

	/* check if malloc() succeeded */
	if (tmp == NULL)
		return NULL;
	/* empty list */
	tmp->head = NULL;
	tmp->length = 0;

	return tmp;
}

/* clone an existing list */
List *
listclone(List *lst) {
	List *new;
	Node *head;

	if (lst == NULL || (new = listnew()) == NULL)
		return NULL;
	for (head = lst->head; head != NULL; head = head->next)
		listappend(new, head->data);

	return new;
}

/* add new node to end of list */
Node *
listappend(List *lst, void *data) {
	Node *ptr;

	if (lst == NULL)
		return NULL;

	for(ptr = lst->head; ptr != NULL && ptr->next != NULL; ptr = ptr->next)
		;
	/* new head if empty list */
	if(ptr == NULL)
		lst->head = ptr = nodenew(data);
	else if((ptr = nodeinsert(ptr, data)) == NULL)
		return NULL;
	(lst->length)++;

	return ptr;
}

/* add a node to the front of the list */
Node *
listprepend(List *lst, void *data) {
	Node *ptr;

	if (lst == NULL || (ptr = nodenew(data)) == NULL)
		return NULL;
	ptr->next = lst->head;
	lst->head = ptr;
	(lst->length)++;

	return ptr;
}

/* return the last node */
Node *
listlast(List *lst) {
	Node *p;

	/* check emptiness */
	if (lst == NULL || lst->head == NULL)
		return NULL;
	/* find last node */
	for (p = lst->head; p->next != NULL; p = p->next)
		;

	return p;
}

/* return the first node */
Node *
listfirst(List *lst) {
	return (lst == NULL) ? NULL : lst->head;
}

/* call func on data of each node in lst (rewrite to visit nodes?) */
void 
listtraverse(List *lst, void (*func)(void *data)) {
	Node *ptr;

	/* empty list */
	if (lst->head == NULL)
		return;
	for (ptr = lst->head; ptr != NULL; ptr = ptr->next)
		(*func)(ptr->data);
}

/* perform a search on every node in the list */
Node *
listsearch(List *haystack, void *needle, int (*search)(void *h, void *n)) {
	Node *ptr;

	/* empty list */
	if (haystack->head == NULL)
		return NULL;
	for (ptr = haystack->head; ptr != NULL; ptr = ptr->next) {
		if (((*search)(ptr->data, needle)) == 0)
			return ptr;
	}

	return NULL;
}

/* remove and free first node */
void *
listremovefirst(List *lst) {
	if (lst == NULL || listsize(lst) <= 0)
		return NULL;
	return listremove(lst, listfirst(lst)->data);
}

/* remove and free last node */
void *
listremovelast(List *lst) {
	if (lst == NULL || listsize(lst) <= 0)
		return NULL;
	return listremove(lst, listlast(lst)->data);
}

/* remove node in list */
void *
listremovenode(List *lst, Node *n) {
	void *tmp;

	/* if null or empty */
	if (lst == NULL || n == NULL || !listsize(lst))
		return NULL;
	if (listfirst(lst) == n)
		return listremovefirst(lst);
	tmp = n->data;
	noderemove(listfirst(lst), n);
	lst->length--;

	return tmp;
}

/* remove the node pointing to data */
void *
listremove(List *lst, void *data) {
	Node *p, *target;

	/* base case */
	if (listsize(lst) == 0)
		return NULL;
	/* check head */
	if (lst->head->data == data) {
		target = lst->head;
		lst->head = target->next;
		lst->length--;
		free(target);
		return data;
	}
	/* find previous node */
	for (p = listfirst(lst); p->next != NULL; p = p->next) {
		if (p->next->data == data)
			break;
	}
	target = p->next;
	/* not found */
	if (target == NULL)
		return NULL;
	p->next = target->next;
	free(target);
	lst->length--;

	return data;
}

/* free all nodes held by lst */
void 
listfree(List *lst) {
	if (lst == NULL)
		return;
	/* free linked nodes held by list */
	while (lst->head != NULL)
		listremovefirst(lst);
	/* free list pointer */
	free(lst);
}

/* return size of list */
int 
listsize(List *lst) {
	if (lst == NULL)
		return -1;
	return lst->length;
}

/* debug */
void 
listprint(List *lst) {
	Node *ptr;
	for (ptr = lst->head; ptr != NULL; ptr = ptr->next)
		printf("%s\n", (char *)(ptr->data));
}
