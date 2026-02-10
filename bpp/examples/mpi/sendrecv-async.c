#include "mpi.h"

#define N 100

int main(int argc, char **argv) {
  int rank;
  int size;
  MPI_Status status;
  int x[N], y[N], done;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < N; i++) {
    x[i] = rank;
  }

  // Asynchronous Send and Receive
  MPI_Request reqs, reqr;
  MPI_Irecv(x, N, MPI_INT, (rank + size - 1) % size, 123, MPI_COMM_WORLD,
            &reqr);
  MPI_Isend(y, N, MPI_INT, (rank + 1) % size, 123, MPI_COMM_WORLD, &reqs);

  MPI_Finalize();
  return 0;
}
