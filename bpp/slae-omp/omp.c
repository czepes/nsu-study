#include "slae.h"
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  double start, end;

  printf("OpenMP SLAE\n");

  if (argc >= 2) {
    int threads = atoi(argv[1]);
    omp_set_num_threads(threads);
  }

  if (argc >= 4) {
    int chunk = atoi(argv[3]);
    char *typename = argv[2];
    omp_sched_t type = omp_sched_auto;
    if (strcmp(typename, "static") == 0) {
      type = omp_sched_static;
    } else if (strcmp(typename, "dynamic") == 0) {
      type = omp_sched_dynamic;
    } else if (strcmp(typename, "guided") == 0) {
      type = omp_sched_guided;
    } else {
      type = omp_sched_auto;
    }
    omp_set_schedule(type, chunk);
  }

#pragma omp parallel master
  {
    omp_sched_t type;
    int chunk;
    omp_get_schedule(&type, &chunk);
    printf("Number of threads = %d\nSchedule = ", omp_get_num_threads());
    switch (type) {
    case omp_sched_static: {
      printf("static");
      break;
    }
    case omp_sched_dynamic: {
      printf("dynamic");
      break;
    }
    case omp_sched_guided: {
      printf("guided");
      break;
    }
    case omp_sched_auto: {
      printf("auto");
      break;
    }
    default: {
      printf("unknown (%d)", type);
    }
    }
    printf(", chunk = %d\n", chunk);
  }

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

  int conv_history = 0;
  bool solved = false;
  int k = 0;
  int next_k = (k + 1) % H;
  int iter = 0;
  double delta_sqr = 0.;

  start = omp_get_wtime();

#pragma omp parallel
  while (!solved && iter < MAX_ITER) {
    double *x_cur = x + k * N;
    double *x_next = x + next_k * N;

#pragma omp for schedule(runtime) reduction(+ : delta_sqr)
    for (int i = 0; i < N; i++) {
      double Ax_i = 0.;
      for (int j = 0; j < N; j++) {
        Ax_i += A[i * N + j] * x_cur[j];
      }
      x_next[i] = x_cur[i] - TAU * (Ax_i - b[i]);
      double diff = x_cur[i] - x_next[i];
      delta_sqr += diff * diff;
    }

#pragma omp single
    {
      conv_history += delta_sqr < EPS2 ? +1 : -1;
      if (conv_history < 0) {
        conv_history = 0;
      }
      solved = conv_history >= H;

      if (iter % 100 == 0 || solved) {
        printf("Iter %6d: delta_sq=%e, solved=%c\n", iter, delta_sqr,
               solved ? 'T' : 'F');
      }

      k = next_k;
      next_k = (k + 1) % H;
      delta_sqr = 0.;
      iter++;
    }
  }

  end = omp_get_wtime();

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

  free(A);
  free(Ax);
  free(b);
  free(x);
  return 0;
}
