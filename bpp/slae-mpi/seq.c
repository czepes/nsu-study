#include "slae.h"
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  double start, end;

  printf("Sequential Simple Iteration");

  MPI_Init(&argc, &argv);
  start = MPI_Wtime();

  double *A = NULL;
  double *b = NULL;
  double *x = NULL;
  init_slae(&A, &b, &x, N, H);

  if (!A || !b || !x) {
    fprintf(stderr, "Failed to allocate A, b, x\n");
    if (A) {
      free(A);
    }
    if (b) {
      free(b);
    }
    if (x) {
      free(x);
    }
    return 1;
  }

  if (read_slae(A, b, N) != 0) {
    fprintf(stderr, "Failed to open A, b files\n");
    free(A);
    free(b);
    free(x);
    return 1;
  }

  print_slae(A, b, N, SHOW);

  bool conv_history[H];
  for (int i = 0; i < H; i++) {
    conv_history[i] = false;
  }

  bool solved = false;
  int k = 0;
  int iter = 0;

  while (!solved && iter < MAX_ITER) {
    int next_k = ((k + 1) % H);

    double *x_cur = x + k * N;
    double *x_next = x + next_k * N;

    double delta_sqr = 0.;
    for (int i = 0; i < N; i++) {
      double Ax_i = 0.;
      for (int j = 0; j < N; j++) {
        Ax_i += A[i * N + j] * x_cur[j];
      }
      x_next[i] = x_cur[i] - TAU * (Ax_i - b[i]);
      double diff = x_cur[i] - x_next[i];
      delta_sqr += diff * diff;
    }

    conv_history[next_k] = delta_sqr < EPS2;
    if (conv_history[next_k]) {
      solved = true;
      for (int j = 0; j < H; j++) {
        solved = solved && conv_history[j];
      }
    }

    if (iter % 100 == 0 || solved) {
      printf("Iter %6d: delta_sq=%e, solved=%c\n", iter, delta_sqr,
             solved ? 'T' : 'F');
    }

    k = next_k;
    iter++;
  }

  end = MPI_Wtime();
  MPI_Finalize();

  double *Ax = malloc(N * sizeof(double));
  double *x_final = x + k * N;

  printf("\n=== Results ===\n");
  printf("Time: %.4f sec\n", end - start);
  printf("Iterations: %d\n", --iter);

  printf("\nFinal solution x:\n");
  print_vector(x_final, N, SHOW);

  matrix_vector_mul(A, x_final, N, Ax);

  printf("\nVector Ax:\n");
  print_vector(Ax, N, SHOW);

  printf("\nVector b:\n");
  print_vector(b, N, SHOW);

  double max_error = 0.0;
  for (int i = 0; i < N; i++) {
    double error = fabs(Ax[i] - b[i]);
    if (error > max_error) {
      max_error = error;
    }
  }

  printf("\nMax error: %e\n", max_error);

  free(A);
  free(Ax);
  free(b);
  free(x);
  return 0;
}
