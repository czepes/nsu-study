#ifndef PREFIXTREE_HEADER
#define PREFIXTREE_HEADER

#include <stdlib.h>
#include <stdbool.h>

typedef struct node Node;
struct node {
    int weight;
    unsigned char ch;
    Node *left;
    Node *right;
};

Node* initNode(int weight);
Node* initLeafNode(int weight, char ch);
Node* joinNodes(Node *left, Node *right);
bool isLeaf(Node* node);
void freeTree(Node *node);

#endif
