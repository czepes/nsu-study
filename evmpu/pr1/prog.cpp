#include <cstdlib>
#include <iostream>
#include <stdexcept>
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
  long clocks;

  int runs = 1;
  double pi;
  long long n;

  if (argc < 2) {
    std::cout << "Missing argument N [long long]" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    n = std::stoll(argv[1]);
    if (argc == 3) {
      runs = std::stoi(argv[2]);
    }
  } catch (const std::invalid_argument &e) {
    std::cout << "Invalid argument: " << e.what() << std::endl;
    return EXIT_FAILURE;
  } catch (const std::out_of_range &e) {
    std::cout << "Out of range: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  double total_time = 0;

  std::cout << "Leibniz precision = " << n << std::endl;

  for (int i = 0; i < runs; i++) {
    times(&start);
    pi = leibniz(n);
    times(&end);

    clocks = end.tms_utime - start.tms_utime;
    double time = (double)clocks / clocks_per_sec;
    total_time += time;

    printf("Calculated PI = %.19g\n", pi);
    printf("Calculation took %.5g sec.\n", time);
  }

  if (runs > 1) {
    std::cout << "Average calculation time: " << total_time / runs << " sec."
              << std::endl;
  }

  return EXIT_SUCCESS;
}
