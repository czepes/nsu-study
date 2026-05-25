#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_SHOW INT_MAX
#define PAGE_SIZE sysconf(_SC_PAGE_SIZE)

struct vma {
  uint64_t start_page;
  uint64_t end_page;
  uint64_t pages_num;

  uint64_t start_vaddr;
  uint64_t end_vaddr;
  uint64_t offset;
  char perms[5];
  char dev[6];
  int inode;
  char pathname[PATH_MAX];
};

struct ptes {
  int amount;
  uint64_t start_idx;
  uint64_t *entries;
};

// INFO: from https://man7.org/linux/man-pages/man5/proc_pid_maps.5.html
// /proc/pid/maps
// A file containing the currently mapped memory regions and
// their access permissions.
// The format of the file is:
//     address           perms offset  dev   inode       pathname
//     00400000-00452000 r-xp 00000000 08:02 173521      /usr/bin/dbus-daemon
//     00651000-00652000 r--p 00051000 08:02 173521      /usr/bin/dbus-daemon
//     00652000-00655000 rw-p 00052000 08:02 173521      /usr/bin/dbus-daemon
//     00e03000-00e24000 rw-p 00000000 00:00 0           [heap]
//     00e24000-011f7000 rw-p 00000000 00:00 0           [heap]
//     ...
int parse_maps_line(char *line, struct vma *vma_p) {
  struct vma vma;
  ssize_t read = sscanf(line, "%lx-%lx %5s %lx %5s %d %s", &vma.start_vaddr,
                        &vma.end_vaddr, vma.perms, &vma.offset, vma.dev,
                        &vma.inode, vma.pathname);

  if (read < 6) {
    return -1;
  }

  vma.start_page = vma.start_vaddr / PAGE_SIZE;
  vma.end_page = (vma.end_vaddr - 1) / PAGE_SIZE;
  vma.pages_num = vma.end_page - vma.start_page + 1;

  *vma_p = vma;
  return 0;
}

void print_vma(int num, struct vma *vma_p) {
  printf("[VMA  No] VIRTUAL MEMORY AREA INFO:\n"
         "[VMA %03d] %-37s %-5s %-10s %-5s %-8s %-s\n"
         "[VMA %03d] 0x%016lx-0x%016lx %-5s %-10lu %-5s %-8d %-s\n",
         num, "Virtual address range", "Perms", "Offset", "Dev", "Inode",
         "Pathname", num, vma_p->start_vaddr, vma_p->end_vaddr, vma_p->perms,
         vma_p->offset, vma_p->dev, vma_p->inode,
         vma_p->pathname[0] ? vma_p->pathname : "[anonymous]");

  if (vma_p->pages_num == 1) {
    printf("[VMA %03d] PAGES = %lu [%lu]\n", num, vma_p->pages_num,
           vma_p->start_page);
  } else {
    printf("[VMA %03d] PAGES = %lu [%lu - %lu]\n", num, vma_p->pages_num,
           vma_p->start_page, vma_p->end_page);
  }
}

//  INFO: from https://www.kernel.org/doc/Documentation/vm/pagemap.txt
// * /proc/pid/pagemap.  This file lets a userspace process find out which
//   physical frame each virtual page is mapped to. It contains one 64-bit
//   value for each virtual page, containing the following data (from
//   fs/proc/task_mmu.c, above pagemap_read):
// * Bits 0-54  page frame number (PFN) if present
// * Bits 0-4   swap type if swapped
// * Bits 5-54  swap offset if swapped
// * Bit  55    pte is soft-dirty (see Documentation/vm/soft-dirty.txt)
// * Bit  56    page exclusively mapped (since 4.2)
// * Bits 57-60 zero
// * Bit  61    page is file-page or shared-anon (since 3.5)
// * Bit  62    page swapped
// * Bit  63    page present
const uint64_t PAGE_PRESENT = 1ULL << 63;
const uint64_t PAGE_SWAPPED = 1ULL << 62;
const uint64_t PAGE_FILE_SHARED = 1ULL << 61;
const uint64_t PAGE_EXCLUSIVE = 1ULL << 56;
const uint64_t PAGE_SOFT_DIRTY = 1ULL << 55;
const uint64_t PFN_MASK = (1ULL << 55) - 1;

void print_ptes(struct ptes *ptes, struct vma *vma) {
  printf("[PTE  No] PAGE INFO:\n"
         "[PTE  No] %-20s %-16s %-10s %-10s %s\n",
         "Virtual Page Number", "Physical", "Present", "Swapped",
         "Other Flags");

  for (int i = 0; i < ptes->amount; i++) {
    uint64_t pte = ptes->entries[i];
    uint64_t page_idx = ptes->start_idx + i;
    uint64_t pfn = pte & PFN_MASK;

    char *file_shared = pte & PAGE_FILE_SHARED
                            ? (vma->pathname[0] ? "FILE-PAGE" : "SHARED-ANON")
                            : "x";
    char *soft_dirty = pte & PAGE_SOFT_DIRTY ? "SOFT-DIRTY" : "x";
    char *exclusive = pte & PAGE_EXCLUSIVE ? "EXCLUSIVE" : "x";
    char *present = pte & PAGE_PRESENT ? "YES" : "NO";
    char *swapped = pte & PAGE_SWAPPED ? "YES" : "NO";

    printf("[PTE %03d] %-20lu 0x%-14lx %-10s %-10s %s %s %s\n", i, page_idx,
           pfn, present, swapped, file_shared, soft_dirty, exclusive);
  }
}

int main(int argc, char **argv) {
  int show = DEFAULT_SHOW;
  int pid;

  if (argc <= 1) {
    printf("Print process pagemap\nUsage: %s <pid> [print=all]\n", argv[0]);
    return 1;
  }

  if (argc >= 2) {
    char *pid_arg = argv[1];
    if (strcmp(pid_arg, "self") == 0) {
      pid = getpid();
    } else {
      pid = atoi(pid_arg);
    }
  }

  if (pid <= 0) {
    fprintf(stderr, "Invalid pid: %d\n", pid);
    return 1;
  }

  if (argc >= 3) {
    char *show_arg = argv[2];
    if (strcmp(show_arg, "all") == 0) {
      show = DEFAULT_SHOW;
    } else {
      show = atoi(argv[2]);
    }
  }

  if (show <= 0) {
    show = DEFAULT_SHOW;
  }

  if (geteuid() != 0) {
    fprintf(stderr,
            "[WARNING] Running without EUID = 0\n"
            "[WARNING] PFN values will be masked to 0\n"
            "[WARNING] Run with: sudo %s <pid> [print=all]\n",
            argv[0]);
  }

  char maps_path[PATH_MAX];
  char pagemap_path[PATH_MAX];
  sprintf(maps_path, "/proc/%d/maps", pid);
  sprintf(pagemap_path, "/proc/%d/pagemap", pid);

  FILE *maps = fopen(maps_path, "rb");

  if (maps == NULL) {
    fprintf(stderr, "Failed to open %s: ", maps_path);
    perror("fopen");
    return 1;
  }

  FILE *pagemap = fopen(pagemap_path, "rb");

  if (pagemap == NULL) {
    fprintf(stderr, "Failed to open %s: ", pagemap_path);
    perror("fopen");
    fclose(maps);
    return 1;
  }

  printf("[PAGEMAP] Analyzing process PID = %d\n", pid);

  size_t length = 256;
  char *line = NULL;
  int vma_count = 0;
  size_t read;

  while ((read = getline(&line, &length, maps)) != -1) {
    struct vma vma;

    if (parse_maps_line(line, &vma) == -1) {
      continue;
    };

    print_vma(++vma_count, &vma);

    uint64_t to_print = vma.pages_num < show ? vma.pages_num : show;
    uint64_t page_entries[to_print];
    uint64_t page_offset = vma.start_page * sizeof(uint64_t);

    if (fseek(pagemap, page_offset, SEEK_SET) == -1) {
      fprintf(stderr, "Failed to move to current page entries\n");
      perror("fseek");
      continue;
    }

    ssize_t pages_read;
    if ((pages_read =
             fread(page_entries, sizeof(uint64_t), to_print, pagemap)) == -1) {
      fprintf(stderr, "Failed to read page table entries\n");
      perror("fread");
      continue;
    }

    struct ptes ptes = {
        pages_read,
        vma.start_page,
        page_entries,
    };

    print_ptes(&ptes, &vma);

    uint64_t skipped = vma.pages_num - to_print;
    if (skipped > 0) {
      printf("[SKIPPED] ... (%lu more pages not shown)\n", skipped);
    }
  }

  free(line);
  fclose(pagemap);
  fclose(maps);
  return 0;
}
