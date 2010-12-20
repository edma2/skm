/* List data structure : 
 * A singly linked list of nodes
 */
#include "node.h"

/* singly linked lists */
struct List {
	Node *head;			/* first item in list */ 
	int length;			/* length */
};
typedef struct List List;

List *List_new(void); 					/* instantiate empty list (head points to NULL) */
List *List_clone(List *lst);				/* clone an existing list */
Node *List_last(List *lst); 				/* return the last Node in List */
Node *List_first(List *lst);				/* return the first Node in List */
Node *List_append(List *lst, void *data);		/* add to back */
Node *List_prepend(List *lst, void *data);		/* add to front */
void free_data(void *data);				/* free data */
void List_remove_first(List *lst); 			/* remove and free the first node */
void List_remove_last(List *lst);			/* remove and free the last node */
void List_free(List *lst);				/* remove and free all nodes and data */
void List_free_data(List *lst);				/* free all data */
void List_free_nodes(List *lst);			/* remove and free all nodes */
void List_print(List *lst); 				/* debug utility */
int List_string_match(void *data, void *str);		/* return 0 if data matches string */
void List_traverse(List *lst, void (*func)(void *data));/* traverse list from front to back */
Node *List_search(List *haystack, void *needle, int (*search)(void *h, void *n)); 
							/* return ptr when seach function returns 0 */
