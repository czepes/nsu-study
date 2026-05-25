#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int global_var = 42;

int main(void) {
  char *self = "";
  int local_var = 123;

  pid_t parent_pid = getpid();
  pid_t child_pid = fork();

  switch (child_pid) {
  case -1: {
    fprintf(stderr, "Failed to create child process\n");
    exit(EXIT_FAILURE);
  }
  case 0: {
    self = "[child]";
    child_pid = getpid();

    printf("%s PPID: %d, PID: %d\n", self, parent_pid, child_pid);

    printf("%s local_var = %d {%p}\n[child] global_var = %d {%p}\n", self,
           local_var, &local_var, global_var, &global_var);

    local_var++;
    global_var++;

    printf("%s local_var = %d {%p}\n[child] global_var = %d {%p}\n", self,
           local_var, &local_var, global_var, &global_var);

    printf("%s Sleeping for 10 seconds...\n", self);
    sleep(10);

    int status = 5;
    printf("%s Exiting with code %d\n", self, status);
    exit(5);
  }
  default: {
    self = "[parent]";

    printf("%s Copy commands:\n"
           "          cat /proc/%d/maps\n"
           "          cat /proc/%d/maps\n"
           "          watch -n1 ps -o pid,ppid,state,cmd -p %d,%d\n",
           self, parent_pid, child_pid, parent_pid, child_pid);

    printf("%s local_var = %d {%p}\n[parent] global_var = %d {%p}\n", self,
           local_var, &local_var, global_var, &global_var);

    printf("%s Sleeping for 20 seconds...\n", self);
    sleep(20);

    printf("%s local_var = %d {%p}\n[parent] global_var = %d {%p}\n", self,
           local_var, &local_var, global_var, &global_var);

    printf("%s Waiting for child process to finish...\n", self);
    int status = 0;
    wait(&status);

    if (WIFEXITED(status)) {
      printf("%s Child process exited normally with code %d\n", self,
             WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("%s Child process was terminated by signal %d\n", self,
             WTERMSIG(status));
    }

    status = EXIT_SUCCESS;
    printf("%s Exiting with code %d\n", self, status);
    exit(status);
  }
  }
}
