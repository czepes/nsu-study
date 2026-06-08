#include "heap.h"
#include <linux/limits.h>

int main(int argc, char **argv) {
  char dumpfile[PATH_MAX] = DEFAULT_DUMPFILE;

  if (argc == 2 && strcmp("-h", argv[1]) == 0) {
    printf("Test heap\nUsage: %s [/path/to/heapdump = %s]\n", argv[0],
           dumpfile);
    return 0;
  }

  if (argc == 2) {
    realpath(argv[1], dumpfile);
  }

  if (hinit(dumpfile) == -1) {
    perror("hinit");
    return 1;
  }

  printf("Initial free space = %zu\n", hfreelen());

  char *area1 = halloc(24 * sizeof(char));
  char *area2 = halloc(24 * sizeof(char));
  char *area3 = halloc(24 * sizeof(char));
  char *area4 = halloc(6 * sizeof(int));
  char *area5 = halloc(3 * sizeof(double));

  strncpy(area1, "Hello!", 8);
  strncpy(area2, "World!", 8);
  snprintf(area3, 8, "UID: %d", getpid());
  strncpy(area4, "Integer", 8);
  strncpy(area5, "Double", 8);

  hfree(area2);
  hfree(area4);

  area2 = halloc(7 * sizeof(char));
  area4 = halloc(7 * sizeof(char));

  printf("Trying to allocate area of length %zu = HEAP_SIZE\n", HEAP_SIZE);
  void *overflow = halloc(HEAP_SIZE);
  if (overflow == NULL) {
    perror("Allocation failed");
  }

  char *zero = halloc(0 * sizeof(char));

  // WARN: UB -> SIGSEGV
  // strcpy(zero, "AHTUNG! Undefined behaviour!");

  char *zero2 = halloc(0 * sizeof(char));

  hfree(zero);
  hfree(zero2);
  hfree(area1);
  hfree(area2);
  hfree(area3);
  hfree(area4);
  hfree(area5);

  return 0;
}
