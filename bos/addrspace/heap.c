#include "heap.h"

void *heap = NULL;         // mmaped virtual memory area
header_t *freelist = NULL; // list of unallocated memory regions

void set_size(header_t *head, size_t size, char is_free) {
  if (head == NULL) {
    return;
  }
  head->size = is_free ? SET_FREE(size) : SET_USED(size);
  FOOTER(head)->size = head->size;
}

header_t *get_left(header_t *current) {
  const char *heap_start = (char *)heap;

  // Someone on the left is in heap
  if ((char *)current < heap_start + sizeof(header_t) + sizeof(footer_t)) {
    return NULL;
  }

  footer_t *prev_footer = (footer_t *)((char *)current - sizeof(footer_t));

  // Someone on the left has valid size
  if ((char *)current - REAL_SIZE(prev_footer->size) < heap_start) {
    return NULL;
  }

  header_t *prev_header = HEADER(prev_footer);

  // Someone on the left is valid area
  if (prev_header->size != prev_footer->size) {
    return NULL;
  }

  return prev_header;
}

header_t *get_right(header_t *current) {
  const char *heap_end = (char *)heap + HEAP_SIZE;

  // Someone on the right is in heap
  const char *next = (char *)current + REAL_SIZE(current->size);
  if (next + sizeof(header_t) + sizeof(footer_t) > heap_end) {
    return NULL;
  }

  header_t *next_header = (header_t *)next;

  // Someone on the right has valid size
  if ((char *)next + REAL_SIZE(next_header->size) > heap_end) {
    return NULL;
  }

  footer_t *next_footer = FOOTER(next_header);

  // Someone on the right is valid area
  if (next_footer->size != next_header->size) {
    return NULL;
  }

  return next_header;
}

void hexit() {
  if (heap != NULL) {
    munmap(heap, HEAP_SIZE);
  }
}

int hinit(char *dumpfile) {
  if (dumpfile == NULL) {
    dumpfile = DEFAULT_DUMPFILE;
  }

  int fd = open(dumpfile, O_CREAT | O_TRUNC | O_RDWR, 0644);
  if (fd == -1) {
    return -1;
  }

  if (ftruncate(fd, HEAP_SIZE) == -1) {
    return -1;
  }

  heap = mmap(NULL, HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  if (heap == MAP_FAILED) {
    return -1;
  }

  memset(heap, 0, HEAP_SIZE);

  freelist = (header_t *)heap;
  set_size(freelist, HEAP_SIZE, 1);

  freelist->next = NULL;
  freelist->prev = NULL;

  atexit(hexit);
  return 0;
}

void *halloc(size_t len) {
  size_t aligned_len = (len + ALIGN) & (~ALIGN);
  size_t needed_size = sizeof(header_t) + aligned_len + sizeof(footer_t);

  // First Fit
  header_t *current = freelist;
  while (current != NULL) {
    if (REAL_SIZE(current->size) >= needed_size) {
      break;
    }
    current = current->next;
  }

  if (current == NULL) {
    errno = ENOMEM;
    return NULL;
  }

  // Splitting
  size_t remaining = REAL_SIZE(current->size) - needed_size;

  if (remaining > sizeof(header_t) + CHUNK_LEN + sizeof(footer_t)) {
    // Split
    header_t *next = (header_t *)((char *)current + needed_size);
    set_size(next, remaining, 1);
    set_size(current, needed_size, 0);

    next->next = current->next;
    next->prev = current;
    if (current->next) {
      current->next->prev = next;
    }
    current->next = next;
  } else {
    // Do not split
    set_size(current, current->size, 0);
  }

  // Removal
  if (current->prev) {
    current->prev->next = current->next;
  }
  if (current->next) {
    current->next->prev = current->prev;
  }
  if (freelist == current) {
    freelist = current->next;
  }

  current->next = NULL;
  current->prev = NULL;

  return (void *)(current + 1);
}

void hfree(void *p) {
  if (p == NULL) {
    return;
  }

  header_t *current = (header_t *)p - 1;

  if (IS_FREE(current->size)) {
    return;
  }

  set_size(current, current->size, 1);

  // Merging
  // left += cur
  header_t *left = get_left(current);
  if (left && IS_FREE(left->size)) {
    // No need to remove current from freelist

    size_t new_size = REAL_SIZE(left->size) + REAL_SIZE(current->size);
    set_size(left, new_size, 1);

    current = left;
  }

  // cur += right
  header_t *right = get_right(current);
  if (right && IS_FREE(right->size)) {
    // Remove right from freelist
    if (right->prev) {
      right->prev->next = right->next;
    }
    if (right->next) {
      right->next->prev = right->prev;
    }
    if (right == freelist) {
      freelist = right->next;
    }

    size_t new_size = REAL_SIZE(current->size) + REAL_SIZE(right->size);
    set_size(current, new_size, 1);
  }

  // Insert at the start of freelist
  if (freelist != current) {
    current->prev = NULL;
    current->next = freelist;
    if (freelist) {
      freelist->prev = current;
    }
    freelist = current;
  }
}

size_t hlen(header_t *head) {
  return head ? REAL_SIZE(head->size) - sizeof(header_t) - sizeof(footer_t) : 0;
}

size_t hfreelen() {
  size_t len = 0;
  header_t *current = freelist;
  while (current != NULL) {
    len += hlen(current);
    current = current->next;
  }
  return len;
}
