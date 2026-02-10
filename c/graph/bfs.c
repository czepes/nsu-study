#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 200000
#define M 200000

#define NO_EDGE -1;

typedef struct edge {
    int next;
    int to;
} Edge;

typedef struct graph {
    int vertices;
    int edgesAmount;
    int heads[N];
    Edge edges[M];
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

bool bfs(Graph *graph, int *depths) {
    int queue[graph->vertices];
    int head = 0;
    int tail = 0;

    bool visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = false;
    }

    queue[tail++] = 0;
    depths[0] = 0;

    while (head < tail) {

        int current = queue[head++];
        visited[current] = true;

        for (
            int i = graph->heads[current];
            i >= 0;
            i = graph->edges[i].next
        ) {
            int to = graph->edges[i].to;

            if (visited[to]) continue;

            queue[tail++] = to; 
            depths[to] = depths[current] + 1;
        }
    }

    return true;
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
    }

    int depths[vertices];
    for (int i = 0; i < vertices; i++) {
        depths[i] = NO_EDGE;
    }

    if (!bfs(graph, depths)) {
        free(graph);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < vertices; i++) {
        printf("%d\n", depths[i]);
    }

    free(graph);
    return EXIT_SUCCESS;
}
