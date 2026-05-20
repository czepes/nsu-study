#include <limits.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#include <linux/ptrace.h>

int main(int argc, char **argv) {
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

    execl("./asm", "asmhello", NULL);
    perror("execl");
    return 1;
  }
  default: {
    int status;
    struct user_regs_struct regs;

    waitpid(pid, &status, 0);
    printf("[tracer][##] Trace on\n");

    int calls = 0;
    while (1) {
      if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) {
        perror("SYSCALL");
        break;
      }

      waitpid(pid, &status, 0);

      if (WIFEXITED(status)) {
        printf("[tracee][##] Exited with code %d\n", WEXITSTATUS(status));
        break;
      }

      if (WIFSIGNALED(status)) {
        printf("[tracee][##] Terminated with signal %d\n", WTERMSIG(status));
        break;
      }

      if (WIFSTOPPED(status)) {
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
          perror("GETREGS");
          continue;
        }

        switch (calls % 2) {
        case 0:
          printf("[tracee][%02d][entry] Syscall %llu\n", calls / 2,
                 regs.orig_rax);
          break;
        case 1:
          printf("[tracee][%02d][.exit] Return ", calls / 2);
          if (regs.rax > UINT_MAX) {
            printf("0x%llx\n", regs.rax);
          } else {
            printf("%llu\n", regs.rax);
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
