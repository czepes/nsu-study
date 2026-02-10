#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "graph.h"
#include "pqueue.h"

int dijkstra(Graph *graph, int *lengths, int *path, int start, int end) {
    for (int v = 0; v < graph->vertices; v++) {
        lengths[v] = INFINITY;
        path[v] = NO_NODE;
    }
    lengths[start] = 0;

    bool visited[graph->vertices];
    for (int v = 0; v < graph->vertices; v++) {
        visited[v] = false;
    }

    PQueue queue = initPQueue();
    push(&queue, start, 0);

    int overflowCount = 0;

    while (queue.head != NO_NODE) {
        int from = pull(&queue);

        if (visited[from]) continue;
        visited[from] = true;

        for (
            int to = graph->vertices - 1;
            to >= 0;
            to--
        ) {
            if (graph->edges[from][to] == NO_EDGE) continue;

            int length = lengths[from] + graph->edges[from][to];
            if (length < 0 || lengths[from] == OVERFLOW) length = OVERFLOW;

            if (from != end && to == end && length == OVERFLOW) {
                overflowCount++;
            }

            if (visited[to]) continue;
            if (length == OVERFLOW && lengths[to] >= 0) continue;

            if (lengths[to] == INFINITY || length < lengths[to]) {
                lengths[to] = length;
                path[to] = from;
                push(&queue, to, length);
            }
        }
    }

    return overflowCount;
}

bool inRange(int value, int left, int right) {
    return left <= value && value <= right;
}

int main(void) {
    int vertices, edges;
    int start, end;

    if (scanf("%d\n", &vertices) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(vertices, 0, N)) {
        printf("bad number of vertices\n");
        return EXIT_SUCCESS;
    }

    if (scanf("%d %d\n", &start, &end) != 2) {
        return EXIT_FAILURE;
    }

    if (!inRange(start, 1, N) || !(inRange(end, 1, N))) {
        printf("bad vertex\n");
        return EXIT_SUCCESS;
    }

    if (scanf("%d\n", &edges) != 1) {
        return EXIT_FAILURE;
    }

    if (!inRange(edges, 0, maxEdges(vertices))) {
        printf("bad number of edges\n");
        return EXIT_SUCCESS;
    }

    Graph *graph = initGraph(vertices);

    for (int i = 0; i < edges; i++) {
        int from, to, length;

        if (scanf("%d %d %d", &from, &to, &length) != 3) {
            freeGraph(graph);
            printf("bad number of lines\n");
            return EXIT_SUCCESS;
        }

        if (!inRange(from, 1, vertices) || !inRange(to, 1, vertices)) {
            freeGraph(graph);
            printf("bad vertex\n");
            return EXIT_SUCCESS;
        }

        if (!inRange(length, 0, INT_MAX)) {
            freeGraph(graph);
            printf("bad length\n");
            return EXIT_SUCCESS;
        }

        addEdge(graph, --from, --to, length);
    }

    int lengths[vertices];
    int path[vertices];

    int overflowCount = dijkstra(graph, lengths, path, --start, --end);

    for (int v = 0; v < vertices; v++) {
        if (lengths[v] == OVERFLOW) printf("INT_MAX+ ");
        else if (lengths[v] == INFINITY) printf("oo ");
        else printf("%d ", lengths[v]);
    }
    printf("\n");

    if (lengths[end] == OVERFLOW && overflowCount >= 2) {
        printf("overflow\n");
    } else if (lengths[end] == INFINITY) {
        printf("no path\n");
    } else {
        for (int v = end; v != NO_NODE; v = path[v]) {
            printf("%d ", v + 1);
        }
    }

    freeGraph(graph);
    return EXIT_SUCCESS;
}
