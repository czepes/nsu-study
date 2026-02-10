#ifndef DSU_HEADER
#define DSU_HEADER

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int size;
    int parent[];
} DSU;

DSU* initDSU(int size);
int findDSU(DSU *dsu, int x);
bool unionDSU(DSU *dsu, int x, int y);

#endif
