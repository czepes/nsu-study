#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include <stdlib.h>
#include "constraints.h"

#define NO_EDGE -1

typedef struct {
    int vertices;
    int **edges;
} Graph;

void freeGraph(Graph *graph);
Graph* initGraph(int vertices);
void addEdge(Graph *graph, int from, int to, int length);
int maxEdges(int vertices);

#endif
