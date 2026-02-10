#include "radixtree.h"

// NodeArray

void freeNodes(NodeArray *nodes) {
    if (!nodes) return;
    if (nodes->items) free(nodes->items);
    free(nodes);
}

NodeArray* initNodes(int capacity) {
    NodeArray *nodes = malloc(sizeof(NodeArray));
    if (!nodes) return NULL;

    nodes->items = malloc(capacity * sizeof(Node));
    if (!nodes->items) {
        free(nodes);
        return NULL;
    }

    nodes->capacity = capacity;
    nodes->size = 0;

    return nodes;
}

void pushNode(NodeArray *nodes, Node *node) {
    if (!nodes || !nodes->items) return;

    if (nodes->size == nodes->capacity) {
        Node *newItems = realloc(nodes->items, sizeof(Node) * (nodes->capacity *= 2));

        if (!newItems) {
            freeNodes(nodes);
            return;
        }

        nodes->items = newItems;
    }

    nodes->items[nodes->size++] = *node;
}

Node* getNode(NodeArray *array, int index) {
    if (!array || !array->items) return NULL;
    if (index < 0 || index >= array->size) return NULL;
    return &array->items[index];
}

// RadixTree

void freeNode(RadixTree *tree, int idx) {
    Node *node = getNode(tree->nodes, idx);

    if (!node) return;

    freeNode(tree, node->child);
    freeNode(tree, node->sibling);
}

void freeTree(RadixTree *tree) {
    if (!tree) return;

    freeNode(tree, tree->root);

    if (tree->nodes) freeNodes(tree->nodes);
    if (tree->buffer) freeText(tree->buffer);
    if (tree->stats) free(tree->stats);

    free(tree);
}

RadixTree *initTree() {
    RadixTree *tree = malloc(sizeof(RadixTree));
    if (!tree) return NULL;

    tree->nodes = initNodes(DEFAULT_SIZE);
    tree->buffer = initText(DEFAULT_SIZE);
    tree->stats = malloc(sizeof(RadixStats));

    if (!tree->nodes || !tree->buffer || !tree->stats) {
        if (tree->nodes) free(tree->nodes);
        if (tree->buffer) free(tree->buffer);
        if (tree->stats) free(tree->stats);
        free(tree);
        return NULL;
    }

    tree->stats->nodes = 0,
    tree->stats->nulls = 0,
    tree->stats->maxLen = 0,
    tree->root = NO_NODE;

    return tree;
}

int initNode(RadixTree *tree, int key, int len) {
    Node node = {
        .key = key,
        .length = len,
        .child = NO_NODE,
        .sibling = NO_NODE
    };

    pushNode(tree->nodes, &node);

    tree->stats->nodes++;
    if (tree->buffer->data[node.key] == '\0') tree->stats->nulls++;
    return tree->nodes->size - 1;
}

int commonPrefix(char *s1, int n, char *s2, int m) {
    int pos;
    for (pos = 0; pos < n && pos < m; pos++) {
        if (s1[pos] != s2[pos]) break;
    }
    return pos;
}

Node* splitNode(RadixTree *tree, int idx, int pos) {
    Node *child, *node = getNode(tree->nodes, idx);
    int childIdx = initNode(tree, node->key + pos, node->length - pos);

    node = getNode(tree->nodes, idx);
    child = getNode(tree->nodes, childIdx);

    node->length = pos;

    child->child = node->child;
    node->child = childIdx;

    return node;
}

int insertNode(RadixTree *tree, char *word, int len) {
    int key = tree->buffer->length;
    bufferAppend(tree->buffer, word, len);
    return initNode(tree, key, len);
}

void radixInsert(RadixTree *tree, char *word, int len) {
    if (!tree || !word || len < 0) return;

    if (len > tree->stats->maxLen) {
        tree->stats->maxLen = len;
    }

    if (tree->root == NO_NODE) {
        tree->root = insertNode(tree, word, len);
        return;
    }

    int idx = tree->root;
    char* suffix = word;
    int suffixLen = len;

    while (idx != NO_NODE) {
        Node *node = getNode(tree->nodes, idx);

        int pos = commonPrefix(
            tree->buffer->data + node->key, node->length,
            suffix, suffixLen
        );

        if (pos == 0) {
            if (node->sibling != NO_NODE) {
                idx = node->sibling;
                continue;
            }

            int sibling = insertNode(tree, suffix, suffixLen);
            (getNode(tree->nodes, idx))->sibling = sibling;

            break;
        } 
        
        if (pos > 0 && pos < len) {

            if (pos < node->length) {
                splitNode(tree, idx, pos);
                node = getNode(tree->nodes, idx);
            }

            suffix = suffix + pos;
            suffixLen -= pos;

            if (node->child != NO_NODE) {
                idx = node->child;
                continue;
            }

            int child = insertNode(tree, suffix, suffixLen);
            (getNode(tree->nodes, idx))->child = child;

            break;
        }

        if (pos == len) break;
    }
}

int findPrefixNode(RadixTree *tree, int idx, char *prefix, int len, int *offset) {
    if (!tree || tree->root == NO_NODE) {
        return NO_NODE;
    }

    Node *node = getNode(tree->nodes, idx);

    if (!node) {
        return NO_NODE;
    }

    int pos = commonPrefix(tree->buffer->data + node->key, node->length, prefix, len);
    *offset = pos;

    if (pos == 0) {
        return findPrefixNode(tree, node->sibling, prefix, len, offset);
    }

    if (pos < len) {
        return findPrefixNode(tree, node->child, prefix + pos, len - pos, offset);
    }

    return idx;
}

bool isWord(RadixTree *tree, Node *node) {
    if (!node) return false;
    return tree->buffer->data[node->key + node->length - 1] == '\0';
}

char* duplicate(char *str) {
    int len = strlen(str) + 1;

    char *dup = malloc(len * sizeof(char));
    if (!dup) return NULL;

    strncpy(dup, str, len);

    return dup;
}

void addWord(char *word, int len, char **words, int *count) {
    word[len] = '\0';
    words[*count] = duplicate(word);
    (*count)++;
}

void collectWords(
    RadixTree *tree, int idx,
    char *buffer, int len,
    char **words, int *count
) {
    Node *node = getNode(tree->nodes, idx);

    if (!node) {
        return;
    }

    strncpy(buffer + len, tree->buffer->data + node->key, node->length);
    int newLen = len + node->length;

    if (isWord(tree, node)) {
        addWord(buffer, newLen, words, count);
    }

    collectWords(tree, node->child, buffer, newLen, words, count);
    collectWords(tree, node->sibling, buffer, len, words, count);
}

char** autocomplete(
    RadixTree *tree,
    char *prefix,
    int len,
    int *count
) {
    if (!tree || tree->root == NO_NODE || !prefix) return NULL;

    *count = 0;
    int offset = 0;
    int prefixIdx = findPrefixNode(tree, tree->root, prefix, len, &offset);
    Node *prefixNode = getNode(tree->nodes, prefixIdx);

    if (!prefixNode) return NULL;

    char **words = malloc(tree->stats->nodes * sizeof(char*));
    if (!words) return NULL;

    char buffer[tree->stats->maxLen + 1];

    strncpy(buffer, prefix, len - offset);
    strncpy(buffer + len - offset, tree->buffer->data + prefixNode->key, prefixNode->length);
    int newLen = len + prefixNode->length - offset;

    if (isWord(tree, prefixNode)) {
        addWord(buffer, newLen, words, count);
    }

    collectWords(tree, prefixNode->child, buffer, newLen, words, count);

    return words;
}

int countNodes(RadixTree *tree) {
    int count = tree->stats->nodes - tree->stats->nulls;
    Node *root = getNode(tree->nodes, tree->root);

    if (root) {
        count += root->sibling != NO_NODE;
    }

    return count;
}
