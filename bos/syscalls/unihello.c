#include <unistd.h>

int main(void) {
  char hello[] = "Hello, World!";
  write(STDOUT_FILENO, hello, sizeof(hello) - 1);
  return 0;
}
