#include "dsu.h"

DSU* initDSU(int size) {
    DSU *dsu = malloc(sizeof(DSU) + size * sizeof(int));
    if (dsu == NULL) return NULL;

    dsu->size = size;
    for (int i = 0; i < size; i++) {
        dsu->parent[i] = i;
    }

    return dsu;
}

int findDSU(DSU *dsu, int x) {
    if (dsu->parent[x] != x) {
        dsu->parent[x] = findDSU(dsu, dsu->parent[x]);
    }
    return dsu->parent[x];
}

bool unionDSU(DSU *dsu, int x, int y) {
    int parentX = findDSU(dsu, x);
    int parentY = findDSU(dsu, y);

    if (parentX != parentY) {
        dsu->parent[parentX] = parentY;
        return true;
    }

    return false;
}
