#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "dsu.h"

#define N 5000
#define M N * (N - 1) / 2

typedef struct {
    int from;
    int to;
    int length;
} Edge;

int maxEdges(int vertices) {
    return vertices * (vertices - 1) / 2;
}

bool inRange(int value, int left, int right) {
    return left <= value && value <= right;
}

int compareEdges(const void *a, const void *b) {
    Edge *edge1 = (Edge*) a;
    Edge *edge2 = (Edge*) b;
    return edge1->length - edge2->length;
}

bool kruskal(int vertices, int edgesAmount, Edge edges[edgesAmount], bool tree[edgesAmount]) {
    qsort(edges, edgesAmount, sizeof(Edge), compareEdges);

    bool visited[vertices];
    visited[0] = true;
    for (int v = 1; v < vertices; v++) {
        visited[v] = false;
    }

    DSU *dsu = initDSU(vertices);
    if (dsu == NULL) return false;

    for (int e = 0; e < edgesAmount; e++) {
        Edge edge = edges[e];
        if (unionDSU(dsu, edge.from, edge.to)) {
            tree[e] = true;
            visited[edge.from] = true;
            visited[edge.to] = true;
        }
    }

    free(dsu);

    for (int v = 0; v < vertices; v++) {
        if (!visited[v]) return false;
    }
    return true;
}

int main(void) {
    int vertices, edgesAmount;

    if (scanf("%d\n", &vertices) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(vertices, 0, N)) {
        printf("bad number of vertices\n");
        return EXIT_SUCCESS;
    }

    if (scanf("%d\n", &edgesAmount) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(edgesAmount, 0, maxEdges(vertices))) {
        printf("bad number of edges\n");
        return EXIT_SUCCESS;
    }

    Edge *edges = malloc(edgesAmount * sizeof(Edge));

    if (edges == NULL) {
        return EXIT_FAILURE;
    }

    bool *tree = malloc(edgesAmount * sizeof(bool));

    if (tree == NULL) {
        free(edges);
        return EXIT_FAILURE;
    }

    for (int e = 0; e < edgesAmount; e++) {
        int from, to, length;

        if (scanf("%d %d %d", &from, &to, &length) != 3) {
            printf("bad number of lines\n");
            free(edges);
            free(tree);
            return EXIT_SUCCESS;
        }

        if (!inRange(from, 1, vertices) || !inRange(to, 1, vertices)) {
            printf("bad vertex\n");
            free(edges);
            free(tree);
            return EXIT_SUCCESS;
        }

        if (!inRange(length, 0, INT_MAX)) {
            printf("bad length\n");
            free(edges);
            free(tree);
            return EXIT_SUCCESS;
        }

        edges[e].length = length;
        edges[e].from = --from;
        edges[e].to = --to;

        tree[e] = false;
    }

    if (vertices == 0 || !kruskal(vertices, edgesAmount, edges, tree)) {
        printf("no spanning tree\n");
    } else {
        for (int e = 0; e < edgesAmount; e++) {
            if (!tree[e]) continue;
            Edge edge = edges[e];
            printf("%d %d\n", edge.from + 1, edge.to + 1);
        }
    }

    free(edges);
    free(tree);
    return EXIT_SUCCESS;
}
