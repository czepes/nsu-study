#include "avltree.h"

Tree* initTree(int nodes) {
    Tree *tree = malloc(sizeof(Tree) + nodes * sizeof(Node));

    if (tree == NULL) return NULL;

    tree->size = 0;
    tree->root = NO_NODE;
    tree->logging = nodes < LOGGING_THRESHOLD;

    return tree;
}

int initNode(Tree *tree, int key) {
    Node *node = &tree->nodes[tree->size];

    node->key = key;
    node->height = 1;
    node->left = NO_NODE;
    node->right = NO_NODE;

    return tree->size++;
}

unsigned char getHeight(Tree *tree, int idx) {
    return idx != NO_NODE ? tree->nodes[idx].height : 0;
}

void fixHeight(Tree *tree, int idx) {
    Node *node = &tree->nodes[idx];

    unsigned char left = getHeight(tree, node->left);
    unsigned char right = getHeight(tree, node->right);

    node->height = (left > right ? left : right) + 1;
}

int balanced(Tree *tree, int idx) {
    Node *node = &tree->nodes[idx];

    unsigned char left = getHeight(tree, node->left);
    unsigned char right = getHeight(tree, node->right);

    return right - left;
}

int rotateLeft(Tree *tree, int aIdx) {
    Node *a = &tree->nodes[aIdx];
    Node *b = &tree->nodes[a->right];
    int bIdx = a->right;

    a->right = b->left;
    b->left = aIdx;

    fixHeight(tree, aIdx);
    fixHeight(tree, bIdx);

    return bIdx;
}

int rotateRight(Tree *tree, int aIdx) {
    Node *a = &tree->nodes[aIdx];
    Node *b = &tree->nodes[a->left];
    int bIdx = a->left;

    a->left = b->right;
    b->right = aIdx;

    fixHeight(tree, aIdx);
    fixHeight(tree, bIdx);

    return bIdx;
}

int balance(Tree *tree, int idx) {
    fixHeight(tree, idx);
    Node *node = &tree->nodes[idx];

    if (balanced(tree, idx) == 2) {
        if (balanced(tree, node->right) < 0) {
            if (tree->logging) printf("BL ");
            node->right = rotateRight(tree, node->right);
        } else {
            if (tree->logging) printf("SL ");
        }
        return rotateLeft(tree, idx);
    } 

    if (balanced(tree, idx) == -2) {
        if (balanced(tree, node->left) > 0) {
            if (tree->logging) printf("BR ");
            node->left = rotateLeft(tree, node->left);
        } else {
            if (tree->logging) printf("SR ");
        }
        return rotateRight(tree, idx);
    }

    return idx;
}

int insert(Tree *tree, int idx, int key) {
    if (idx == NO_NODE) {
        return initNode(tree, key);
    }

    Node *node = &tree->nodes[idx];

    if (key < node->key) {
        node->left = insert(tree, node->left, key);
    } else {
        node->right = insert(tree, node->right, key);
    }

    return balance(tree, idx);
}
