#include "graph.h"

void freeGraph(Graph *graph) {
    if (graph == NULL) return;

    for (int v = 0; v < graph->vertices; v++) {
        if (graph->edges[v] != NULL) {
            free(graph->edges[v]);
        }
    }

    if (graph->edges != NULL) {
        free(graph->edges);
    }
    free(graph);
}

Graph* initGraph(int vertices) {
    Graph *graph = malloc(sizeof(Graph));
    if (graph == NULL) {
        return NULL;
    }

    graph->vertices = vertices;
    
    graph->edges = malloc(sizeof(int*) * vertices);
    if (graph->edges == NULL) {
        freeGraph(graph);
        return NULL;
    }

    for (int from = 0; from < vertices; from++) {
        graph->edges[from] = malloc(sizeof(int) * vertices);

        if (graph->edges[from] == NULL) {
            freeGraph(graph);
            return NULL;
        }

        for (int to = 0; to < vertices; to++) {
            graph->edges[from][to] = to == from ? 0 : NO_EDGE;
        }
    }

    return graph;
}

void addEdge(Graph *graph, int from, int to, int length) {
    graph->edges[from][to] = length;
    graph->edges[to][from] = length;
}

int maxEdges(int vertices) {
    return vertices * (vertices - 1) / 2;
}
