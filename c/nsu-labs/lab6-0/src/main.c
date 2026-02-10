#include "avltree.h"

#define N 2 * 1000000

int main(void) {
    int nodes;

    if (scanf("%d\n", &nodes) != 1) {
        printf("Empty input\n");
        return EXIT_SUCCESS;
    }

    if (nodes < 0 || nodes > N) {
        printf("Bad input\n");
        return EXIT_SUCCESS;
    }

    Tree *tree = initTree(nodes);

    if (tree == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < nodes; i++) {
        int key;

        if (scanf("%d", &key) != 1) {
            free(tree);
            printf("Bad input");
            return EXIT_SUCCESS;
        }

        tree->root = insert(tree, tree->root, key);
    }

    printf("%d\n", getHeight(tree, tree->root));

    free(tree);
    return EXIT_SUCCESS;
}
