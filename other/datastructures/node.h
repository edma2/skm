/* Node data structure :
 * Elements of a linked list
 */
#include <stdio.h> /* for printf in List_print() */
#include <string.h>
#include <stdlib.h>

struct Node {
	void *data;
	struct Node *next; 
};
typedef struct Node Node;

Node *Node_insert(Node *p, void *data); 	/* insert after node p */
inline Node *Node_create(void *data); 		/* create unconnected node */
void Node_remove(Node *head, Node *n); 		/* remove and free node pointed by n head && n != NULL */
