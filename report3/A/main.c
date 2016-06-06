#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct _tnode{
    int data;
    struct _tnode *parent, *left, *right;
} tnode;


tnode *new_node(int data) {
    tnode *root = malloc(sizeof(tnode));
    if(root==NULL) {
        fprintf(stderr, "failed to create new node\n");
        exit(-1);
    }
    root->data = data;
    root->left = NULL;
    root->right = NULL;
    return root;
}

tnode *btree_create() {
    return NULL;
}

tnode *btree_insert(int v, tnode *t) {
    if(t==NULL) {
        return new_node(v);
    } else if(v < t->data) {
       t->left = btree_insert(v, t->left);
    } else {
       t->right = btree_insert(v, t->right);
    }
    return t;
}

void btree_destroy(tnode *t) {
    if(t->left != NULL) btree_destroy(t->left);
    if(t->right != NULL) btree_destroy(t->right);
    free(t);
}

void btree_dump(tnode *t) {
    if(t==NULL) {
        fprintf(stderr, "error: binary tree is NULL\n");
        return;
    }
    if(t->left != NULL) btree_dump(t->left);
    printf("%d\n", t->data);
    if(t->right != NULL) btree_dump(t->right);
}

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void *func(void *arg) {
    int i;
    unsigned int seed = 100;
    for(i=0; i<1000; ++i) {
        int ran = rand_r(&seed);
        int r;
        r = pthread_mutex_lock(&m);
        if (r != 0) {
            fprintf(stderr, "can not lock");
            exit(-1);
        }
        tnode **tree = (tnode **) arg;
        *tree = btree_insert(ran, *tree);
        r = pthread_mutex_unlock(&m);
        if (r != 0) {
            fprintf(stderr, "can not lock");
            exit(-1);
        }
    }
    pthread_exit(NULL);
}

void *badfunc(void *arg) {
    int i;
    unsigned int seed = 100;
    for(i=0; i<1000; ++i) {
        int ran = rand_r(&seed);
        tnode **tree = (tnode **) arg;
        *tree = btree_insert(ran, *tree);
    }
    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t thread[100];
    tnode *tree;
    tree = btree_create();

    for(i=0; i<100; ++i) {
        //if(pthread_create(&thread[i], NULL, badfunc, &tree) != 0) {
        if(pthread_create(&thread[i], NULL, func, &tree) != 0) {
            fprintf(stderr, "error: pthread_create\n");
            return -1;
        }
    }
    for(i=0; i<100; ++i) {
        if(pthread_join(thread[i], NULL) != 0) {
            fprintf(stderr, "error: pthread_join\n");
            return 1;
        }
    }

    btree_dump(tree);
    if(tree != NULL) btree_destroy(tree);
    return 0;
}
