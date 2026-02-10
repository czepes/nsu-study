#ifndef QUICKSORT_HEADER
#define QUICKSORT_HEADER

typedef enum {
    MORE = 1,
    LESS = -1,
    EQUAL = 0,
} CompareResult;

void swap(int *a, int *b);
int partition(int* arr, int left, int right, CompareResult (*compare) (int*, int, int));
void quicksort(int* arr, int left, int right, CompareResult (*compare) (int*, int, int));

#endif
