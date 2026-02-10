#include "matrix.h"
#include <stdio.h>

int parseargs(int argc, char **argv, int *n, int *m) {
  if (argc < 3) {
    fprintf(stderr, "Missing parameters:\n");
    if (argc < 2) {
      fprintf(stderr, " [N] in [%d, %d] multiple of %d\n", MIN_N, MAX_N, BLOCK);
    }
    if (argc < 3) {
      fprintf(stderr, " [M] in [%d, %d]\n", MIN_M, MAX_M);
    }
    return 1;
  }

  const int dim = atoi(argv[1]);
  const int iterations = atoi(argv[2]);

  if (dim < MIN_N || dim > MAX_N || dim % BLOCK) {
    fprintf(stderr,
            "Wrong value N = %d\n "
            "Expected: [N] in [%d, %d] multiple of %d\n",
            dim, MIN_N, MAX_N, BLOCK);
    return 1;
  }

  if (iterations < MIN_M || iterations > MAX_M) {
    fprintf(stderr,
            "Wrong value M = %d\n "
            "Expected: [M] in [%d, %d]\n",
            iterations, MIN_M, MAX_M);
    return 1;
  }

  *n = dim;
  *m = iterations;

  return 0;
}

int main(int argc, char **argv) {
  int dim;
  int iterations;

  if (parseargs(argc, argv, &dim, &iterations) > 0) {
    return 1;
  }

  calculate(dim, iterations, true);

  return 0;
}
