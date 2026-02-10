#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 100000
#define M 200000

typedef struct stack {
    int size;
    int content[N];
} Stack;

void push(Stack *stack, int value) {
    stack->content[stack->size++] = value;
}

int pop(Stack *stack) {
    if (stack->size <= 0) return 0;
    return stack->content[--stack->size];
}

typedef struct edge {
    int to;
    int next;
} Edge;

typedef struct graph {
    int vertices;
    int edgesAmount;
    Edge edges[M];
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

    graph->edges[graph->edgesAmount].to = to;
    graph->edges[graph->edgesAmount].next = graph->heads[from];
    graph->heads[from] = graph->edgesAmount++;
}

bool dfs(Graph *graph, Stack *stack, int *visited, int from) {

    bool status = true;
    visited[from] = 1;

    for (
        int i = graph->heads[from];
        i >= 0 && status;
        i = graph->edges[i].next
    ) {
        int to = graph->edges[i].to;

        if (visited[to] == 1) status = false;
        if (visited[to] > 0) continue;

        status = dfs(graph, stack, visited, to) && status;
    }

    push(stack, from);
    visited[from] = 2;
    return status;
}

bool topSort(Graph *graph, Stack *stack) {

    int visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = 0;
    }

    for (int i = 0; i < graph->vertices; i++) {
        if (visited[i] != 0) continue;
        if (!dfs(graph, stack, visited, i)) return false;
    }

    return true;
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
            free(graph);
            return EXIT_FAILURE;
        }

        addEdge(graph, --from, --to);
    }

    Stack stack = {
        .size = 0
    };

    bool status = topSort(graph, &stack);

    printf("%s\n", status ? "YES" : "NO");

    if (!status) {
        free(graph);
        return EXIT_SUCCESS;
    }

    int orders[vertices];
    for (int i = 0; i < vertices; i++) {
        int popped = pop(&stack);
        orders[popped] = i + 1;
    }

    for (int i = 0; i < vertices; i++) {
        printf("%d ", orders[i]);
    }

    free(graph);
    return EXIT_SUCCESS;
}
