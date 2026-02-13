#include "dlfcn.h"
#include "stdio.h"

int main(void) {
  void *handle = dlopen("./runtime/libruntime.so", RTLD_LAZY);

  if (handle == NULL) {
    fprintf(stderr, "Failed to open library runtime-hello.so:\n%s\n",
            dlerror());
    return 1;
  }

  char *error_msg;
  void (*print_hello_runtime)(void) = dlsym(handle, "print_hello_runtime");

  if ((error_msg = dlerror()) != NULL) {
    fprintf(stderr, "Failed to retrieve print_hello_runtime function:\n%s\n",
            error_msg);
  }

  print_hello_runtime();

  if (dlclose(handle) != 0) {
    fprintf(stderr, "Failed to close library runtime-hello.so:\n%s\n",
            dlerror());
  }

  return 0;
}
