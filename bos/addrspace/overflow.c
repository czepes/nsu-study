#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#define KB 1024
#define PAGE 4 * KB

const size_t default_stack_size = 10;
const size_t default_heap_size = 10;
const int default_heap_iters = 100;
const useconds_t sleep_time = 1e5;

void stack_overflow(int i) {
  size_t buf_size = default_stack_size * PAGE;
  char stack[buf_size];
  printf("[%d] stack[%zu]: %p - %p\n", i, buf_size, stack, stack + buf_size);
  usleep(sleep_time);
  stack_overflow(i + 1);
}

void heap_overflow(int iters, size_t buf_pages) {
  size_t buf_size = buf_pages * PAGE;
  char *heaps[iters];
  int i = 0;

  for (i = 0; i < iters; i++) {
    heaps[i] = malloc(buf_size);
    if (heaps[i]) {
      printf("[%d] allocated heap[%zu]: %p - %p\n", i, buf_size, heaps[i],
             heaps[i] + buf_size);
    } else {
      printf("[%d] Failed to allocate heap[%zu]\n", i, buf_size * PAGE);
      break;
    }
    usleep(sleep_time);
  }

  for (int j = i - 1; j >= 0; j--) {
    free(heaps[j]);
    usleep(sleep_time);
    printf("[%d] freed heap[%zu]: %p - %p\n", j, buf_size, heaps[j],
           heaps[j] + buf_size);
  }
}

int main(int argc, char **argv) {
  char mode = 's';

  if (argc >= 2) {
    mode = argv[1][0];
  } else {
    printf("Usage: %s {s | h}\n Modes: s = stack, h = heap\n", argv[0]);
    return 1;
  }

  printf("$ watch -n1 -d cat /proc/%d/maps\n", getpid());
  sleep(5);

  switch (mode) {
  default:
  case 's': {
    stack_overflow(0);
    break;
  }
  case 'h': {
    int iters = argc >= 3 ? atoi(argv[2]) : default_heap_iters;
    int buf_pages = argc >= 4 ? atoi(argv[3]) : default_heap_size;
    heap_overflow(iters, buf_pages);
    break;
  }
  }

  return 0;
}
