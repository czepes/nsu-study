#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Get ready!\n > watch -n1 -d cat /proc/%d/maps\n", getpid());
    sleep(5);
    execl(argv[0], argv[0], "0", NULL);
  } else {
    int i = atoi(argv[1]);
    printf("[%d] pid = %d\n", i, getpid());
    char arg[64];
    sprintf(arg, "%d", i + 1);
    sleep(1);
    execl(argv[0], argv[0], arg, NULL);
  }
  perror("execl");
  return 1;
}
