#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BOUND UINT_MAX

int writer_process() {
  int pos = 0;

  for (unsigned long num = 0; num < BOUND; num++) {
    printf("%u ", (unsigned int)num);
    fflush(stdout);
    fprintf(stderr, "[writer] Written [%d] = %u\n", pos, (unsigned int)num);

    pos++;
  }

  return 0;
}

int reader_process() {
  int pos = 0;
  unsigned int prev = 0;
  unsigned int cur = 0;

  for (unsigned long count = 0; count < BOUND; count++) {
    scanf("%u ", &cur);
    printf("[reader] Read [%d] = %u\n", pos, cur);

    if (pos > 0 && cur <= prev) {
      fprintf(stderr, "[reader] Sequence is invalid: [%d] = %u >= [%d] = %u\n",
              pos - 1, prev, pos, cur);
      return 1;
    }

    pos++;
    prev = cur;
  }

  return 0;
}

pid_t rw_init() {
  int pipefd[2];
  if (pipe(pipefd) != 0) {
    fprintf(stderr, "Failed to create pipe\n");
    return -1;
  }

  pid_t child_pid = fork();

  switch (child_pid) {
  case -1: {
    fprintf(stderr, "Failed to create child process\n");
    return -1;
  }
  case 0: {
    close(pipefd[1]);

    if (dup2(pipefd[0], STDIN_FILENO) == -1) {
      close(pipefd[0]);
      perror("[child] dup2");
      kill(getppid(), SIGTERM);
      return -1;
    }

    close(pipefd[0]);
    return 0;
  }
  default: {
    close(pipefd[0]);

    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
      close(pipefd[1]);
      perror("[parent] dup2");
      kill(child_pid, SIGTERM);
      return -1;
    }

    close(pipefd[1]);
    return child_pid;
  }
  }
}

int main(void) {
  pid_t child_pid = rw_init();
  switch (child_pid) {
  case -1: {
    exit(1);
  }
  case 0: {
    printf("[reader] Starting\n");
    int r = reader_process();
    exit(r);
  }
  default: {
    fprintf(stderr, "[writer] Starting\n");
    int r = writer_process();
    exit(r);
  }
  }
}
