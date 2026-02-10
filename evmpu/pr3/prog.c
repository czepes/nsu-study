#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

double leibniz(long long n) {
  double pi = 0;

  for (long long k = 0, sign = 1; k < n; k++) {
    pi += (double)sign / (2 * k + 1);
    sign = -sign;
  }

  return 4 * pi;
}

int main(int argc, char **argv) {
  struct tms start, end;
  long clocks_per_sec = sysconf(_SC_CLK_TCK);

  if (argc < 2) {
    printf("Missing argument N [long long]\n");
    return EXIT_FAILURE;
  }

  long long n = atoll(argv[1]);

  printf("Leibniz precision = %lld\n", n);

  times(&start);
  double pi = leibniz(n);
  times(&end);

  long clocks = end.tms_utime - start.tms_utime;
  double time = (double)clocks / clocks_per_sec;

  printf("Calculated PI = %.19g\n", pi);
  printf("Calculation took %.5g sec.\n", time);

  return EXIT_SUCCESS;
}
