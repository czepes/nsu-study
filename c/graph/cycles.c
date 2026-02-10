#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 200000
#define M 200000

typedef struct edge {
    int to;
    int next;
} Edge;

typedef struct graph {
    int vertices;
    int edgesAmount;
    Edge edges[M * 2];
    int heads[N];
} Graph;

Graph* initGraph(int vertices) {
    Graph *graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        return NULL;
    }

    graph->vertices = vertices;
    for (int i = 0; i < vertices; i++) {
        graph->heads[i] = -1;
    }

    return graph;
}

void addEdge(Graph *graph, int from, int to) {
    int lastEdge = graph->edgesAmount;

    graph->edges[lastEdge].to = to;

    graph->edges[lastEdge].next = graph->heads[from];
    graph->heads[from] = lastEdge;

    graph->edgesAmount = ++lastEdge;
}

bool isCycle(Graph *graph, bool *visited, int from) {
    visited[from] = true;

    bool result = true;
    int edgesCount = 0;

    for (
        int i = graph->heads[from];
        i >= 0;
        i = graph->edges[i].next
    ) {
        int to = graph->edges[i].to;
        edgesCount++;
        if (visited[to]) continue;
        result = isCycle(graph, visited, to) && result;
    }

    return edgesCount == 2 && result;
}

int countCycles(Graph *graph) {
    int count = 0;

    bool visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = false;
    }

    for (int i = 0; i < graph->vertices; i++) {
        if (visited[i]) continue;
        if (isCycle(graph, visited, i)) count++;
    }

    return count;
}

int main() {
    int vertices, edges;

    if (scanf("%d %d", &vertices, &edges) != 2) {
        return EXIT_FAILURE;
    }

    Graph *graph = initGraph(vertices);

    if (graph == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < edges; i++) {
        int from, to;

        if (scanf("%d %d", &from, &to) != 2) {
            return EXIT_FAILURE;
        }

        from--;
        to--;

        addEdge(graph, from, to);
        addEdge(graph, to, from);
    }

    printf("%d\n", countCycles(graph));

    free(graph);
    return EXIT_SUCCESS;
}
