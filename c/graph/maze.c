#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 1000

#define WALL '*'
#define PATH ' '
#define EOL '\n'

#define NO_VERTEX -1

#define DIRECTIONS 4
#define YX 2
#define Y 0
#define X 1

const int directions[DIRECTIONS][YX] = {
    {-1, 0},
    {0, 1},
    {1, 0},
    {0, -1}
};

void dfs(int v, bool visited[v], int size, int maze[size][size], int y, int x) {

    visited[maze[y][x]] = true;

    for (int i = 0; i < DIRECTIONS; i++) {

        int ny = y + directions[i][Y];
        if (ny < 0 || ny >= size) continue;

        int nx = x + directions[i][X];
        if (nx < 0 || nx >= size) continue;

        if (
            maze[ny][nx] == NO_VERTEX
            || visited[maze[ny][nx]]
        ) continue;

        dfs(v, visited, size, maze, ny, nx);
    }

    return;
}

int countComponents(int vertices, int size, int maze[size][size]) {

    bool visited[vertices];

    for (int i = 0; i < vertices; i++) {
        visited[i] = false;
    }

    int count = 0;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {

            if (
                maze[y][x] == NO_VERTEX
                || visited[maze[y][x]]
            ) continue;

            dfs(vertices, visited, size, maze, y, x);
            count++;
        }
    }

    return count;
}

int main() {
    int size = N;

    if (scanf("%d\n", &size) != 1) {
        return EXIT_FAILURE;
    }

    int maze[size][size];
    int vertex = 0;

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
 
            char cell;
 
            if (scanf("%c", &cell) != 1) {
                return EXIT_FAILURE;
            }
 
            if (cell == WALL) {
                maze[y][x] = NO_VERTEX;
                continue;
            }
 
            maze[y][x] = vertex;
            vertex++;
        }
        scanf("\n");
    }

    int count = countComponents(vertex, size, maze);
    printf("%d\n", count - 1);

    return EXIT_SUCCESS;
}
