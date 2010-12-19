#include "node.h"

static void tree_free_helper(Tree *t);
static int tree_copy_helper(Tree *copy, Tree *t);
static void tree_print_helper(Tree *t, int depth);

/* making new trees */
Tree *tree_new(void *data) {
        Tree *t;

        t = malloc(sizeof(Tree));
        if (t == NULL)
                return NULL;
        t->parent = NULL;
        t->next = NULL;
        t->child = NULL;
        t->data = data;
        return t;
}

Tree *tree_insert_child(Tree *p, void *data) {
        Tree *t, *sib;

        if (p == NULL)
                return NULL;
        /* prepare tree for insertion */
        t = malloc(sizeof(Tree));
        if (t == NULL)
                return NULL;
        t->parent = p;
        t->child = NULL;
        t->data = data;
        /* check for existing sibling */
        sib = p->child;
        if (sib != NULL) {
                /* append to end */
                for (; sib->next; sib = sib->next)
                        ;
                t->next = sib->next;
                sib->next = t;
        } else {
                p->child = t;
                t->next = NULL;
        }
        return t;
}

/* insert tree right after sibling */
Tree *tree_insert_sib(Tree *sib, void *data) {
        Tree *t;
        if (sib == NULL)
                return NULL;
        /* fails with roots */
        if (sib->parent == NULL)
                return NULL;
        /* prepare for insertion */
        t = malloc(sizeof(Tree));
        if (t == NULL)
                return NULL;
        t->parent = sib->parent;
        t->next = sib->next;
        t->child = NULL;
        t->data = data;
        /* insert after sibling */
        sib->next = t;
        return t;
}

/* detach a tree from its parent and/or sibling */
Tree *tree_detach(Tree *t) {
        Tree *p, *sib;

        if (t == NULL)
                return NULL;
        /* disconnected adjacent trees */
        if (t->parent != NULL) {
                p = t->parent;
                sib = t->parent->child;
                /* t is the first child or only child */
                if (sib == t)
                        p->child = t->next;
                else {
                        /* find the preceding sibling */
                        for (; sib; sib = sib->next) {
                                if (sib->next == t)
                                        break;
                        }
                        if (sib == NULL)
                                return NULL;
                        sib->next = t->next;
                }
        }
        t->parent = NULL;
        t->next = NULL;
        return t;
}

void tree_set_data(Tree *t, void *data) {
        if (t == NULL)
                return;
        t->data = data;
}

void tree_free(Tree *t) {
        if (t == NULL)
                return;
        tree_detach(t);
        tree_traverse(t, tree_free_helper);
}

/* cast into the right function pointer */
static void tree_free_helper(Tree *t) {
        free(t);
}

/* dfs post order traversal */
void tree_traverse(Tree *t, void (*func)(Tree *t)) {
        Tree *c, *next;

        if (t == NULL)
                return;
        for (c = t->child; c; c = next) {
                /* save the next tree in case we detach the current tree */
                next = c->next;
                tree_traverse(c, func);
        }
        /* visit tree after children tree have been visited */
        func(t);
}

Tree *tree_copy(Tree *orig) {
        Tree *copy;

        if (orig == NULL)
                return NULL;
        copy = tree_new(orig->data);
        if (copy == NULL)
                return NULL;
        if (tree_copy_helper(copy, orig)) {
                tree_free(copy);
                return NULL;
        }
        return copy;
}

/* copy children - return zero on success */
static int tree_copy_helper(Tree *copy, Tree *orig) {
        Tree *origc, *copyc;

        if (orig == NULL)
                return -1;
        /* add children */
        for (origc = orig->child; origc; origc = origc->next) {
                copyc = tree_insert_child(copy, origc->data);
                if (copyc == NULL)
                        return -1;
                if (tree_copy_helper(copyc, origc))
                        return -1;
        }
        return 0;
}

void tree_print(Tree *t) {
        tree_print_helper(t, 0);
}

static void tree_print_helper(Tree *t, int depth) {
        Tree *c;
        int d;

        if (t == NULL)
                return;
        /* print nicely */
        for (d = depth; d > 0; d--)
                printf("- ");
        printf("\\");
        if (t->data == NULL)
                printf("NULL\n");
        else
                printf("%s\n", (char *)(t->data));
        for (c = t->child; c; c = c->next)
                tree_print_helper(c, depth + 1);
}

int tree_count_children(Tree *t) {
        int i = 0;

        if (t == NULL)
                return -1;
        for (t = t->child; t; t = t->next)
                i++;
        return i;
}
        
int tree_is_root(Tree *t) {
        if (t == NULL)
                return -1;
        return (t->parent == NULL);
}

int tree_is_leaf(Tree *t) {
        if (t == NULL)
                return -1;
        return (t->child == NULL);
}

/* navigating the tree */
Tree *tree_next(Tree *t) {
        if (t == NULL)
                return NULL;
        return t->next;
}

Tree *tree_parent(Tree *t) {
        if (t == NULL)
                return NULL;
        return t->parent;
}

Tree *tree_child(Tree *t) {
        if (t == NULL)
                return NULL;
        return t->child;
}
