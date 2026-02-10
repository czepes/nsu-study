#ifndef GRAPH_HEADER
#define GRAPH_HEADER

#include <stdbool.h>
#include "constraints.h"
#include "stack.h"
	
#define NO_VERTEX -1
#define NOT_VISITED 0
#define INSIDE 1
#define OUTSIDE 2

typedef struct {
    int vertices;
    bool edges[N + 1][N + 1];
} Graph;

void initGraph(Graph *graph, int vertices);
bool dfs(Graph *graph, Stack *stack, int *visited, int from);
bool topSort(Graph *graph, Stack *stack);
int maxEdges(int vertices);

#endif
