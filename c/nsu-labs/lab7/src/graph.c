#include "graph.h"

void initGraph(Graph *graph, int vertices) {
    graph->vertices = vertices;

    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            graph->edges[i][j] = false;
        }
    }
}

bool dfs(Graph *graph, Stack *stack, int *visited, int from) {

    visited[from] = INSIDE;

    for (
        int to = 0;
        to <= graph->vertices;
        to++
    ) {
        if (!graph->edges[from][to]) continue;
        if (visited[to] == OUTSIDE) continue;
        if (visited[to] == INSIDE) return false;
        if (!dfs(graph, stack, visited, to)) return false;
    }

    push(stack, from);
    visited[from] = OUTSIDE;
    return true;
}

bool topSort(Graph *graph, Stack *stack) {

    int visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = NOT_VISITED;
    }

    for (int i = 0; i < graph->vertices; i++) {
        if (visited[i] != NOT_VISITED) continue;
        if (!dfs(graph, stack, visited, i)) return false;
    }

    return true;
}

int maxEdges(int vertices) {
    return vertices*(vertices-1)/2;
}
