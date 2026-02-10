#include "../pr5/matrix.h"
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

int init_file(FILE **file_ptr) {
  time_t current_time;
  time(&current_time);

  struct tm *local_time = localtime(&current_time);

  char filename[32];

  sprintf(filename, "%02d-%02d-%02dx%02d-%02d-%04d.csv", local_time->tm_hour,
          local_time->tm_min, local_time->tm_sec, local_time->tm_mday,
          local_time->tm_mon, local_time->tm_year + 1900);

  FILE *file = fopen(filename, "w");
  if (!file) {
    fprintf(stderr, "Failed to create %s\n", filename);
    return 1;
  }

  fprintf(file, "clocks;direct;reverse;random\n");
  *file_ptr = file;

  return 0;
}

void to_units(int n, char *buffer, size_t buf_size) {
  char *units[] = {"B", "KB", "MB", "GB", "TB"};
  const int step = 1024;
  int unit_idx = 0;
  float value = n;

  while (value >= step) {
    value /= step;
    unit_idx++;
  }

  snprintf(buffer, buf_size, "%.2f %s", value, units[unit_idx]);
}

void no_opt(int k) {
  if (k == 12345) {
    printf("-_-\n");
  }
}

int measure_direct(int n, int t) {
  uint64_t start;
  uint64_t end;

  int *arr = malloc(sizeof(int) * n);
  if (!arr) {
    return -1;
  }

  // direct order
  for (int i = 0; i < n - 1; i++) {
    arr[i] = i + 1;
  }
  arr[n - 1] = 0;

  // heating the cache
  int k = 0;
  for (int i = 0; i < n; i++) {
    k = arr[k];
  }
  no_opt(k);

  // measurement
  k = 0;
  start = __rdtsc();
  for (int i = 0; i < n * t; i++) {
    k = arr[k];
  }
  end = __rdtsc();
  no_opt(k);

  free(arr);
  return (end - start) / (n * t);
}

int measure_reverse(int n, int t) {
  uint64_t start;
  uint64_t end;

  int *arr = malloc(sizeof(int) * n);
  if (!arr) {
    return -1;
  }

  // reverse order
  for (int i = n - 1; i > 0; i--) {
    arr[i] = i - 1;
  }
  arr[0] = n - 1;

  // heating the cache
  int k = 0;
  for (int i = 0; i < n; i++) {
    k = arr[k];
  }
  no_opt(k);

  // measurement
  k = 0;
  start = __rdtsc();
  for (int i = 0; i < n * t; i++) {
    k = arr[k];
  }
  end = __rdtsc();
  no_opt(k);

  free(arr);
  return (end - start) / (n * t);
}

int measure_random(int n, int t) {
  uint64_t start;
  uint64_t end;

  int *arr = malloc(sizeof(int) * n);
  if (!arr) {
    return -1;
  }

  int *indices = malloc(sizeof(int) * n);
  if (!indices) {
    free(arr);
    return -1;
  }

  for (int i = 0; i < n; i++) {
    indices[i] = i;
  }

  // random order
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = indices[i];
    indices[i] = indices[j];
    indices[j] = temp;
  }
  for (int i = 0; i < n - 1; i++) {
    arr[indices[i]] = indices[i + 1];
  }
  arr[indices[n - 1]] = indices[0];

  // heating the cache
  int k = 0;
  for (int i = 0; i < n; i++) {
    k = arr[k];
  }
  no_opt(k);

  // measurement
  k = 0;
  start = __rdtsc();
  for (int i = 0; i < n * t; i++) {
    k = arr[k];
  }
  end = __rdtsc();
  no_opt(k);

  free(arr);
  free(indices);
  return (end - start) / (n * t);
}

int min(int a, int b) { return a < b ? a : b; }

void measure(FILE *file, int n, int t) {
  const int size = n / sizeof(int);
  char in_units[16];
  to_units(n, in_units, sizeof(in_units));

  int direct_clocks = INT_MAX;
  int reverse_clocks = INT_MAX;
  int random_clocks = INT_MAX;

  printf("Array size: %s\n", in_units);
  printf("Measuring direct order passage\n");
  for (int i = 0; i < t; i++) {
    direct_clocks = min(measure_direct(size, t), direct_clocks);
  }

  printf("Measuring reverse order passage\n");
  for (int i = 0; i < t; i++) {
    reverse_clocks = min(measure_reverse(size, t), reverse_clocks);
  }

  printf("Measuring random order passage\n");
  for (int i = 0; i < t; i++) {
    random_clocks = min(measure_random(size, t), random_clocks);
  }

  fprintf(file, "%s;%d;%d;%d\n", in_units, direct_clocks, reverse_clocks,
          random_clocks);
}

int main(void) {
  srand(42);
  FILE *file;

  if (init_file(&file) > 0) {
    return 1;
  }

  const int start = 1024;
  const int end = 1024 * 1024 * 32;
  const int t = 10;
  const float step = 1.2f;

  printf("Heating the CPU...\n");
  calculate(1024, 20, false);

  int n;
  for (n = start; n < end; n *= step) {
    measure(file, n, t);
  }
  if (n > end) {
    measure(file, end, t);
  }

  fclose(file);
  return 0;
}
