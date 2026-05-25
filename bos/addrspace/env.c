#include <stdio.h>
#include <stdlib.h>

void print_env(char *env) {
  char *cur_val = getenv(env);
  if (cur_val == NULL) {
    fprintf(stderr, "Environment variable %s is not found\n", env);
    return;
  }
  printf("%s = %s\n", env, cur_val);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Change environment variable ENV = value\nUsage: %s <ENV> <value>",
           argv[0]);
    return 1;
  }

  char *env = argv[1];
  char *value = argv[2];

  print_env(env);
  setenv(env, value, 1);
  print_env(env);

  return 0;
}
