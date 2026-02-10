#ifndef BLTREE_HEADER
#define BLTREE_HEADER

#include <stdlib.h>
#include <stdbool.h>

#define NO_NODE -1

typedef struct {
    bool black;
    int key;
    int left;
    int right;
    int parent;
} Node;

typedef struct {
    int size;
    int root;
    Node nodes[];
} Tree;

Tree* initTree(int nodes);
int initNode(Tree *tree, int parent, int key);

bool isBlack(Tree *tree, int idx);
int getHeight(Tree *tree);

int rotateLeft(Tree* tree, int idx);
int rotateRight(Tree* tree, int idx);

int getParent(Tree *tree, int idx);
int getUncle(Tree *tree, int idx);

void balance5(Tree *tree, int idx, int pIdx, int gIdx);
void balance4(Tree *tree, int idx, int pIdx, int gIdx);
void balance3(Tree *tree, int idx);
void balance2(Tree *tree, int idx);
void balance1(Tree *tree, int idx);

void insert(Tree *tree, int key);

#endif
