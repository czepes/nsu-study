#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

bool inRange(int value, int left, int right) {
    return left <= value && value <= right;
}

int main() {
    int vertices, edges;

    if (scanf("%d\n%d\n", &vertices, &edges) != 2) {
        printf("bad number of lines\n");
        return EXIT_SUCCESS;
    }

    if (!inRange(vertices, 0, N)) {
        printf("bad number of vertices\n");
        return EXIT_SUCCESS;
    }

    if (!inRange(edges, 0, maxEdges(vertices))) {
        printf("bad number of edges\n");
        return EXIT_SUCCESS;
    }

    Graph graph;
    initGraph(&graph, vertices);

    for (int i = 0; i < edges; i++) {
        int from, to;

        if (scanf("%d %d", &from, &to) != 2) {
            printf("bad number of lines\n");
            return EXIT_SUCCESS;
        }

        if (!inRange(from, 1, vertices) || !inRange(to, 1, vertices)) {
            printf("bad vertex\n");
            return EXIT_SUCCESS;
        }

        graph.edges[--from][--to] = true;
    }

    Stack stack;
    initStack(&stack);

    if (!topSort(&graph, &stack)) {
        printf("impossible to sort\n");
        return EXIT_SUCCESS;
    }

    for (int i = 0; i < vertices; i++) {
        printf("%d ", pop(&stack) + 1);
    }
    printf("\n");

    return EXIT_SUCCESS;
}
