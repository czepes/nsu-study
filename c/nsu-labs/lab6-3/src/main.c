#include <stdlib.h>
#include <stdio.h>
#include "radixtree.h"

#define N 10000

int main(void) {
    int amount;

    if (scanf("%d", &amount) != 1) {
        return EXIT_FAILURE;
    }

    RadixTree *tree = initTree();
    char word[N + 1];
    int len = 0;

    for (int i = 0; i < amount; i++) {
        if (scanf("%10000s\n", word) != 1) {
            freeTree(tree);
            return EXIT_FAILURE;
        }

        len = strlen(word) + 1;

        if (len < 2) {
            freeTree(tree);
            return EXIT_FAILURE;
        }

        radixInsert(tree, word, len);
    }

    if (scanf("%10000s\n", word) != 1) {
        word[0] = '\0';
        len = 0;
    }

    amount = 0;
    char **words = autocomplete(tree, word, strlen(word), &amount);

    if (amount == 0) {
        printf("None");
    }
    for (int i = 0; i < amount; i++) {
        printf("%s ", words[i]);
        free(words[i]);
    }

    printf("\n%d\n", countNodes(tree));

    free(words);
    freeTree(tree);
    return EXIT_SUCCESS;
}
