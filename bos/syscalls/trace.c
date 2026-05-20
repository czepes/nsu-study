#include "sysinfo.h"
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  if (argc <= 1) {
    printf("Trace programs syscalls\nUsage: %s /path/to/program\n", argv[0]);
    return 1;
  }

  pid_t pid = fork();
  switch (pid) {
  case -1:
    perror("fork");
    return 1;
  case 0: {
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
      perror("TRACEME");
      return 1;
    }

    execv(argv[1], argv + 1);
    perror("execv");
    return 1;
  }
  default: {
    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      fprintf(stderr, "Process exited before tracing\n");
      return 1;
    }
    if (WIFSIGNALED(status)) {
      fprintf(stderr, "Process terminated before tracing\n");
      return 1;
    }

    printf("[tracer][##] Trace on\n");

    int calls = 0;
    struct user_regs_struct regs;

    while (1) {
      if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) {
        perror("SYSCALL");
        break;
      }

      waitpid(pid, &status, 0);

      if (WIFEXITED(status)) {
        printf("\n[tracee][##] Exited with code %d\n", WEXITSTATUS(status));
        break;
      }

      if (WIFSIGNALED(status)) {
        printf("\n[tracee][##] Terminated with signal %d\n", WTERMSIG(status));
        break;
      }

      if (WIFSTOPPED(status)) {
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
          perror("GETREGS");
          continue;
        }

        unsigned long long ret = regs.rax;
        int sys_num = regs.orig_rax;

        switch (calls % 2) {
        case 0:
          printf("[tracee][%02d] Syscall %s(%d)", calls / 2,
                 get_syscall_name(sys_num), sys_num);
          break;
        case 1:
          if (is_err_syscall(sys_num, ret)) {
            printf(" = %lld (%s)\n", ret, strerror(-ret));
          } else if (is_ptr_syscall(sys_num)) {
            printf(" = %p\n", (void *)ret);
          } else {
            printf(" = %lld\n", ret);
          }
          break;
        }

        calls++;
      }
    }

    printf("[tracer][##] Trace off\n");
    return 0;
  }
  }
}
