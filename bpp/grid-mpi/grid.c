#include "matrix.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

MPI_Datatype create_coltype(int count, int blocklen, int stride) {
  MPI_Datatype type;
  MPI_Type_vector(count, blocklen, stride, MPI_DOUBLE, &type);
  MPI_Type_commit(&type);
  return type;
}

MPI_Datatype create_coltype_ext(int count, int blocklen, int stride,
                                size_t extent_size) {
  MPI_Datatype col, type;
  MPI_Type_vector(count, blocklen, stride, MPI_DOUBLE, &col);
  MPI_Type_create_resized(col, 0, extent_size * blocklen, &type);
  MPI_Type_free(&col);
  MPI_Type_commit(&type);
  return type;
}

int main(int argc, char **argv) {
  double start, end;
  int rank, size;
  int P1, P2; // P1 - grid rows, P2 - grid cols

  MPI_Init(&argc, &argv);

  start = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 3) {
    if (rank == 0) {
      printf(
          "Calculate A x B = C via 2D Cartesian Grid [P1 x P2] Distribution\n"
          "Usage: mpirun -n <P1 * P2> %s <P1> <P2>\n"
          "INFO:\n"
          "1. P1 x P2 = MPIProcs\n"
          "2. Generate matrices A, B with ./gen\n"
          "3. N1 mod P1, N2 mod P2 == 0\n",
          argv[0]);
    }
    MPI_Finalize();
    return 1;
  }

  P1 = atoi(argv[1]);
  P2 = atoi(argv[2]);

  if (P1 <= 0 || P2 <= 0) {
    if (rank == 0) {
      fprintf(stderr, "Wrong Cartesian Grid size [%d x %d]\n", P1, P2);
    }
    MPI_Finalize();
    return 1;
  }

  if (size != P1 * P2) {
    if (rank == 0) {
      fprintf(stderr, "Expecting MPIProcs == %d\n", P1 * P2);
    }
    MPI_Finalize();
    return 1;
  }

  if (N1 % P1 != 0 || N3 % P2 != 0) {
    if (rank == 0) {
      fprintf(stderr,
              "Expecting (N1 = %d mod %d = P1), (N3 = %d mod %d = P2)\n", N1,
              P1, N3, P2);
    }
    MPI_Finalize();
    return 1;
  }

  double *A;
  double *B;
  double *C;

  if (rank == 0) {
    readm(&A, &B);
    C = calloc(N1 * N3, sizeof(double));
  }

  int row_size = N1 / P1;
  int col_size = N3 / P2;

  double *A_row = malloc(row_size * N2 * sizeof(double));
  double *B_col = malloc(N2 * col_size * sizeof(double));
  double *C_block = calloc(row_size * col_size, sizeof(double));

  MPI_Comm grid_comm;
  int dims[] = {P1, P2};
  int periods[] = {0, 0};
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &grid_comm);

  int coords[2];
  MPI_Cart_coords(grid_comm, rank, 2, coords);
  int i_rank = coords[0]; // C3 or C4
  int j_rank = coords[1]; // C0 or C1 or C2

  MPI_Comm row_comm, col_comm;
  MPI_Comm_split(grid_comm, i_rank, j_rank, &row_comm);
  MPI_Comm_split(grid_comm, j_rank, i_rank, &col_comm);

  int row_rank, col_rank;
  MPI_Comm_rank(row_comm, &row_rank);
  MPI_Comm_rank(col_comm, &col_rank);

  // Send rows
  // Root is (0, 0) -> (1, 0), ...
  if (row_rank == 0) {
    MPI_Scatter(A, row_size * N2, MPI_DOUBLE, A_row, row_size * N2, MPI_DOUBLE,
                0, col_comm);
  }
  // Roots are
  // - (0, 0) -> (0, 1), (0, 2), ...
  // - (1, 0) -> (1, 1), (1, 2), ...
  // ...
  MPI_Bcast(A_row, row_size * N2, MPI_DOUBLE, 0, row_comm);

  // Send cols
  MPI_Datatype col_send_type =
      create_coltype_ext(N2, col_size, N3, sizeof(double));
  MPI_Datatype col_recv_type = create_coltype(N2, col_size, col_size);

  // Root is (0, 0) -> (0, 1), (0, 2) ...
  if (col_rank == 0) {
    MPI_Scatter(B, 1, col_send_type, B_col, 1, col_recv_type, 0, row_comm);
  }
  // Roots are
  // - (0, 0) -> (1, 0), ...
  // - (0, 1) -> (1, 1), ...
  // - (0, 2) -> (1, 2), ...
  // ...
  MPI_Bcast(B_col, N2 * col_size, MPI_DOUBLE, 0, col_comm);

  // Local matrix multiplication
  for (int i = 0; i < row_size; i++) {
    for (int j = 0; j < N2; j++) {
      double A_ij = A_row[i * N2 + j];
      for (int k = 0; k < col_size; k++) {
        C_block[i * col_size + k] += A_ij * B_col[j * col_size + k];
      }
    }
  }

  // Gathering results
  MPI_Datatype block_recv_type =
      create_coltype_ext(row_size, col_size, N3, sizeof(double));
  MPI_Datatype block_send_type = create_coltype(row_size, col_size, col_size);

  int displs[size];
  int recvcounts[size];
  if (rank == 0) {
    for (int p = 0; p < size; p++) {
      MPI_Cart_coords(grid_comm, p, 2, coords);
      // offset in sizeof(block_recv_type) values:
      // - row_size is for each block
      // - P2 is for blocks in one row
      displs[p] = coords[0] * row_size * P2 + coords[1];
      recvcounts[p] = 1;
    }
  }
  MPI_Gatherv(C_block, 1, block_send_type, C, recvcounts, displs,
              block_recv_type, 0, MPI_COMM_WORLD);

  end = MPI_Wtime();

  MPI_Comm_free(&row_comm);
  MPI_Comm_free(&col_comm);
  MPI_Comm_free(&grid_comm);

  MPI_Type_free(&col_send_type);
  MPI_Type_free(&col_recv_type);
  MPI_Type_free(&block_send_type);
  MPI_Type_free(&block_recv_type);

  free(B_col);
  free(A_row);
  free(C_block);

  if (rank == 0) {
    printf("A x B = C [%d x %d] =\n", N1, N3);
    printmd(C, N1, N3);
    printf("Calculation time = %f sec.\n", end - start);
    free(A);
    free(B);
    free(C);
  }

  MPI_Finalize();

  return 0;
}
