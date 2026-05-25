#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  const char filepath[] = "./secret.txt";

  size_t size = 256;
  char buffer[size];
  int fd;

  printf("RUID: %d\n"
         "EUID: %d\n"
         "RGID: %d\n"
         "EGID: %d\n",
         getuid(), geteuid(), getgid(), getegid());

  fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  ssize_t bytes;
  while ((bytes = read(fd, buffer, size)) > 0) {
    write(STDOUT_FILENO, buffer, bytes);
  }

  if (bytes < 0) {
    perror("write");
    return 1;
  }

  return 0;
}
