#include "mpi.h"
#include "stdio.h"

#define N 120000

int main(int argc, char **argv) {
  const int ARRAY_TAG = 123;
  const int SUM_TAG = 321;

  int a[N], b[N];
  long long s = 0;
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
  printf("Processes: %2d | Process: %2d | Block: %d\n", size, rank, block);

  if (rank == 0) {
    start = MPI_Wtime();
    for (int i = 1; i < size; i++) {
      MPI_Send(a + block * i, block, MPI_INT, i, ARRAY_TAG, MPI_COMM_WORLD);
      MPI_Send(b, N, MPI_INT, i, ARRAY_TAG, MPI_COMM_WORLD);
    }
  } else {
    MPI_Recv(a + block * rank, block, MPI_INT, 0, ARRAY_TAG, MPI_COMM_WORLD,
             &status);
    MPI_Recv(b, N, MPI_INT, 0, ARRAY_TAG, MPI_COMM_WORLD, &status);
  }

  for (int i = 0; i < block; i++) {
    for (int j = 0; j < N; j++) {
      s += a[rank * block + i] * b[j];
    }
  }

  if (rank == 0) {
    for (int i = 1; i < size; i++) {
      long long si = 0;
      MPI_Recv(&si, 1, MPI_LONG_LONG, i, SUM_TAG, MPI_COMM_WORLD, &status);
      s += si;
    }
    end = MPI_Wtime();
  } else {
    MPI_Send(&s, 1, MPI_LONG_LONG, 0, SUM_TAG, MPI_COMM_WORLD);
  }

  if (rank == 0) {
    printf("Result: %lld\n", s);
    printf("Calculation time: %f\n", end - start);
  }

  MPI_Finalize();

  return 0;
}
