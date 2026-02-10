#include <stdlib.h>
#include <stdio.h>

#define N 20000
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

void addEdge(Graph *graph, int from, int to) {
    int lastEdge = graph->edgesAmount;

    graph->edges[lastEdge].to = to;
    graph->edges[lastEdge].next = graph->heads[from];
    graph->heads[from] = lastEdge;

    graph->edgesAmount = ++lastEdge;
}

void dfs(Graph *graph, int from, int* components, int component) {

    if (components[from] > 0) return;

    components[from] = component;

    for (
        int edge = graph->heads[from];
        edge >= 0;
        edge = graph->edges[edge].next
    ) {
        int to = graph->edges[edge].to;
        if (components[to] > 0) continue;
        dfs(graph, to, components, component);
    }
}

int countComponents(Graph *graph, int* components) {
    int count = 0;

    for (int i = 0; i < graph->vertices; i++) {
        if (components[i] > 0) continue;
        dfs(graph, i, components, ++count);
    }

    return count;
}

int main() {
    int vertices, edges;

    if (scanf("%d %d", &vertices, &edges) != 2) {
        return EXIT_FAILURE;
    }

    Graph *graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        return EXIT_FAILURE;
    }

    graph->vertices = vertices;
    graph->edgesAmount = 0;

    int components[N];

    for (int i = 0; i < vertices; i++) {
        graph->heads[i] = -1;
        components[i] = 0;
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

    int count = countComponents(graph, components);

    printf("%d\n", count);
    for (int i = 0; i < vertices; i++) {
        printf("%d ", components[i]);
    }
    printf("\n");

    free(graph);
    return EXIT_SUCCESS;
}
