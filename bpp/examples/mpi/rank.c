#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
  char msg[20];
  int rank;
  int size;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("Amount of processes: %d, Process ID: %d\n", size, rank);

  MPI_Finalize();
}
