#include <string.h>
#include <syscall.h>
#include <unistd.h>

int syswrite(char *msg) {
  return syscall(SYS_write, STDOUT_FILENO, msg, strlen(msg));
}

int main(void) {
  syswrite("Hello, World!");
  return 0;
}
