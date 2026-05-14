#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  size_t buf_size = 100;
  char *buffer = malloc(sizeof(char) * buf_size);

  if (buffer == NULL) {
    perror("malloc");
    return 1;
  }

  const char *msg = "Hello, World";
  strcpy(buffer, msg);

  printf("Initial:\n%p '%s'\n", buffer, buffer);
  free(buffer);
  printf("Freed:\n%p '%s'\n", buffer, buffer);

  buffer = malloc(sizeof(char) * buf_size);

  strcpy(buffer, msg);

  printf("Reinitialized:\n%p '%s'\n", buffer, buffer);

  char *middle = buffer + (buf_size / 2);
  free(middle);

  printf("Freed in the middle:\n%p '%s'\n", buffer, buffer);

  free(buffer);
  return 0;
}
