#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

#define N 2 * 1000000

int main(void) {
    int nodes;

    if (scanf("%d", &nodes) != 1) {
        return EXIT_FAILURE;
    }

    Tree *tree = initTree(nodes);

    if (tree == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nodes; i++) {
        int key;

        if (scanf("%d", &key) != 1) {
            free(tree);
            return EXIT_FAILURE;
        }

        insert(tree, key);
    }

    printf("%d\n", getHeight(tree));

    free(tree);
    return EXIT_SUCCESS;
}
