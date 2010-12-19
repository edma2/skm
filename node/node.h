/* node - data structure implementations
 * author: Eugene Ma (edmaa2) */

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
	void *data;
	struct Node *next; 
};
struct List {
	Node *head;			
	int length;		
};
typedef struct List List;
struct Tree {
	struct Tree *parent;
	struct Tree *next;
	struct Tree *child;
	void *data;
};
typedef struct Tree Tree;

Node *listappend(List *lst, void *data); 
List *listclone(List *lst);		
Node *listfirst(List *lst);	
void listfree(List *lst);
int listsize(List *lst);
Node *listlast(List *lst); 
List *listnew(void); 	
Node *listprepend(List *lst, void *data); 	
void listprint(List *lst); 		
void *listremovefirst(List *lst); 
void *listremovelast(List *lst);
void *listremovenode(List *lst, Node *n);
void *listremove(List *lst, void *data);
Node *listsearch(List *haystack, void *needle, int (*search)(void *h, void *n));
void listtraverse(List *lst, void (*func)(void *data));	

Node *nodeinsert(Node *p, void *data); 		
inline Node *nodenew(void *data); 	
void noderemove(Node *h, Node *t);

Tree *tree_new(void *data);
Tree *tree_insert_child(Tree *t, void *data);
Tree *tree_insert_sib(Tree *t, void *data);
Tree *tree_detach(Tree *t);
Tree *tree_next(Tree *t);
Tree *tree_child(Tree *t);
Tree *tree_parent(Tree *t);
Tree *tree_copy(Tree *t);
int tree_is_root(Tree *t);
int tree_is_leaf(Tree *t);
int tree_count_children(Tree *t);
void tree_set_data(Tree *t, void *data);
void tree_traverse(Tree *t, void (*func)(Tree *t));
void tree_free(Tree *t);
void tree_print(Tree *t);
