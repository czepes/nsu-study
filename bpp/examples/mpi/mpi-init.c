#include "mpi.h"

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  // Some parallel work here

  MPI_Finalize();
  return 0;
}
