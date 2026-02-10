#include "ptree.h"
#include <stdio.h>

Node* initNode(int weight) {
    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    node->weight = weight;
    node->left = NULL;
    node->right = NULL;
    node->ch = '\0';

    return node;
}

Node* initLeafNode(int weight, char ch) {
    Node *node = initNode(weight);
    if (!node) return NULL;

    node->ch = ch;

    return node;
}

bool isLeaf(Node* node) {
    if (!node) {
        fprintf(stderr, "NODE IS NULL!!!\n");
        return false;
    }
    return !node->left && !node->right;
}

Node* joinNodes(Node *left, Node *right) {
    Node *root = initNode(left->weight + right->weight);
    if (!root) return NULL;

    root->left = left;
    root->right = right;

    return root;
}

void freeTree(Node *node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

