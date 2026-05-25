#include <fcntl.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool file_exists(const char *path) {
  struct stat st;
  return stat(path, &st) == 0;
}

char *resolve_path(const char *name, char *resolved, int new) {
  size_t src_len = 0;

  if (name == NULL || (src_len = strlen(name)) == 0) {
    return NULL;
  }

  char is_null = resolved == NULL;
  char path[PATH_MAX];

  if (is_null) {
    resolved = malloc(sizeof(char) * PATH_MAX);
    if (resolved == NULL) {
      return NULL;
    }
  }

  if (name[0] != '/') {
    getcwd(path, PATH_MAX);

    const char *copy_ptr = name;
    if (name[0] == '.' && src_len >= 2 && name[1] == '/') {
      copy_ptr += 2;
    }

    sprintf(path, "%s/%s", path, copy_ptr);
  } else {
    strcpy(path, name);
  }

  char *token;
  char *saveptr;
  char *tokens[PATH_MAX];
  int last = 0;

  token = strtok_r(path, "/", &saveptr);

  while (token != NULL) {
    if (strcmp(token, "..") == 0) {
      last = last > 0 ? last - 1 : 0;
    } else if (strcmp(token, ".") != 0 && strcmp(token, "") != 0) {
      tokens[last++] = token;
    }

    token = strtok_r(NULL, "/", &saveptr);
  }

  size_t dst_cur = 0;
  resolved[0] = '/';
  resolved[1] = '\0';

  for (int i = 0; i < last; i++) {
    token = tokens[i];
    size_t token_len = strlen(token);

    resolved[dst_cur++] = '/';
    strncpy(resolved + dst_cur, token, token_len);
    dst_cur += token_len;
    resolved[dst_cur] = '\0';

    if (!file_exists(resolved)) {
      if (new && i + 1 == last) {
        continue;
      }
      if (is_null) {
        free(resolved);
      }
      return NULL;
    }
  }

  return resolved;
}

int main(int argc, char **argv) {
  char *my_path = NULL;
  char *real_path = NULL;
  char my_st_path[PATH_MAX];
  char real_st_path[PATH_MAX];

  resolve_path(argv[1], my_st_path, 0);
  my_path = resolve_path(argv[1], NULL, 0);

  realpath(argv[1], real_st_path);
  real_path = realpath(argv[1], NULL);

  printf("my_path[]: %s\nreal_path[]: %s\n\n*my_path: %s\n*real_path: %s\n",
         my_st_path, real_st_path, my_path ? my_path : "NULL",
         real_path ? real_path : "NULL");

  if (my_path) {
    free(my_path);
  }
  if (real_path) {
    free(real_path);
  }
  return 0;
}
