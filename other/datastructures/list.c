/* list.c - a singly linked list implementation
 * badly written by Eugene Ma */

#include "datastructures.h"

/* create an empty list */
List *List_new(void) {
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
List *List_clone(List *lst) {
	List *new;
	Node *head;

	if(!lst || !(new = List_new()))
		return NULL;
	for(head = lst->head; head != NULL; head = head->next)
		List_append(new, head->data);

	return new;
}

/* add new node to end of list */
Node *List_append(List *lst, void *data) {
	Node *ptr;

	for(ptr = lst->head; ptr != NULL && ptr->next != NULL; ptr = ptr->next)
		;
	/* new head if empty list */
	if(ptr == NULL)
		lst->head = ptr = Node_create(data);
	else if((ptr = Node_insert(ptr, data)) == NULL)
		return NULL;
	(lst->length)++;

	return ptr;
}

/* add a node to the front of the list */
Node *List_prepend(List *lst, void *data) {
	Node *ptr;

	if (lst == NULL || (ptr = Node_create(data)) == NULL)
		return NULL;
	ptr->next = lst->head;
	lst->head = ptr;
	(lst->length)++;

	return ptr;
}
/* return the last node */
Node *List_last(List *lst) {
	Node *ptr;

	/* check emptiness */
	if (lst == NULL || lst->head == NULL)
		return NULL;
	/* find last node */
	for (ptr = lst->head; ptr->next != NULL; ptr = ptr->next)
		;

	return ptr;
}

/* return the first node */
Node *List_first(List *lst) {
	if (lst == NULL || lst->head == NULL)
		return NULL;
	return lst->head;
}

/* call func on data of each node in lst */
void List_traverse(List *lst, void (*func)(void *data)) {
	Node *ptr;

	/* empty list */
	if (lst->head == NULL)
		return;
	for (ptr = lst->head; ptr != NULL; ptr = ptr->next)
		(*func)(ptr->data);
}

/* pass to List_traverse */
void free_data(void *data) {
	free(data);
}

/* perform a search on every node in the list */
Node *List_search(List *haystack, void *needle, int (*search)(void *h, void *n)) {
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

/* return 0 if data matches string */
int List_search_string(void *data, void *str) {
	if (data == NULL || str == NULL)
		return -1;
	return strcmp((char *)data, (char *)str);
}

/* delete and free the first node of the list */
void List_remove_first(List *lst) {
	Node *tmp = lst->head;

	if(tmp == NULL)
		return;
	lst->head = tmp->next;
	free(tmp);
	(lst->length)--;
}

/* delete and free the last node from the list */
void List_remove_last(List *lst) {
	Node *tmp;

	/* check if empty or NULL */
	if (lst == NULL || lst->head == NULL)
		return;
	/* find the last node */
	for (tmp = lst->head; tmp->next != NULL; tmp = tmp->next)
		;
	/* remove head or subsequent node */
	if (tmp == lst->head) {
		List_remove_first(lst);
		return;
	}
	else
		Node_remove(lst->head, tmp);
	(lst->length)--;
}

/* free all nodes held by lst */
void List_free_nodes(List *lst) {
	if (lst == NULL)
		return;
	/* free linked nodes held by list */
	while (lst->head != NULL)
		List_remove_first(lst);
	/* free list pointer */
	free(lst);
}
void List_free_data(List *lst) {
	/* visit every node and free data */
	List_traverse(lst, free_data);
}
void List_free(List *lst) {
	List_free_data(lst);
	List_free_nodes(lst);
}

/* debug */
void List_print(List *lst) {
	Node *ptr;
	for (ptr = lst->head; ptr != NULL; ptr = ptr->next)
		printf("%s\n", (char *)(ptr->data));
}
