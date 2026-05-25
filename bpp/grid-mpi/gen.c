#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc > 1) {
    printf("Generate matrices A[N1 x N2] & B[N2 x N3]\n");
    printf("N1, N2, N3 are defined in matrix.h\n");
    return 1;
  }

  double *A = malloc(N1 * N2 * sizeof(double));
  double *B = malloc(N2 * N3 * sizeof(double));

  generatem(A, B);

  printf("Generated matrices:\n A [%d x %d] =\n", N1, N2);
  printmd(A, N1, N2);
  printf("B [%d x %d] =\n", N2, N3);
  printmd(B, N2, N3);

  writem(A, B);

  free(A);
  free(B);
  return 0;
}
