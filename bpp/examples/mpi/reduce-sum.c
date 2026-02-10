#include "mpi.h"
#include <stdio.h>

#define N 10

int main(int argc, char **argv) {
  int rank;
  double x[N], y[N], s = 0., sum;
  //       rank 0    rank 1    .....    rank n
  // x: |  .....  |  .....  |        |  .....  |
  // y: |  .....  |  .....  |        |  .....  |
  //      <- N ->   <- N ->
  //                 Total size of data == n * N

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < N; i++) {
    s += x[i] * y[i];
  }
  MPI_Reduce(&s, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("Result: %lf\n", sum);
  }

  MPI_Finalize();
  return 0;
}
