#include "mpi.h"
#include "stdio.h"

#define N 120000

int main(int argc, char **argv) {
  int a[N], b[N];
  long long s = 0;
  double start, end;

  for (int i = 0; i < N; i++) {
    a[i] = i;
    b[i] = N - i;
  }

  MPI_Init(&argc, &argv);
  start = MPI_Wtime();

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      s += a[i] * b[j];
    }
  }

  end = MPI_Wtime();
  MPI_Finalize();

  printf("Result: %lld\n", s);
  printf("Calculation time: %f\n", end - start);
  return 0;
}
