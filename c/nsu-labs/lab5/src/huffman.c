#include "huffman.h"

void freeNodes(int amount, Node *nodes[amount]) {
    for (int i = 0; i < amount; i++) {
        freeTree(nodes[i]);
    }
}

int countChars(Reader *reader, int counter[CHARS]) {
    int count = 0;

    while (readerRead(reader) > 0) {
        for (int i = 0; i < reader->length; i++) {
            unsigned char ch = reader->buffer[i];
            if (counter[ch] == 0) count++;
            counter[ch]++;
        }
    }

    readerRestart(reader);
    return count;
}


Node* buildTree(int amount, int counter[CHARS]) {
    if (amount < 2) return NULL;

    Node *nodes[amount];
    int current = 0;

    for (int c = 0; c < CHARS && current < amount; c++) {
        if (counter[c] == 0) continue;

        nodes[current] = initLeafNode(counter[c], c);

        if (!nodes[current]) {
            freeNodes(amount, nodes);
            return NULL;
        }

        current++;
    }

    MinHeap *heap = initMinHeap(CHARS);
    if (!heap) {
        freeNodes(amount, nodes);
        return NULL;
    }

    for (int i = 0; i < amount; i++) {
        insertMin(heap, i, nodes[i]->weight);
    }

    while (heap->size > 1) {
        int left = extractMin(heap);
        int right = extractMin(heap);

        Node *root = joinNodes(nodes[left], nodes[right]);
        if (!root) {
            freeNodes(amount, nodes);
            return NULL;
        }

        nodes[left] = root;
        nodes[right] = NULL;

        insertMin(heap, left, nodes[left]->weight);
    }

    Node *tree = nodes[extractMin(heap)];

    free(heap);
    return tree;
}

void dfsWrite(Node *node, BitWriter *writer) {
    if (!node) return;

    bool leaf = isLeaf(node);

    writeBit(writer, leaf);
    if (leaf) writeByte(writer, node->ch);

    dfsWrite(node->left, writer);
    dfsWrite(node->right, writer);
}

void buildDict(Node *node, char dict[CHARS][CODE_LEN], char *code, int *length) {
    if (!node) return;

    if (node->left) {
        code[*length] = '0';
        *length += 1;
        buildDict(node->left, dict, code, length);
    }

    if (isLeaf(node)) {
        strncpy(dict[node->ch], code, *length);
        *length -= 1;
    }

    if (node->right) {
        code[*length] = '1';
        *length += 1;
        buildDict(node->right, dict, code, length);
        *length -= 1;
    }
}

int encodeText(BitWriter *writer, Reader *reader, char dict[CHARS][CODE_LEN]) {
    while (readerRead(reader) > 0) {
        for (int i = 0; i < reader->length; i++) {
            unsigned char ch = reader->buffer[i];
            for (int j = 0; j < (int) strlen(dict[ch]); j++) {
                writeBit(writer, dict[ch][j] == '1');
            }
        }
    }

    return flushBits(writer);
}

void compress() {
    Reader *reader = initReader(stdin);
    BitWriter *writer = initBitWriter(stdout);

    if (!reader || !writer) {
        if (reader) free(reader);
        if (writer) free(writer);
        return;
    }

    int counter[CHARS] = {0};
    int amount = countChars(reader, counter);

    if (amount == 1) {
        unsigned char pseudo = 0;
        if (counter[pseudo]) pseudo++;

        amount++;
        counter[pseudo]++;
    }

    Node *tree = buildTree(amount, counter);

    if (!tree) {
        free(reader);
        free(writer);
        return;
    }

    dfsWrite(tree, writer);
    flushBits(writer);

    char code[CODE_LEN] = "";
    char dict[CHARS][CODE_LEN] = {""};

    int codeLen = 0;
    buildDict(tree, dict, code, &codeLen);

    fpos_t trailPos;
    fgetpos(stdout, &trailPos);
    printf("%c", 0);

    char trail = encodeText(writer, reader, dict);

    fsetpos(stdout, &trailPos);
    printf("%c", trail);

    free(reader);
    free(writer);
    freeTree(tree);
}

Node* dfsRead(BitReader *reader) {
    if (feof(stdin)) return NULL;

    bool leaf = readBit(reader);

    if (leaf) {
        char byte = readByte(reader);
        return initLeafNode(0, byte);
    }

    Node *node = initNode(0);

    if (!node) return NULL;

    node->left = dfsRead(reader);
    node->right = dfsRead(reader);

    if (!node->left || !node->right) {
        if (node->left) freeTree(node->left);
        if (node->right) freeTree(node->right);
        return NULL;
    }

    return node;
}

void decompress() {
    BitReader *reader = initBitReader(stdin, 0);

    Node *tree = dfsRead(reader);
    dumpByte(reader);

    if (!tree) {
        free(reader);
        return;
    }

    reader->trail = readByte(reader);

    Node *node = tree;

    while (!readerFinished(reader)) {
        bool bit = readBit(reader);
        node = bit ? node->right : node->left;
        if (isLeaf(node)) {
            printf("%c", node->ch);
            node = tree;
        }
    }

    free(reader);
    freeTree(tree);
}

