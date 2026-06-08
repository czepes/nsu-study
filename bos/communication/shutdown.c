#include "sock.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Shutdown socket server\nUsage: %s {soft|hard}\n", argv[0]);
    return 0;
  }

  int mode = SIGINT;
  if (argc == 2) {
    if (strcmp(argv[1], "soft") == 0) {
      mode = SIGUSR1;
    }
    if (strcmp(argv[1], "hard") == 0) {
      mode = SIGINT;
    }
  }

  int termfd = open(TERM_ADDR, O_RDONLY);
  if (termfd == -1) {
    perror("open");
    return 1;
  }

  pid_t server_pid;
  if (read(termfd, &server_pid, sizeof(pid_t)) < sizeof(pid_t)) {
    perror("write");
    return 1;
  }
  close(termfd);

  kill(server_pid, mode);

  return 0;
}
