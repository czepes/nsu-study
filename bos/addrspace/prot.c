#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#define KB 1024
#define PAGE_SIZE 4 * KB
#define MAX_PAGES 10
#define BUFLEN 256

int median(int value, int lower, int higher) {
  return value < lower ? lower : value > higher ? higher : value;
}

void swap(int *a, int *b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int parse_prot(char *arg, int fallback) {
  int prot = arg ? atoi(arg) : fallback;
  return median(prot, 0, 7);
}

int parse_pages(char *arg, int fallback) {
  int pages = arg ? atoi(arg) : fallback;
  return median(pages, 1, MAX_PAGES);
}

int parse_page(char *arg, int pages, int fallback) {
  int page = arg ? atoi(arg) : fallback;
  return median(page, 0, pages - 1);
}

int main(int argc, char **argv) {
  const int default_prot = 6;
  const int default_mode = MAP_PRIVATE | MAP_ANONYMOUS;

  int pages = 0;
  int prot = 0;
  int mode = 0;

  printf("$ watch -n1 -d cat /proc/%d/maps\n", getpid());

  char *area = NULL;
  char *line = NULL;
  size_t linelen = BUFLEN;
  char command[BUFLEN] = "";

  while (1) {
    char arg1[BUFLEN] = "";
    char arg2[BUFLEN] = "";

    printf("$ ");

    if (getline(&line, &linelen, stdin) < 0) {
      break;
    }

    line[strcspn(line, "\n")] = '\0';

    int parsed = sscanf(line, "%s %s %s", command, arg1, arg2);

    if (parsed <= 0) {
      printf("Failed to parse '%s'\n", line);
      continue;
    }

    if (strcmp(command, "quit") == 0) {
      break;
    }

    if (strcmp(command, "read") == 0) {
      int page = parse_page(parsed >= 2 ? arg1 : NULL, pages, 0);

      printf("Reading from the page %d...\n", page);
      char str[BUFLEN] = "";
      sscanf(area + page * PAGE_SIZE, "%s", str);
      printf("Read: '%s'\n", str);
      continue;
    }

    if (strcmp(command, "write") == 0) {
      char *arg = arg1;
      int page = parse_page(parsed >= 2 ? arg2 : NULL, pages, 0);

      printf("Writing to the page %d...\n", page);
      strncpy(area + page * PAGE_SIZE, arg, strlen(arg));
      printf("Written: '%s'\n", arg);
      continue;
    }

    if (strcmp(command, "map") == 0) {
      if (area) {
        printf("Area is already mapped!\n");
        continue;
      }

      pages = parse_pages(parsed >= 2 ? arg1 : NULL, 1);
      prot = parse_prot(parsed >= 3 ? arg2 : NULL, default_prot);
      mode = default_mode;

      area = mmap(NULL, pages * PAGE_SIZE, prot, mode, -1, 0);
      printf("Mapped '%c%c%c%c' area of %d pages: %p - %p\n",
             mode & MAP_PRIVATE ? 'p' : 's', prot & PROT_READ ? 'r' : '-',
             prot & PROT_WRITE ? 'w' : '-', prot & PROT_EXEC ? 'x' : '-', pages,
             area, area + pages * PAGE_SIZE);
      continue;
    }

    if (strcmp(command, "unmap") == 0) {
      if (!area) {
        printf("Area is already unmapped!\n");
        continue;
      }

      int start = parse_page(parsed >= 2 ? arg1 : NULL, pages, 0);
      int end = parse_page(parsed >= 3 ? arg2 : NULL, pages, start);

      if (start > end) {
        swap(&start, &end);
      }

      int unpages = end - start + 1;

      size_t offset = start * PAGE_SIZE;
      size_t unlen = unpages * PAGE_SIZE;

      printf("Unmapping %d pages: %d - %d...\n", unpages, start, end);
      munmap(area + offset, unlen);
      printf("Unmapped area: %p - %p\n", area + offset, area + offset + unlen);

      if (unpages >= pages) {
        area = NULL;
      }
      continue;
    }

    printf(
        "Unknown command: '%s'\nExpected: map | read | write | unmap | quit\n",
        command);
  }

  if (line) {
    free(line);
  }
  return 0;
}
