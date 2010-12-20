/* list.c - linked lists
 * author: Eugene Ma (edmaa2) */
#include "ds.h"

/* create an empty list */
List *list_new(void) {
	List *ls = malloc(sizeof(List));

	/* check if malloc() succeeded */
	if (ls == NULL)
		return NULL;
	/* empty list */
	ls->head = NULL;
	ls->length = 0;
	return ls;
}

/* clone an existing list */
List *list_copy(List *orig) {
	List *copy;
	Node *head;

	if (orig == NULL)
		return NULL;
        copy = list_new();
        if (copy == NULL)
                return NULL;
	for (head = orig->head; head; head = head->next) {
		if (!list_append(copy, head->data)) {
                        list_free(copy);
                        return NULL;
                }
        }
	return copy;
}

/* add new node to end of list */
Node *list_append(List *ls, void *data) {
	Node *p, *n;

	if (ls == NULL)
		return NULL;
        /* prepare new node */
        n = malloc(sizeof(Node));
        if (n == NULL)
                return NULL;
        n->data = data;
        n->next = NULL;
	/* make new head if empty list */
	if (ls->head == NULL) {
		ls->head = n;
        } else {
                /* insert it at the end */
                for (p = ls->head; p->next; p = p->next)
                        ;
                /* link up */
                p->next = n;
        }
	ls->length++;
	return n;
}

/* add a node to the front of the list */
Node *list_prepend(List *ls, void *data) {
	Node *n;

        if (ls == NULL)
                return NULL;
        n = malloc(sizeof(Node));
        if (n == NULL)
		return NULL;
	n->next = ls->head;
	ls->head = n;
	ls->length++;
	return n;
}

/* return the last node */
Node *list_last(List *ls) {
	Node *p;

        if (ls == NULL)
                return NULL;
        /* empty list */
	if (ls->head == NULL)
		return NULL;
	/* find last node */
	for (p = ls->head; p->next; p = p->next)
		;
	return p;
}

/* return the first node */
Node *list_first(List *ls) {
	return (ls == NULL) ? NULL : ls->head;
}

/* visit DATA of each node in the list */
void list_traverse(List *ls, void (*func)(void *data)) {
	Node *p;

	/* empty list */
	if (ls->head == NULL)
		return;
	for (p = ls->head; p; p = p->next)
		(*func)(p->data);
}

/* return first NODE in list that returns true */
Node *list_search(List *haystack, void *needle, int (*search)(void *h, void *n)) {
	Node *p;

	/* empty list */
	if (haystack->head == NULL)
		return NULL;
	for (p = haystack->head; p; p = p->next) {
		if (((*search)(p->data, needle)) == 0)
			return p;
	}
	return NULL;
}

/* remove and free first node */
void *list_remove_first(List *ls) {
	if (ls == NULL || !list_size(ls))
		return NULL;
	return list_remove(ls, list_first(ls)->data);
}

/* remove and free last node */
void *list_remove_last(List *ls) {
	if (ls == NULL || !list_size(ls))
		return NULL;
	return list_remove(ls, list_last(ls)->data);
}

/* remove the node pointing to data */
void *list_remove(List *ls, void *data) {
	Node *p, *target;

	/* base case */
	if (!list_size(ls))
		return NULL;
	/* check head */
	if (ls->head->data == data) {
		target = ls->head;
		ls->head = target->next;
		ls->length--;
		free(target);
		return data;
	}
	/* find previous node */
	for (p = list_first(ls); p->next; p = p->next) {
		if (p->next->data == data)
			break;
	}
	target = p->next;
	/* not found */
	if (target == NULL)
		return NULL;
	p->next = target->next;
	free(target);
	ls->length--;
	return data;
}

/* free all nodes held by lst */
void list_free(List *ls) {
	if (ls == NULL)
		return;
	/* free nodes held by list */
	while (ls->head != NULL)
		list_remove_first(ls);
	free(ls);
}

/* return size of list */
int list_size(List *ls) {
	if (ls == NULL)
		return -1;
	return ls->length;
}

/* debug */
void list_print(List *ls) {
	Node *p;
	for (p = ls->head; p; p = p->next)
		printf("%s\n", (char *)(p->data));
}
