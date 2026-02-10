#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <x86intrin.h>

#define KB 1024
#define MB (KB * KB)

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

  fprintf(file, "fragments;clocks\n");
  *file_ptr = file;

  return 0;
}

int alloc_array(int **array, size_t size) {
  int *arr = malloc(sizeof(int) * size);

  if (!arr) {
    fprintf(stderr, "Failed to initialize array\n");
    return 1;
  }

  *array = arr;

  return 0;
}

void init_array(int *array, size_t offset, size_t fragments,
                size_t fragment_size) {
  for (size_t i = 0; i < fragment_size - 1; i++) {
    for (size_t j = 0; j < fragments - 1; j++) {
      array[offset * j + i] = offset * (j + 1) + i;
    }
    array[offset * (fragments - 1) + i] = i + 1;
  }
  array[offset * (fragments - 1) + fragment_size - 1] = 0;
}

void reduce_optimization(size_t k) {
  if (k == 12345) {
    fprintf(stdout, "༼つ´ω`༽つ");
  }
}

uint64_t min(uint64_t a, uint64_t b) { return a > b ? b : a; }

uint64_t measure(int *array, size_t fragments, size_t fragment_size,
                 int times) {
  uint64_t clocks = UINT64_MAX;
  uint64_t start, end;

  size_t k = 0;
  for (int i = 0; i < times; i++) {
    start = __rdtsc();
    for (size_t j = 0; j < fragments * fragment_size * times; j++) {
      k = array[k];
    }
    end = __rdtsc();

    clocks = min(clocks, (end - start) / (fragments * fragment_size * times));
  }
  reduce_optimization(k);

  return clocks;
}

int main(void) {
  FILE *file;

  int code = init_file(&file);
  if (code != 0) {
    return code;
  }

  const int times = 5;
  const int max_fragments = 32;
  const size_t cache = 24 * MB / sizeof(int);
  const size_t offset = cache;
  const size_t fragment_size = cache / max_fragments;
  const size_t size = offset * max_fragments;

  int *array;
  code = alloc_array(&array, size);
  if (code != 0) {
    return code;
  }

  fprintf(stdout, " %9s | %6s\n", "fragments", "clocks");

  for (int fragments = 1; fragments <= max_fragments; fragments++) {
    fprintf(stdout, " %9d |", fragments);

    init_array(array, offset, fragments, fragment_size);
    uint64_t clocks = measure(array, fragments, fragment_size, times);

    fprintf(stdout, " %6zu\n", clocks);
    fprintf(file, "%d;%lu\n", fragments, clocks);
  }

  fclose(file);
  free(array);
  return 0;
}
