#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define KB 1024
#define PAGE_SIZE 4 * KB

#define BOUND 4096
#define SLEEP 1000

int writer_process(unsigned int *buffer, int length) {
  int pos = 0;

  for (unsigned long num = 0; num < BOUND; num++) {
    buffer[pos] = num;

    printf("[writer] Written [%d] = %u\n", pos, (unsigned int)num);
    fflush(stdout);

    pos = (pos + 1) % length;

    usleep(SLEEP);
  }

  return 0;
}

int reader_process(unsigned int *buffer, int length) {
  int pos = 0;
  unsigned int prev = 0;
  unsigned int cur = 0;

  for (unsigned long count = 0; count < BOUND; count++) {
    usleep(SLEEP);

    cur = buffer[pos];

    printf("[reader] Read [%d] = %u\n", pos, cur);
    fflush(stdout);

    if (pos > 0 && cur <= prev) {
      fprintf(stderr, "[reader] Sequence is invalid: [%d] = %u >= [%d] = %u\n",
              pos - 1, prev, pos, cur);
      fflush(stdout);
      return 1;
    }

    pos = (pos + 1) % length;
    prev = cur;
  }

  return 0;
}

int main(void) {
  size_t size = PAGE_SIZE;
  void *mem =
      mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

  printf("Mmaped shared memory area [%p, %p]\n", mem, mem + size - 1);
  memset(mem, 0, size);

  unsigned int *buffer = (unsigned int *)mem;
  int length = size / sizeof(unsigned int);

  pid_t pid = fork();

  switch (pid) {
  case -1: {
    fprintf(stderr, "Failed to create child process\n");
    exit(EXIT_FAILURE);
  }
  case 0: {
    printf("[writer] Writing 0...UINT_MAX to buffer\n");
    int r = writer_process(buffer, length);
    exit(r);
  }
  default: {
    printf("[reader] Reading from buffer\n");
    int r = reader_process(buffer, length);
    exit(r);
  }
  }
}
