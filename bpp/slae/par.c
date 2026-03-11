#include "slae.h"
#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *sendcounts_rows;
  int *displs_rows;
  int *sendcounts_matrix;
  int *displs_matrix;
} Distribution;

Distribution distribute(int total_rows, int num_procs) {
  Distribution dist;

  // Vectors distribution
  dist.sendcounts_rows = malloc(num_procs * sizeof(int));
  dist.displs_rows = malloc(num_procs * sizeof(int));

  // Matrix distribution
  dist.sendcounts_matrix = malloc(num_procs * sizeof(int));
  dist.displs_matrix = malloc(num_procs * sizeof(int));

  int base_size = total_rows / num_procs;
  int remainder = total_rows % num_procs;

  int offset_rows = 0;
  int offset_matrix = 0;

  for (int i = 0; i < num_procs; i++) {
    int rows = (i < remainder) ? base_size + 1 : base_size;

    // Vectors distribution
    dist.sendcounts_rows[i] = rows;
    dist.displs_rows[i] = offset_rows;

    // Matrix distribution
    dist.sendcounts_matrix[i] = rows * total_rows;
    dist.displs_matrix[i] = offset_matrix;

    offset_rows += rows;
    offset_matrix += rows * total_rows;
  }

  return dist;
}

int main(int argc, char **argv) {
  int rank, num_procs;
  double start, end;

  MPI_Init(&argc, &argv);
  start = MPI_Wtime();
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  double *A = NULL;
  double *b = NULL;
  double *x = NULL;

  double *A_local = NULL;
  double *b_local = NULL;
  double *x_local = NULL;

  Distribution dist;
  int local_rows;

  // Init
  if (rank == 0) {
    printf("MPI Simple Iteration with %d processes\n", num_procs);

    init_slae(&A, &b, &x, N, H);

    if (!A || !b || !x) {
      fprintf(stderr, "Failed to allocate on rank 0\n");
      if (A) {
        free(A);
      }
      if (b) {
        free(b);
      }
      if (x) {
        free(x);
      }
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (read_slae(A, b, N) != 0) {
      fprintf(stderr, "Failed to open A, b files\n");
      free(A);
      free(b);
      free(x);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    dist = distribute(N, num_procs);

    printf("\nDistribution:\n");
    for (int i = 0; i < num_procs; i++) {
      printf("  Proc %d: %d rows, matrix offset %d, vector offset %d\n", i,
             dist.sendcounts_rows[i],
             dist.displs_matrix[i] / N, // amount of rows
             dist.displs_rows[i]);
    }
  }

  // Sending sendcounts & displs 0 -> all
  int *sendcounts_rows = NULL;
  int *displs_rows = NULL;
  int *sendcounts_matrix = NULL;
  int *displs_matrix = NULL;

  if (rank == 0) {
    sendcounts_rows = dist.sendcounts_rows;
    displs_rows = dist.displs_rows;
    sendcounts_matrix = dist.sendcounts_matrix;
    displs_matrix = dist.displs_matrix;
  } else {
    sendcounts_rows = malloc(num_procs * sizeof(int));
    displs_rows = malloc(num_procs * sizeof(int));
  }

  MPI_Bcast(sendcounts_rows, num_procs, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs_rows, num_procs, MPI_INT, 0, MPI_COMM_WORLD);

  local_rows = sendcounts_rows[rank];

  printf("Process %d: local_rows = %d\n", rank, local_rows);

  // Scattering A, b, x 0 -> all
  A_local = malloc(local_rows * N * sizeof(double));
  b_local = malloc(local_rows * sizeof(double));
  x_local = calloc(H * local_rows, sizeof(double));

  if (!A_local || !b_local || !x_local) {
    fprintf(stderr, "Process %d: Allocation failed\n", rank);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  MPI_Scatterv(A, sendcounts_matrix, displs_matrix, MPI_DOUBLE, A_local,
               local_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Scatterv(b, sendcounts_rows, displs_rows, MPI_DOUBLE, b_local, local_rows,
               MPI_DOUBLE, 0, MPI_COMM_WORLD);

  double *x_cur_global = malloc(N * sizeof(double));

  bool conv_history[H];
  for (int i = 0; i < H; i++) {
    conv_history[i] = false;
  }

  bool solved = false;
  int k = 0;
  int iter = 0;

  while (!solved && iter < MAX_ITER) {
    int next_k = (k + 1) % H;

    double *x_cur = x_local + k * local_rows;
    double *x_next = x_local + next_k * local_rows;

    // Gatheing x_local[i] to x[i] all -> all
    MPI_Allgatherv(x_cur, local_rows, MPI_DOUBLE, x_cur_global, sendcounts_rows,
                   displs_rows, MPI_DOUBLE, MPI_COMM_WORLD);

    // Iterational calculations
    double local_delta_sq = 0.0;
    for (int i = 0; i < local_rows; i++) {
      double Ax_i = 0.;
      for (int j = 0; j < N; j++) {
        Ax_i += A_local[i * N + j] * x_cur_global[j];
      }
      x_next[i] = x_cur[i] - TAU * (Ax_i - b_local[i]);
      double diff = x_cur[i] - x_next[i];
      local_delta_sq += diff * diff;
    }

    // Gathering delta all -> all
    double global_delta_sq = 0.0;
    MPI_Allreduce(&local_delta_sq, &global_delta_sq, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);

    // Checking convergence
    conv_history[next_k] = global_delta_sq < EPS2;

    solved = true;
    for (int i = 0; i < H; i++) {
      solved = solved && conv_history[i];
    }

    if (rank == 0) {
      if (iter % 100 == 0 || solved) {
        printf("Iter %6d: delta_sq=%e, solved=%c\n", iter, global_delta_sq,
               solved ? 'T' : 'F');
      }
    }

    k = next_k;
    iter++;
  }

  // Gathering final solution all -> 0
  double *x_final = NULL;
  if (rank == 0) {
    x_final = malloc(N * sizeof(double));
  }

  MPI_Gatherv(x_local + k * local_rows, local_rows, MPI_DOUBLE, x_final,
              sendcounts_rows, displs_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  end = MPI_Wtime();

  free(A_local);
  free(b_local);
  free(x_local);
  free(x_cur_global);
  free(sendcounts_rows);
  free(displs_rows);

  // Results
  if (rank == 0) {
    printf("\n=== Results ===\n");
    printf("Time: %.4f sec\n", end - start);
    printf("Iterations: %d\n", --iter);

    printf("\nFinal solution x:\n");
    print_vector(x_final, N, SHOW);

    double *Ax = malloc(N * sizeof(double));

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
    free(b);
    free(x);
    free(Ax);
    free(sendcounts_matrix);
    free(displs_matrix);
    free(x_final);
  }

  MPI_Finalize();
  return 0;
}
