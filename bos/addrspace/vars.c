#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KB 1024
#define PAGE 4 * KB

struct symbol {
  uint64_t addr;
  char *name;
};

int global_var = 123;
char global_bss[PAGE];
const int global_const = 321;

void get_addresses_areas(struct symbol symbols[], size_t amount) {
  int pid = getpid();
  char path[32];

  sprintf(path, "/proc/%d/maps", pid);
  FILE *maps_file = fopen(path, "r");

  if (maps_file == NULL) {
    perror("fopen");
    return;
  }

  bool found[amount];
  for (int i = 0; i < amount; i++) {
    found[i] = false;
  }

  size_t length = 256;
  int read = 0;
  char *line = NULL;

  while ((read = getline(&line, &length, maps_file)) != -1) {
    uint64_t start_vaddr, end_vaddr;
    sscanf(line, "%lx-%lx", &start_vaddr, &end_vaddr);

    printf("%s", line);
    for (int i = 0; i < amount; i++) {
      if (found[i]) {
        continue;
      }
      struct symbol symbol = symbols[i];
      if (start_vaddr <= symbol.addr && symbol.addr <= end_vaddr) {
        printf("[%d] %s: %lx\n", i, symbol.name, symbol.addr);
        found[i] = true;
      }
    }
  }
  free(line);

  printf("Not found:\n");
  for (int i = 0; i < amount; i++) {
    if (!found[i]) {
      struct symbol symbol = symbols[i];
      printf("[%i] %s: %lx\n", i, symbol.name, symbol.addr);
    }
  }
}

int *get_local_ptr() {
  int local_var = 78;
  return &local_var;
}

uint64_t get_local_addr() {
  int local_var = 78;
  uint64_t addr = (uint64_t)&local_var;
  return addr;
}

int main(void) {
  int local_var = 1;
  int static static_var = 2;
  const int const_var = 42;

  int *function_var_ptr = get_local_ptr();
  uint64_t function_var_addr = get_local_addr();

  printf("Variable addresses:\n");
  printf("local_var: %p\n", &local_var);
  printf("static_var: %p\n", &static_var);
  printf("const_var: %p\n", &const_var);
  printf("global_var: %p\n", &global_var);
  printf("global_bss: %p %p %p\n", global_bss, &global_bss, global_bss + 4096);
  printf("global_const: %p\n", &global_const);
  printf("function_var: %p %lx\n", function_var_ptr, function_var_addr);
  printf("functions: %p %p\n", get_local_addr, get_local_ptr);

  struct symbol symbols[] = {
      {(uint64_t)&local_var, "local_var"},
      {(uint64_t)&static_var, "static_var"},
      {(uint64_t)&const_var, "const_var"},
      {(uint64_t)global_bss, "global_bss"},
      {(uint64_t)&global_bss, "global_bss_start"},
      {(uint64_t)(global_bss + PAGE), "global_bss_end"},
      {(uint64_t)&global_const, "global_const"},
      {(uint64_t)function_var_ptr, "function_var_ptr"},
      {(uint64_t)function_var_addr, "function_var_addr"},
      {(uint64_t)get_local_ptr, "get_local_ptr"},
      {(uint64_t)get_local_addr, "get_local_addr"},
  };

  printf("\nVirtual Memory Areas:\n");
  get_addresses_areas(symbols, sizeof(symbols) / sizeof(struct symbol));

  return 0;
}
