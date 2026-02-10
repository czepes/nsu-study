#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  int rank;
  int size;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < size; i++) {
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == i) {
      printf("%d\n", rank);
    }
  }

  MPI_Finalize();
}
