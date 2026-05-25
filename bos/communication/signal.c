#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define KB 1024
#define PAGE_SIZE 4 * KB

#define BOUND UINT_MAX
#define TIMEOUT 1

#define SIGSYN SIGUSR1
#define SIGSTART SIGUSR2

pid_t *wpid = NULL;
pid_t *rpid = NULL;

sigset_t rw_sigset() {
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGSYN);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  return set;
}

int synwait(char *self, pid_t other_pid) {
  siginfo_t info;
  sigset_t set = rw_sigset();
  const struct timespec timeout = {
      .tv_sec = 1,
  };

  while (1) {
    if (sigtimedwait(&set, &info, &timeout) == -1) {
      if (errno == EAGAIN) {
        fprintf(stderr, "%s Wait timeout\n", self);
        fflush(stderr);
      }
      if (errno == EINTR) {
        fprintf(stderr, "%s Interrupted by unblocked signal\n", self);
        fflush(stderr);
      }
      kill(other_pid, SIGINT);
      return 1;
    }

    printf("%s Caught signal %d from PID %d\n", self, info.si_signo,
           info.si_pid);
    fflush(stdout);

    if (info.si_signo == SIGINT || info.si_signo == SIGTERM) {
      if (info.si_pid != other_pid) {
        kill(other_pid, SIGINT);
      }
      return 1;
    }

    if (info.si_signo == SIGSYN) {
      if (info.si_pid == other_pid) {
        return 0;
      }
    }
  };

  printf("%s Wait timeout\n", self);
  fflush(stdout);
  return 1;
}

int writer_process(unsigned int *buffer, int length) {
  if (wpid == NULL || rpid == NULL) {
    return 1;
  }

  pid_t other_pid = *rpid;

  char *self = "[writer]";
  int pos = 0;

  for (unsigned long num = 0; num < BOUND; num++) {
    buffer[pos] = num;
    kill(other_pid, SIGSYN);
    printf("%s Written [%d] = %u\n", self, pos, (unsigned int)num);
    fflush(stdout);

    printf("%s Waiting for reader\n", self);
    fflush(stdout);
    int r = synwait(self, other_pid);
    if (r != 0) {
      return r;
    }

    pos = (pos + 1) % length;
  }

  return 0;
}

int reader_process(unsigned int *buffer, int length) {
  if (wpid == NULL || rpid == NULL) {
    return 1;
  }

  pid_t other_pid = *wpid;

  char *self = "[reader]";
  int pos = 0;
  unsigned int prev = 0;
  unsigned int cur = 0;

  for (unsigned long count = 0; count < BOUND; count++) {
    printf("%s Waiting for writer\n", self);
    fflush(stdout);
    int r = synwait(self, other_pid);
    if (r != 0) {
      return r;
    }

    cur = buffer[pos];
    kill(other_pid, SIGSYN);

    printf("%s Read [%d] = %u\n", self, pos, cur);
    fflush(stdout);

    if (pos > 0 && cur <= prev) {
      fprintf(stderr, "%s Sequence is invalid: [%d] = %u >= [%d] = %u\n", self,
              pos - 1, prev, pos, cur);
      fflush(stderr);
      return 2;
    }

    prev = cur;
    pos = (pos + 1) % length;
  }

  return 0;
}

int rw_init() {
  rpid = mmap(NULL, 2 * sizeof(pid_t), PROT_READ | PROT_WRITE,
              MAP_SHARED | MAP_ANON, -1, 0);

  if (rpid == MAP_FAILED) {
    return 1;
  }

  wpid = rpid + 1;
  *rpid = 0;
  *wpid = 0;

  sigset_t set = rw_sigset();
  sigemptyset(&set);
  sigaddset(&set, SIGSYN);
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGTERM);
  sigprocmask(SIG_BLOCK, &set, NULL);

  return 0;
}

int rw_share_pids(pid_t reader_pid, pid_t writer_pid) {
  if (rpid == NULL || wpid == NULL) {
    return 1;
  }
  *rpid = reader_pid;
  *wpid = writer_pid;
  mprotect(rpid, 2 * sizeof(pid_t), PROT_READ);
  return 0;
}

int main(void) {
  size_t size = PAGE_SIZE;
  void *mem =
      mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
  if (mem == MAP_FAILED) {
    fprintf(stderr, "Failed to mmap buffer\n");
    return 1;
  }

  printf("Mmaped shared memory area [%p, %p]\n", mem, mem + size - 1);
  memset(mem, 0, size);

  unsigned int *buffer = (unsigned int *)mem;
  int length = size / sizeof(unsigned int);

  if (rw_init() != 0) {
    fprintf(stderr, "Failed to mmap reader/writer pids buffer\n");
    return 1;
  }

  pid_t pid = fork();

  switch (pid) {
  case -1: {
    fprintf(stderr, "Failed to create child process\n");
    exit(1);
  }
  case 0: {
    char *self = "[writer]";

    printf("%s Writing 0...UINT_MAX to buffer\n", self);
    int r = writer_process(buffer, length);

    printf("%s Exiting with code %d\n", self, r);
    exit(r);
  }
  default: {
    char *self = "[reader]";
    rw_share_pids(getpid(), pid);

    printf("%s Reading from buffer\n", self);
    int r = reader_process(buffer, length);

    printf("%s Exiting with code %d\n", self, r);
    exit(r);
  }
  }
}
