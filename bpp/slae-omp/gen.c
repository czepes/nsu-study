#include "slae.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  double *A = malloc(N * N * sizeof(double));
  double *b = malloc(N * sizeof(double));

  if (!A || !b) {
    fprintf(stderr, "Failed to initialize A & b\n");
    if (A) {
      free(A);
    }
    if (b) {
      free(b);
    }
    return 1;
  }

  gen_slae(A, b, N);
  print_slae(A, b, N, SHOW);
  write_slae(A, b, N);

  free(A);
  free(b);
  return 0;
}
