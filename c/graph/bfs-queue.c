#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 200000
#define M 200000

#define NO_EDGE -1;

typedef struct queueElement* QueueElement;
struct queueElement {
    QueueElement next;
    int vertex;
};

typedef struct queue* Queue;
struct queue {
    QueueElement head;
    QueueElement tail;
};

bool push(Queue queue, int vertex) {
    QueueElement next = malloc(sizeof(struct queueElement));

    if (next == NULL) {
        return false;
    }

    next->vertex = vertex;
    next->next = NULL;

    if (queue->tail != NULL) {
        queue->tail->next = next;
    }
    queue->tail = next;

    if (queue->head == NULL) {
        queue->head = next;
    }

    return true;
}

QueueElement pop(Queue queue) {
    if (queue->head == NULL) {
        return NULL;
    }

    QueueElement popped = queue->head;

    queue->head = popped->next;

    if (queue->tail == popped) {
        queue->tail = NULL;
    }

    return popped;
}

Queue initQueue() {
    Queue queue = malloc(sizeof(struct queue));

    if (queue == NULL) {
        return NULL;
    }

    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void freeQueue(Queue queue) {
    QueueElement current = queue->head;

    while (current != NULL) {
        QueueElement tmp = current->next;
        free(current);
        current = tmp;
    }

    free(queue);
}

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
    Queue queue = initQueue();

    if (queue == NULL) {
        return false;
    }

    bool visited[graph->vertices];
    for (int i = 0; i < graph->vertices; i++) {
        visited[i] = false;
    }

    push(queue, 0);
    depths[0] = 0;

    while (queue->head != NULL) {

        QueueElement popped = pop(queue);

        if (popped == NULL) {
            freeQueue(queue);
            return false;
        }

        int current = popped->vertex;
        free(popped);

        visited[current] = true;

        for (
            int i = graph->heads[current];
            i >= 0;
            i = graph->edges[i].next
        ) {
            int to = graph->edges[i].to;

            if (visited[to]) continue;

            visited[to] = true;
            depths[to] = depths[current] + 1;
            bool pushed = push(queue, to);
            
            if (!pushed) {
                freeQueue(queue);
                return false;
            }
        }
    }

    freeQueue(queue);
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
