#include "rbtree.h"
#include <stdio.h>

Tree* initTree(int nodes) {
    Tree *tree = malloc(sizeof(Tree) + nodes * sizeof(Node));

    if (tree == NULL) return NULL;

    tree->size = 0;
    tree->root = NO_NODE;

    return tree;
}

int initNode(Tree *tree, int parent, int key) {
    int idx = tree->size++;
    Node *node = &tree->nodes[idx];

    node->black = false;
    node->left = NO_NODE;
    node->right = NO_NODE;
    node->parent = parent;
    node->key = key;

    if (parent == NO_NODE) return idx;

    Node *parentNode = &tree->nodes[parent];
    if (key < parentNode->key) {
        parentNode->left = idx;
    } else {
        parentNode->right = idx;
    }

    return idx;
}

bool isBlack(Tree *tree, int idx) {
    return idx == NO_NODE || tree->nodes[idx].black;
}

int getHeight(Tree *tree) {
    if (tree->root == NO_NODE) return 0;

    int idx = tree->nodes[tree->root].left;
    int height = 2;

    while (idx != NO_NODE) {
        height += isBlack(tree, idx);
        idx = tree->nodes[idx].left;
    }

    return height;
}

int rotateLeft(Tree* tree, int idx) {
    if (idx == NO_NODE) return NO_NODE;
    Node *node = &tree->nodes[idx];

    int childIdx = node->right;
    Node *child = &tree->nodes[childIdx];

    child->parent = node->parent;

    if (node->parent != NO_NODE) {
        Node *parent = &tree->nodes[node->parent];
        if (parent->left == idx) {
            parent->left = childIdx;
        } else {
            parent->right = childIdx;
        }
    }

    node->right = child->left;
    if (child->left != NO_NODE) {
        tree->nodes[child->left].parent = idx;
    }

    node->parent = childIdx;
    child->left = idx;

    return childIdx;
}

int rotateRight(Tree* tree, int idx) {
    if (idx == NO_NODE) return NO_NODE;
    Node *node = &tree->nodes[idx];

    int childIdx = node->left;
    Node *child = &tree->nodes[childIdx];

    child->parent = node->parent;

    if (node->parent != NO_NODE) {
        Node *parent = &tree->nodes[node->parent];
        if (parent->left == idx) {
            parent->left = childIdx;
        } else {
            parent->right = childIdx;
        }
    }

    node->left = child->right;
    if (child->right != NO_NODE) {
        tree->nodes[child->right].parent = idx;
    }

    node->parent = childIdx;
    child->right = idx;

    return childIdx;
}

int getParent(Tree *tree, int idx) {
    if (idx == NO_NODE) return NO_NODE;
    return tree->nodes[idx].parent;
}

int getUncle(Tree *tree, int idx) {
    int pIdx = getParent(tree, idx);
    int gIdx = getParent(tree, pIdx);
    if (gIdx == NO_NODE) return NO_NODE;

    Node *gparent = &tree->nodes[gIdx];
    return gparent->left == pIdx ? gparent->right : gparent->left;
}

void balance5(Tree *tree, int idx, int pIdx, int gIdx) {
    Node *node = &tree->nodes[idx];
    Node *parent = &tree->nodes[pIdx];
    Node *gparent = &tree->nodes[gIdx];

    /* CASE 5: parent is red, uncle is black */
    /*        g-b                 p-b        */
    /*       /   \     SR(g)     /   \       */
    /*     p-r   u-b   p-r/b   n-r   g-r     */
    /*     / \   / \   g-b/r   / \   / \     */
    /*   n-r  3 4   5   ==>   1   2 3  u-b   */
    /*   / \                           / \   */
    /*  1   2                         4   5  */
    if (idx == parent->left && node->parent == gparent->left) {
        parent->black = true;
        gparent->black = false;
        rotateRight(tree, parent->parent);
        if (gIdx == tree->root) tree->root = node->parent;
    } else if (idx == parent->right && node->parent == gparent->right) {
        parent->black = true;
        gparent->black = false;
        rotateLeft(tree, parent->parent);
        if (gIdx == tree->root) tree->root = node->parent;
    }
}

void balance4(Tree *tree, int idx, int pIdx, int gIdx) {
    Node *node = &tree->nodes[idx];
    Node *parent = &tree->nodes[pIdx];
    Node *gparent = &tree->nodes[gIdx];

    /* CASE 4: parent is red, uncle is black -> CASE 5 */
    /*             g-b                  g-b            */
    /*            /   \     SL(p)      /   \           */
    /*          p-r   u-b    ==>     n-r   u-b         */
    /*          / \   / \            / \   / \         */
    /*         1 n-r 4   5          p-r 3 4   5        */
    /*        / \                  / \                 */
    /*       2   3                1   2                */
    if (idx == parent->right && node->parent == gparent->left) {
        rotateLeft(tree, node->parent);
        idx = node->left;
    } else if (idx == parent->left && node->parent == gparent->right) {
        rotateRight(tree, node->parent);
        idx = node->right;
    }

    balance5(tree, idx, getParent(tree, idx), gIdx);
}

void balance3(Tree *tree, int idx) {
    int pIdx = getParent(tree, idx);
    int gIdx = getParent(tree, pIdx);
    int uIdx = getUncle(tree, idx);

    Node *parent = &tree->nodes[pIdx];
    Node *gparent = &tree->nodes[gIdx];
    Node *uncle = &tree->nodes[uIdx];

    // CASE 3: parent and uncle are red
    if (!isBlack(tree, uIdx)) {
        parent->black = true;
        uncle->black = true;
        gparent->black = false;
        balance1(tree, parent->parent);
    } else {
        balance4(tree, idx, pIdx, gIdx);
    }
}

void balance2(Tree *tree, int idx) {
    Node *node = &tree->nodes[idx];

    // CASE 2: parent is black
    if (!isBlack(tree, node->parent)) balance3(tree, idx);
}

void balance1(Tree *tree, int idx) {
    if (idx == NO_NODE) return;
    Node *node = &tree->nodes[idx];

    // CASE 1: root node
    if (node->parent == NO_NODE) {
        node->black = true;
        tree->root = idx;
    } else {
        balance2(tree, idx);
    }
}

void insert(Tree *tree, int key) {
    int pIdx = NO_NODE;
    int idx = tree->root;

    while (idx != NO_NODE) {
        Node *node = &tree->nodes[idx];
        pIdx = idx;
        idx = key < node->key ? node->left : node->right;
    }

    idx = initNode(tree, pIdx, key);
    balance1(tree, idx);
}

