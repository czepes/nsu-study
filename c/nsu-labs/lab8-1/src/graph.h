#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include <stdlib.h>

typedef struct {
    int v1;
    int v2;
    int length;
} Edge;

typedef struct {
    int vertices;
    int edgesAmount;
    int *adjacency;
    int *offsets;
    Edge* edges;
} Graph;

Graph* initGraph(int vertices, int edges);
void freeGraph(Graph *graph);
void addEdge(Graph *graph, int v1, int v2, int length);
void finalizeGraph(Graph *graph);
int maxEdges(int vertices);

#endif
