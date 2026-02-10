#include "graph.h"

Graph* initGraph(int vertices, int edges) {
    Graph *graph = malloc(sizeof(Graph));

    if (graph == NULL) return NULL;

    graph->edges = malloc(edges * sizeof(Edge));
    graph->offsets = calloc(vertices + 1, sizeof(int));
    graph->adjacency = malloc(2 * edges * sizeof(int));

    if (!graph->edges || !graph->offsets || !graph->adjacency) {
        freeGraph(graph);
        return NULL;
    }

    graph->vertices = vertices;
    graph->edgesAmount = 0;

    return graph;
}

void freeGraph(Graph *graph) {
    if (!graph) return;

    if (graph->edges) free(graph->edges);
    if (graph->offsets) free(graph->offsets);
    if (graph->adjacency) free(graph->adjacency);

    free(graph);
}

void addEdge(Graph *graph, int v1, int v2, int length) {
    if (v1 > v2) {
        int tmp = v1;
        v1 = v2;
        v2 = tmp;
    }

    Edge *edge = &graph->edges[graph->edgesAmount];

    edge->v1 = v1;
    edge->v2 = v2;
    edge->length = length;

    graph->offsets[v1 + 1]++;
    graph->offsets[v2 + 1]++;
    graph->edgesAmount++;
}

void finalizeGraph(Graph *graph) {
    for (int i = 1; i <= graph->vertices; i++) {
        graph->offsets[i] += graph->offsets[i - 1];
    }
    
    int shift[graph->vertices];
    for (int v = 0; v < graph->vertices; v++) {
        shift[v] = 0;
    }
    
    for (int i = 0; i < graph->edgesAmount; i++) {
        int from = graph->edges[i].v1;
        int to = graph->edges[i].v2;
        
        graph->adjacency[graph->offsets[from] + shift[from]++] = i;
        graph->adjacency[graph->offsets[to] + shift[to]++] = i;
    }
}

int maxEdges(int vertices) {
    return vertices * (vertices - 1) / 2;
}
