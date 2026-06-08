#include "heap.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  char dumpfile[PATH_MAX] = DEFAULT_DUMPFILE;

  if (argc == 2 && strcmp("-h", argv[1]) == 0) {
    printf(
        "Read heapdump from heaptest.c\nUsage: %s [/path/to/heapdump = %s]\n",
        argv[0], dumpfile);
    return 0;
  }

  if (argc == 2) {
    realpath(argv[1], dumpfile);
  }

  FILE *file = fopen(dumpfile, "rb");
  if (file == NULL) {
    perror("fopen");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  size_t hsize = ftell(file);
  fseek(file, 0, SEEK_SET);

  if (hsize != HEAP_SIZE) {
    fprintf(stderr, "Wrong dumpfile format. Heap was not initialized properly "
                    "during heaptest.\n");
    return 1;
  }

  char *heap = malloc(hsize * sizeof(char));
  if (heap == NULL) {
    perror("malloc");
    return 1;
  }

  if (fread(heap, sizeof(char), hsize, file) != hsize) {
    perror("fread");
    fclose(file);
    return 1;
  }
  fclose(file);

  printf("[Area No] Reading heap structure from %s\n", dumpfile);

  header_t *area = (header_t *)heap;
  int idx = 0;

  while ((char *)area < heap + hsize) {
    size_t len = hlen(area);
    printf("[----+--] "
           "+----------------------------------------------+\n"
           "[Area %02d] Offset = %ld\n"
           "[Area %02d] Length = %zu (%zu | %zu)\n"
           "[Area %02d] Prev = %p\n"
           "[Area %02d] Next = %p\n",
           idx, (char *)area - heap, idx, len, REAL_SIZE(area->size),
           IS_FREE(area->size), idx, area->prev, idx, area->next);

    if (!IS_FREE(area->size) && len > 0) {
      int to_print = len > 32 ? 32 : len;
      char *data = (char *)(area + 1);

      printf("[Area %02d] Data[0..%d] = \"", idx, to_print - 1);
      for (int i = 0; i < to_print; i++) {
        char ch = data[i];
        if (32 <= ch && ch <= 127) {
          printf("%c", ch);
        } else {
          printf(".");
        }
      }
      printf("\"\n");
    } else {
      printf("[Area %02d] No Data\n", idx);
    }

    area = (header_t *)((void *)area + REAL_SIZE(area->size));
    idx++;
  }

  return 0;
}
