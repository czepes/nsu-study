#include <linux/prctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  int *heirs_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                          MAP_ANON | MAP_SHARED, -1, 0);
  *heirs_count = 0;
  char *self = "[grandparent]";
  int child_sleep = 20, parent_sleep = 10;

  prctl(PR_SET_CHILD_SUBREAPER, 1);

  pid_t grandparent_pid = getpid();
  pid_t parent_pid = fork();
  switch (parent_pid) {
  case -1: {
    fprintf(stderr, "%s failed to create child process\n", self);
    exit(EXIT_FAILURE);
  }
  case 0: {
    self = "[parent]";
    ++(*heirs_count);

    parent_pid = getpid();
    pid_t child_pid = fork();

    switch (child_pid) {
    case -1: {
      fprintf(stderr, "%s failed to create child process\n", self);
      exit(EXIT_FAILURE);
    }
    case 0: {
      self = "[child]";
      ++(*heirs_count);

      child_pid = getpid();
      printf("%-13s Copy command:\n"
             "%-13s watch -n1 ps -o pid,ppid,state,cmd -p %d,%d,%d\n",
             self, "", grandparent_pid, parent_pid, child_pid);

      printf("%-13s PID: %d, PPID: %d\n", self, child_pid, getppid());
      printf("%-13s Sleeping for %d seconds...\n", self, child_sleep);
      sleep(child_sleep);
      printf("%-13s PID: %d, PPID: %d\n", self, child_pid, getppid());

      int status = 5;
      printf("%-13s Exiting with code %d\n", self, status);
      exit(status);
    }
    default: {
      printf("%-13s PID: %d, PPID: %d\n", self, getpid(), getppid());
      printf("%-13s Sleeping for %d seconds...\n", self, parent_sleep);
      sleep(parent_sleep);

      int status = 5;
      printf("%-13s Exiting with code %d\n", self, status);
      exit(status);
    }
    }
  }
  default: {
    printf("%-13s Sleeping for 1 second...\n", self);
    sleep(1);

    int status = 0;
    while (*heirs_count > 0) {
      printf("%-13s Press <Enter> to reap ZOMBIE heirs (%d left)\n", self,
             *heirs_count);
      getchar();

      pid_t waited_pid = waitpid(-1, &status, WNOHANG);

      if (waited_pid > 0) {
        if (WIFEXITED(status)) {
          printf("%-13s Heir process %d exited normally with code %d\n", self,
                 waited_pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
          printf("%-13s Heir process %d was terminated by signal %d\n", self,
                 waited_pid, WTERMSIG(status));
        }
        --(*heirs_count);
      } else if (waited_pid == 0) {
        printf("%-13s No ZOMBIE heirs found\n", self);
      } else {
        fprintf(stderr, "%s ", self);
        perror("waitpid");
      }
    }

    status = EXIT_SUCCESS;
    printf("%-13s No heirs left; Exiting with code %d\n", self, status);
    exit(status);
  }
  }
}
