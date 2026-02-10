#ifndef AVLTREE_HEADER
#define AVLTREE_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define NO_NODE -1
#define LOGGING_THRESHOLD 100

typedef struct {
    unsigned char height;
    int key;
    int left;
    int right;
} Node;

typedef struct {
    int size;
    int root;
    bool logging;
    Node nodes[];
} Tree;

Tree* initTree(int nodes);
int initNode(Tree *tree, int key);
unsigned char getHeight(Tree *tree, int idx);
void fixHeight(Tree *tree, int idx);
int balanced(Tree *tree, int idx);
int rotateLeft(Tree *tree, int aIdx);
int rotateRight(Tree *tree, int aIdx);
int balance(Tree *tree, int idx);
int insert(Tree *tree, int idx, int key);

#endif
