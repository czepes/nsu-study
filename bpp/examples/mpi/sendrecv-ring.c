#include "mpi.h"

#define N 100

int main(int argc, char **argv) {
  int size, rank;
  MPI_Request request[2];
  MPI_Status status[2];
  int x[N], y[N], done;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < N; i++) {
    x[i] = rank;
    y[i] = -1;
  }

  //  / -> 0 -> 1 -> 2 -> ... -> n -> \
  // |                                |
  // \ <-                         <- /
  MPI_Isend(x, N, MPI_INT, (rank + 1) % size, 123, MPI_COMM_WORLD, request);
  MPI_Irecv(y, N, MPI_INT, (rank + size - 1) % size, 123, MPI_COMM_WORLD,
            request + 1);
  MPI_Waitall(2, request, status);

  MPI_Finalize();
  return 0;
}
