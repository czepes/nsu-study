#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct vma {
  uint64_t start_vaddr;
  uint64_t end_vaddr;
  uint64_t offset;
  char perms[5];
  char dev[6];
  int inode;
  char pathname[PATH_MAX];
};

struct ptes {
  uint64_t start_idx;
  int amount;
  uint64_t *entries;
};

const int DEFAULT_SHOW = 10;

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
int parse_maps_line(char *line, struct vma *vma_ptr) {
  struct vma vma;
  int read = sscanf(line, "%lx-%lx %5s %lx %5s %d %s", &vma.start_vaddr,
                    &vma.end_vaddr, vma.perms, &vma.offset, vma.dev, &vma.inode,
                    vma.pathname);
  if (read < 6) {
    return -1;
  }
  *vma_ptr = vma;
  return 0;
}
void print_vma(struct vma *vma_ptr, FILE *out) {
  fprintf(out, "Virtual Address range: 0x%lx-0x%lx\n", vma_ptr->start_vaddr,
          vma_ptr->end_vaddr);
  fprintf(out, "%-5s %-10s %-5s %-8s %-s\n", "Perms", "Offset", "Dev", "Inode",
          "Pathname");
  fprintf(out, "%-5s 0x%08lx %-5s %-8d %-s\n", vma_ptr->perms, vma_ptr->offset,
          vma_ptr->dev, vma_ptr->inode,
          vma_ptr->pathname[0] ? vma_ptr->pathname : "[anonymous]");
}

//  INFO: from https://www.kernel.org/doc/Documentation/vm/pagemap.txt
// * /proc/pid/pagemap.  This file lets a userspace process find out which
//   physical frame each virtual page is mapped to.  It contains one 64-bit
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

void print_ptes(struct ptes *ptes, struct vma *vma, FILE *out) {
  fprintf(out, "  %-20s %-8s %-10s %-10s %s\n", "Virtual Page Number",
          "Physical", "Present", "Swapped", "Other Flags");

  for (int i = 0; i < ptes->amount; i++) {
    uint64_t pte = ptes->entries[i];
    uint64_t page_idx = ptes->start_idx + i;
    uint64_t pfn = pte & PFN_MASK;
    char flags[64] = "";

    if (pte & PAGE_FILE_SHARED) {
      strcat(flags, vma->pathname[0] ? "FILE-PAGE " : "SHARED-ANON ");
    }
    if (pte & PAGE_SOFT_DIRTY) {
      strcat(flags, "SOFT-DIRTY ");
    }
    if (pte & PAGE_EXCLUSIVE) {
      strcat(flags, "EXCLUSIVE");
    }

    if (pte & PAGE_PRESENT) {
      fprintf(out, "  %-20lu 0x%-6lx %-10s %-10s %s\n", page_idx, pfn, "YES",
              "", flags);
    } else if (pte & PAGE_SWAPPED) {
      fprintf(out, "  %-20lu %-8s %-10s %-10s %s\n", page_idx, "-", "NO", "YES",
              flags);
    } else {
      fprintf(out, "  %-20lu %-8s %-10s %-10s %s\n", page_idx, "-", "NO", "NO",
              flags);
    }
  }
}

int main(int argc, char **argv) {
  int show = DEFAULT_SHOW;
  int pid;

  if (argc <= 1) {
    printf("Print process pagemap\nUsage: %s <pid> [print=10]\n", argv[0]);
    return 1;
  }

  if (argc >= 2) {
    char *pidname = argv[1];
    if (strcmp(pidname, "self") == 0) {
      pid = getpid();
    } else {
      pid = atoi(pidname);
    }
  }

  if (pid <= 0) {
    fprintf(stderr, "Invalid pid: %d\n", pid);
    return 1;
  }

  if (argc >= 3) {
    show = atoi(argv[2]);
  }

  if (show <= 0) {
    show = DEFAULT_SHOW;
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

  long page_size = sysconf(_SC_PAGE_SIZE);
  printf("Analyzing process: %d\n", pid);
  printf("System Page Size: %ld bytes\n\n", page_size);

  size_t length = 256;
  char *line = NULL;
  int vma_count = 0;
  size_t read;

  while ((read = getline(&line, &length, maps)) != -1) {
    struct vma vma;

    if (parse_maps_line(line, &vma) == -1) {
      continue;
    };

    printf("VMA %d:\n", ++vma_count);
    print_vma(&vma, stdout);

    uint64_t start_page = vma.start_vaddr / page_size;
    uint64_t end_page = (vma.end_vaddr - 1) / page_size;
    uint64_t pages_num = end_page - start_page + 1;

    if (strstr(vma.pathname, "[vsyscall]") != NULL) {
      printf("(special region, skipping)\n");
      continue;
    }

    printf("Pages: %lu (from %lu to %lu)\n", pages_num, start_page, end_page);

    int show_now = pages_num < show ? pages_num : show;
    uint64_t page_entries[show_now];

    long page_offset = start_page * sizeof(uint64_t);

    if (fseek(pagemap, page_offset, SEEK_SET) == -1) {
      fprintf(stderr, "Failed to move to current page entries\n");
      perror("fseek");
      continue;
    }

    int pages_read;

    if ((pages_read =
             fread(page_entries, sizeof(uint64_t), show_now, pagemap)) == -1) {
      fprintf(stderr, "Failed to read page table entries\n");
      perror("fread");
      continue;
    }

    struct ptes ptes = {
        start_page,
        pages_read,
        page_entries,
    };

    printf("Page Info:\n");
    print_ptes(&ptes, &vma, stdout);
    int not_shown = pages_num - show;
    if (not_shown > 0) {
      printf("  ... (%d more pages not shown)\n", not_shown);
    }
  }

  free(line);
  fclose(pagemap);
  fclose(maps);
  return 0;
}
