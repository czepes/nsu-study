#define _GNU_SOURCE

#include <fcntl.h>
#include <linux/sched.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define STACK_SIZE PAGE_SIZE
#define DEFAULT_DUMPFILE "stack.dump"

void *stack = NULL;

void sexit() {
  if (stack != NULL) {
    munmap(stack, STACK_SIZE);
  }
}

void *sinit(char *dumpfile) {
  if (stack != NULL) {
    return stack + STACK_SIZE;
  }

  if (dumpfile == NULL) {
    dumpfile = DEFAULT_DUMPFILE;
  }

  int fd = open(dumpfile, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (fd == -1) {
    return NULL;
  }

  if (ftruncate(fd, STACK_SIZE) == -1) {
    return NULL;
  }

  stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_STACK,
               fd, 0);
  close(fd);
  memset(stack, 0, STACK_SIZE);

  if (stack == MAP_FAILED) {
    return NULL;
  }

  atexit(sexit);
  return stack + STACK_SIZE;
}

void hello(int depth) {
  if (depth == 0) {
    return;
  }
  char msg[] = "Hello World!";
  hello(depth - 1);
}

int child_fn(void *_) {
  const int depth = 10;
  printf("[child] My PID is %d.\n"
         "[child] I was created with clone(2) by %d!\n"
         "[child] I will call hello() recursively %d times!\n"
         "[child] You can inspect my stack with:\n"
         "          xxd %s\n",
         getpid(), getppid(), depth, DEFAULT_DUMPFILE);
  hello(depth);
  return 0;
}

int main(void) {
  void *stack_top = sinit(NULL);
  if (stack_top == NULL) {
    perror("init stack");
    return 1;
  }

  printf("[parent] My PID = %d.\n[parent] I will call clone(2).\n", getpid());

  int pid = clone(child_fn, stack_top, SIGCHLD, NULL);
  if (pid == -1) {
    perror("clone");
    return 1;
  }

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    return 1;
  }
  if (WIFEXITED(status)) {
    printf("[parent] Child has exited with code %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("[parent] Child has terminated with signal %d\n", WTERMSIG(status));
  }

  return 0;
}
