/* node - data structure implementations
 * author: Eugene Ma (edmaa2) */

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

typedef struct Node Node;
typedef struct Tree Tree;
struct Node {
	void *data;
	struct Node *next; 
};

typedef struct {
	Node *head;			
	int length;		
} List;

struct Tree {
	void *data;
	List *children;	
	struct Tree *parent;
};

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

Tree *treeaddchild(Tree *parent, void *data); 	
Tree *treeaddsib(Tree *t, void *data);
Tree *treecopy(Tree *orig);		
Tree *treefirstchild(Tree *parent);	
Tree *treesearch(Tree *haystack, void *needle, int (*search)(void *h, void *n));
void treefree(void *t);			
void treefreedata(Tree *t);			
int treeisleaf(Tree *t);
Tree *treenew(void *data);	
Tree *treenext(Tree *t);
Tree *treeparent(Tree *t);
void treeremove(Tree *t);
void treeprint(Tree *t, int depth);    	
int treesearchstring(void *datum, void *str); 	
void treesetdatum(Tree *t, void *data); 
void treetraverse(Tree *t, void (*func)(Tree *t));
