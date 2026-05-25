#include "dlfcn.h"
#include "stdio.h"
#include <fcntl.h>
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Usage: %s [arguments]\n", argv[0]);
    return 0;
  }

  void *so_handle = dlopen("libmanager.so", RTLD_LAZY);

  if (so_handle == NULL) {
    fprintf(stderr, "Failed to open libmanager.so:\n%s\n", dlerror());
    return 1;
  }

  char *error_msg;
  char *(*resolve_path)(char *, char *, int) = dlsym(so_handle, "resolve_path");

  if ((error_msg = dlerror()) != NULL) {
    fprintf(stderr, "Failed to retrieve resolve_path():\n%s\n", error_msg);
    return 1;
  }

  char *command = basename(argv[0]);

  if (command == NULL) {
    fprintf(stderr, "Failed to resolve path\n");
    return 1;
  }

  char *path = NULL;
  path = resolve_path(argv[1], path, 1);

  if (path == NULL) {
    fprintf(stderr, "Wrong path: '%s'", argv[1]);
    return 1;
  }

  argv[1] = path;

  char handle_symbol[PATH_MAX];
  sprintf(handle_symbol, "handle_%s", command);

  int (*handle_command)(int, char **) = dlsym(so_handle, handle_symbol);

  if (dlerror() != NULL) {
    fprintf(stderr, "Unknown command: %s\n", command);
    return 1;
  }

  int r = handle_command(argc, argv);

  free(path);
  return r;
}
