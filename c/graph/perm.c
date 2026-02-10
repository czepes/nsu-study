#include <stdlib.h>
#include <stdio.h>

#define N 100000
#define M 100000

#define NOT_SEEN 0
#define SEEN 1
#define ADDED 2

#define NO_EDGE -1;

typedef struct edge {
    int next;
    int to;
} Edge;

typedef struct graph {
    int vertices;
    int edgesAmount;
    int heads[N];
    Edge edges[M * 2];
} Graph;

void addEdge(Graph *graph, int from, int to) {

    graph->edges[graph->edgesAmount].to = to;
    graph->edges[graph->edgesAmount].next = graph->heads[from];

    graph->heads[from] = graph->edgesAmount++;
}

Graph* initGraph(int vertices) {

    Graph *graph = malloc(sizeof(Graph));

    if (graph == NULL) {
        return NULL;
    }

    graph->vertices = vertices;
    graph->edgesAmount = 0;

    for (int i = 0; i < vertices; i++) {
        graph->heads[i] = NO_EDGE;
    }

    return graph;
}

void minPerm(Graph *graph, int *perm) {

    int visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = NOT_SEEN;
    }
    
    visited[0] = SEEN;

    for (int i = 0; i < graph->vertices; i++) {

        int from;

        for (int j = 0; j < graph->vertices; j++) {

            if (visited[j] != SEEN) continue;

            from = j;
            break;
        }

        visited[from] = ADDED;
        perm[i] = from;

        for (
            int edge = graph->heads[from];
            edge >= 0;
            edge = graph->edges[edge].next
        ) {
            int to = graph->edges[edge].to;

            if (visited[to] != NOT_SEEN) continue;

            visited[to] = SEEN;
        }
    }
}

int main() {
    int vertices = N, edges = M;

    if (scanf("%d %d", &vertices, &edges) != 2) {
        return EXIT_FAILURE;
    }

    Graph *graph = initGraph(vertices);

    if (graph == NULL) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < edges; i++) {
        
        int from, to;

        if (scanf("%d %d", &from ,&to) != 2) {
            free(graph);
            return EXIT_FAILURE;
        }

        addEdge(graph, --from, --to);
        addEdge(graph, to, from);
    }

    int perm[vertices];
    for (int i = 0; i < vertices; i++) {
        perm[i] = vertices;
    }

    minPerm(graph, perm);

    for (int i = 0; i < vertices; i++) {
        printf("%d ", perm[i] + 1);
    }

    free(graph);
    return EXIT_SUCCESS;
}
