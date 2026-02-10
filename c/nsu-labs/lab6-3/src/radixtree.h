#ifndef RADIXTREE_HEADER
#define RADIXTREE_HEADER

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "strbuffer.h"

#define NO_NODE -1
#define DEFAULT_SIZE 1024

typedef struct {
    int key;
    int length;
    int sibling;
    int child;
} Node;

typedef struct {
    Node *items;
    int capacity;
    int size;
} NodeArray;

typedef struct {
    int nodes;
    int nulls;
    int maxLen;
} RadixStats;

typedef struct {
    NodeArray *nodes;
    StringBuffer *buffer;
    RadixStats *stats;
    int root;
} RadixTree;

NodeArray* initNodes(int capacity);
void freeNodes(NodeArray *nodes);
void pushNode(NodeArray *nodes, Node *node);
Node* getNode(NodeArray *nodes, int index);

RadixTree *initTree();
int initNode(RadixTree *tree, int key, int len);
int initNewNode(RadixTree *tree, char *word, int len);
void freeNode(RadixTree *tree, int idx);
void freeTree(RadixTree *tree);

void radixInsert(RadixTree *tree, char *word, int len);
char** autocomplete(RadixTree *tree, char *prefix, int len, int *amount);
int countNodes(RadixTree *tree);

#endif
