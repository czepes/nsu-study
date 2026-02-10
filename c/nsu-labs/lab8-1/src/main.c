#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "graph.h"
#include "minheap.h"

#define N 5000
#define INFINITY -1

bool inRange(int value, int left, int right) {
    return left <= value && value <= right;
}

bool prim(Graph *graph, int *parents, int start) {
    if (graph->vertices == 0) {
        return false;
    }

    MinHeap *heap = initMinHeap(graph->vertices);
    if (heap == NULL) return false;

    int lengths[graph->vertices];
    bool visited[graph->vertices];
    for (int v = 0; v < graph->vertices; v++) {
        parents[v] = v;
        lengths[v] = INFINITY;
        visited[v] = false;
    }
    lengths[start] = 0;

    insert(heap, start, 0);

    while (heap->size > 0) {
        int from = extractMin(heap);

        if (visited[from]) continue;
        visited[from] = true;

        for (
            int i = graph->offsets[from];
            i < graph->offsets[from + 1];
            i++
        ) {
            Edge edge = graph->edges[graph->adjacency[i]];
            int to = edge.v1 == from ? edge.v2 : edge.v1;

            if (visited[to]) continue;
            if (lengths[to] != INFINITY && edge.length >= lengths[to]) {
                continue;
            }

            lengths[to] = edge.length;
            parents[to] = from;
            insert(heap, to, edge.length);
        }
    }

    free(heap);

    for (int v = 0; v < graph->vertices; v++) {
        if (!visited[v]) return false;
    }
    return true;
}

int main(void) {
    int vertices, edges;

    if (scanf("%d", &vertices) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(vertices, 0, N)) {
        printf("bad number of vertices\n");
        return EXIT_SUCCESS;
    }

    if (scanf("%d", &edges) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(edges, 0, maxEdges(vertices))) {
        printf("bad number of edges\n");
        return EXIT_SUCCESS;
    }

    Graph *graph = initGraph(vertices, edges);

    if (graph == NULL) {
        return EXIT_FAILURE;
    }

    for (int e = 0; e < edges; e++) {
        int from, to, length;

        if (scanf("%d %d %d", &from, &to, &length) != 3) {
            printf("bad number of lines\n");
            freeGraph(graph);
            return EXIT_SUCCESS;
        }

        if (!inRange(from, 1, vertices) || !inRange(to, 1, vertices)) {
            printf("bad vertex\n");
            freeGraph(graph);
            return EXIT_SUCCESS;
        }

        if (!inRange(length, 0, INT_MAX)) {
            printf("bad length\n");
            freeGraph(graph);
            return EXIT_SUCCESS;
        }

        addEdge(graph, --from, --to, length);
    }
    finalizeGraph(graph);

    int spanTree[vertices];
    int start = 0;

    if (!prim(graph, spanTree, start)) {
        printf("no spanning tree\n");
    } else {
        for (int v = 0; v < graph->vertices; v++) {
            if (v == start) continue;
            printf("%d %d\n", spanTree[v] + 1, v + 1);
        }
    }
    
    freeGraph(graph);
    return EXIT_SUCCESS;
}
