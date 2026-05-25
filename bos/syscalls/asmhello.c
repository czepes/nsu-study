#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int asmwrite(const char *msg) {
  int r;
  asm volatile("syscall"
               : "=a"(r)             // %rax = bytes_written
               : "a"(SYS_write),     // %rax = write
                 "D"(STDOUT_FILENO), // %rdi = stdout
                 "S"(msg),           // %rsi = message
                 "d"(strlen(msg))    // %rdx = length
  );
  return r;
}

int main(void) {
  asmwrite("Hello, World!");
  return 0;
}
