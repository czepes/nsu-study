#include <stdlib.h>
#include <stdio.h>

#define WEIGHT 0
#define PRICE 1

typedef struct backpack {
    int capacity;
    int items;
    int packed[][2];
} Backpack;

Backpack* initBackpack(int capacity, int items) {
    Backpack *backpack = malloc(sizeof(Backpack) + sizeof(int[2]) * items);

    if (backpack == NULL) {
        return NULL;
    }

    backpack->capacity = capacity;
    backpack->items = 0;

    return backpack;
}

int max(int a, int b) {
    return a > b ? a : b;
}

int fillBackpack(
    int items,
    int weights[items],
    int prices[items],
    Backpack *backpack
) {
    int capacity = backpack->capacity;
    int dp[items + 1][capacity + 1];

    for (int i = 0; i < items + 1; i++) {
        for (int w = 0; w < capacity + 1; w++) {

            if (i == 0 || w == 0) {
                dp[i][w] = 0;
                continue;
            }

            if (weights[i - 1] > w) {
                dp[i][w] = dp[i - 1][w];
                continue;
            }

            int keep = dp[i - 1][w];
            int add = dp[i - 1][w - weights[i - 1]] + prices[i - 1];

            dp[i][w] = max(add, keep);
        }
    }

    for (
        int i = items, w = capacity;
        i > 0 && w > 0;
        i--
    ) {
        if (dp[i][w] == dp[i - 1][w]) continue;

        backpack->packed[backpack->items][WEIGHT] = weights[i - 1];
        backpack->packed[backpack->items][PRICE] = prices[i - 1];
        backpack->items++;

        w -= weights[i - 1];
    }

    return dp[items][capacity];
}

int main(void) {
    int items, capacity;

    if (scanf("%d %d", &items, &capacity) != 2) {
        return EXIT_FAILURE;
    }

    int weights[items];
    int prices[items];

    for (int i = 0; i < items; i++) {

        if (scanf("%d %d", weights + i, prices + i) != 2) {
            return EXIT_FAILURE;
        }
    }

    Backpack *backpack = initBackpack(capacity, items);

    if (backpack == NULL) {
        return EXIT_FAILURE;
    }

    int result = fillBackpack(items, weights, prices, backpack);

    printf("%d\n", result);
    for (int i = backpack->items - 1; i >= 0; i--) {
        int weight = backpack->packed[i][WEIGHT];
        int price = backpack->packed[i][PRICE];

        printf("%d %d\n", weight, price);
    }

    free(backpack);
    return EXIT_SUCCESS;
}
