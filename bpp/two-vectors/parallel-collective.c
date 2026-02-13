#include "mpi.h"
#include "stdio.h"

#define N 120000

int main(int argc, char **argv) {
  const int ARRAY_TAG = 123;
  const int SUM_TAG = 321;

  int a[N], b[N];
  long long s = 0, si = 0;
  int size, rank;
  double start, end;
  MPI_Status status;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    for (int i = 0; i < N; i++) {
      a[i] = i;
      b[i] = N - i;
    }
  }

  const int block = N / size;
  int c[block];
  printf("Processes: %2d | Process: %2d | Block: %d\n", size, rank, block);

  if (rank == 0) {
    start = MPI_Wtime();
  }

  MPI_Scatter(a, block, MPI_INT, c, block, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(b, N, MPI_INT, 0, MPI_COMM_WORLD);

  for (int i = 0; i < block; i++) {
    for (int j = 0; j < N; j++) {
      si += c[i] * b[j];
    }
  }

  MPI_Reduce(&si, &s, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    end = MPI_Wtime();
    printf("Result: %lld\n", s);
    printf("Calculation time: %f\n", end - start);
  }

  MPI_Finalize();

  return 0;
}
