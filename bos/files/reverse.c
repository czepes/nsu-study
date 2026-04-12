#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define KB 1024
#define MB (KB * KB)

char *get_dirent_type(struct dirent *dentry) {
  switch (dentry->d_type) {
  case (DT_FIFO):
    return "Pipe (FIFO)";
  case (DT_CHR):
    return "Character device";
  case (DT_DIR):
    return "Directory";
  case (DT_BLK):
    return "Block device";
  case (DT_REG):
    return "Regular file";
  case (DT_LNK):
    return "Symbolic link";
  case (DT_SOCK):
    return "Local domain socket";
  case (DT_WHT):
    return "Whiteout Inode";
  default:
    return "Unknown type";
  }
}

void reverse_str(const char *src, char *dst, size_t length) {
  if (!src || !dst) {
    return;
  }

  for (int i = 0; i < length / 2; i++) {
    char ch = src[i];
    dst[i] = src[length - 1 - i];
    dst[length - 1 - i] = ch;
  }

  if (length % 2 == 1) {
    dst[length / 2] = src[length / 2];
  }

  dst[length] = '\0';
}

void reverse_last_in_path(const char *src_path, char *dst_path) {
  if (!src_path || !dst_path) {
    return;
  }

  strcpy(dst_path, src_path);
  char *last = strrchr(dst_path, '/');

  if (last) {
    last++;
  } else {
    last = dst_path;
  }

  reverse_str(last, last, strlen(last));
}

int copy_file_content(int src_fd, int dst_fd) {
  const int buf_size = MB;
  char buffer[buf_size];
  ssize_t bytes_read;

  while ((bytes_read = read(src_fd, buffer, buf_size)) != 0) {
    if (write(dst_fd, buffer, buf_size) == -1) {
      perror("Failed to write copied buffer");
      return 1;
    }
  }

  return 0;
}

int reverse_file_content(int src_fd, int dst_fd) {
  const off_t file_size = lseek(src_fd, 0, SEEK_END);
  if (file_size == -1) {
    perror("lseek to end failed");
    return 1;
  }

  if (ftruncate(dst_fd, file_size) == -1) {
    perror("Failed to set new file size");
    return 1;
  }

  const int buf_size = MB;
  char buffer[buf_size];
  off_t remains = file_size;

  while (remains > 0) {
    size_t chunk_size = (remains > buf_size) ? buf_size : remains;
    off_t read_pos = remains - chunk_size;

    if (lseek(src_fd, read_pos, SEEK_SET) == -1) {
      perror("lseek failed");
      return 1;
    }

    ssize_t bytes_read = read(src_fd, buffer, chunk_size);
    if (bytes_read != chunk_size) {
      perror("Failed to read chunk");
      return 1;
    }

    reverse_str(buffer, buffer, chunk_size - 1);

    if (write(dst_fd, buffer, chunk_size) == -1) {
      perror("Failed to write chunk");
      return 1;
    }

    remains -= chunk_size;
  }

  return 0;
}

int copy_file(struct dirent *entry, char *file_path, char *new_path,
              bool reverse) {
  if (!entry || !file_path || !new_path) {
    fprintf(stderr, "Invalied arguments\n");
    return 1;
  }

  int src_fd = open(file_path, O_RDONLY);
  if (src_fd == -1) {
    perror("Failed to open file");
    return -1;
  }

  int dst_fd = open(new_path, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
  if (dst_fd == -1) {
    perror("Failed to create new file");
    close(src_fd);
    return 1;
  }

  int r;
  if (reverse) {
    r = reverse_file_content(src_fd, dst_fd);
  } else {
    r = copy_file_content(src_fd, dst_fd);
  }

  if (r == 0) {
    printf("%s file: '%s' -> '%s'\n", reverse ? "Reversed" : "Copied",
           file_path, new_path);
  }

  close(src_fd);
  close(dst_fd);

  return r;
}

int copy_dir(char *dir_path, char *new_path, bool reverse) {
  if (!dir_path || !new_path) {
    fprintf(stderr, "Invalid arguments\n");
    return 1;
  }

  if (mkdir(new_path, S_IRWXU) == -1) {
    perror("Failed to create new copy of a directory");
    return 1;
  }

  DIR *dir = opendir(dir_path);
  struct dirent *entry;

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char entry_path[PATH_MAX];
    char entry_path_new[PATH_MAX];

    sprintf(entry_path, "%s/%s", dir_path, entry->d_name);
    sprintf(entry_path_new, "%s/%s", new_path, entry->d_name);

    if (reverse) {
      reverse_last_in_path(entry_path_new, entry_path_new);
    }

    switch (entry->d_type) {
    case DT_DIR: {
      int r = copy_dir(entry_path, entry_path_new, reverse);
      if (r != 0) {
        return r;
      }
      break;
    }
    case DT_REG: {
      int r = copy_file(entry, entry_path, entry_path_new, reverse);
      if (r != 0) {
        return r;
      }
      break;
    }
    default: {
      printf("Skipping %s: '%s'\n", get_dirent_type(entry), entry_path);
    }
    }
  }

  printf("%s directory: '%s' -> '%s'\n", reverse ? "Reversed" : "Copied",
         dir_path, new_path);

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Reverse directory & it's contents\nUsage: %s path/to/dir\n",
           argv[0]);
    return 1;
  }

  char *dir_path = argv[1];
  char resolved_path[PATH_MAX];

  realpath(dir_path, resolved_path);
  printf("Resolved path: '%s'\n", resolved_path);

  int length = strlen(resolved_path);
  if (length > 1 && resolved_path[length - 1] == '/') {
    resolved_path[length - 1] = '\0';
    length--;
  }

  char reversed_path[length + 1];
  reverse_last_in_path(resolved_path, reversed_path);

  return copy_dir(resolved_path, reversed_path, true);
}
