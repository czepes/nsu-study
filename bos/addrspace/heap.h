#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define HEAP_SIZE (PAGE_SIZE * 10)
#define CHUNK_LEN (sysconf(_SC_LONG_BIT) / 8)
#define ALIGN (CHUNK_LEN - 1)
#define DEFAULT_DUMPFILE "heap.dump"

#define REAL_SIZE(size) ((size) & ~1)
#define IS_FREE(size) ((size) & 1)
#define SET_FREE(size) ((size) | 1)
#define SET_USED(size) ((size) & ~1)

#define FOOTER(h)                                                              \
  ((footer_t *)((char *)(h) + REAL_SIZE((h)->size) - sizeof(footer_t)))
#define HEADER(f)                                                              \
  ((header_t *)((char *)(f) - REAL_SIZE((f)->size) + sizeof(footer_t)))

typedef struct header {
  size_t size;
  struct header *next;
  struct header *prev;
} header_t;

typedef struct footer {
  size_t size;
} footer_t;

int hinit(char *dumpfile);
void *halloc(size_t len);
void hfree(void *p);
size_t hlen(header_t *head);
size_t hfreelen();
