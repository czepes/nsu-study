#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define KB 1024
#define MB (KB * KB)

const char DEFAULT_MODE[] = "744";

mode_t resolve_modes(const char modes[3]) {
  mode_t mode = 0;
  switch (modes[0]) {
  case ('7'): {
    mode |= S_IRWXU;
  }
  case ('6'): {
    mode |= S_IRUSR | S_IWUSR;
    break;
  }
  case ('5'): {
    mode |= S_IRUSR | S_IXUSR;
    break;
  }
  case ('4'): {
    mode |= S_IRUSR;
    break;
  }
  case ('3'): {
    mode |= S_IWUSR | S_IXUSR;
    break;
  }
  case ('2'): {
    mode |= S_IWUSR;
    break;
  }
  case ('1'): {
    mode |= S_IXUSR;
    break;
  }
  }
  switch (modes[1]) {
  case ('7'): {
    mode |= S_IRWXG;
  }
  case ('6'): {
    mode |= S_IRGRP | S_IWGRP;
    break;
  }
  case ('5'): {
    mode |= S_IRGRP | S_IXGRP;
    break;
  }
  case ('4'): {
    mode |= S_IRGRP;
    break;
  }
  case ('3'): {
    mode |= S_IWGRP | S_IXGRP;
    break;
  }
  case ('2'): {
    mode |= S_IWGRP;
    break;
  }
  case ('1'): {
    mode |= S_IXGRP;
    break;
  }
  }
  switch (modes[2]) {
  case ('7'): {
    mode |= S_IRWXO;
  }
  case ('6'): {
    mode |= S_IROTH | S_IWOTH;
    break;
  }
  case ('5'): {
    mode |= S_IROTH | S_IXOTH;
    break;
  }
  case ('4'): {
    mode |= S_IROTH;
    break;
  }
  case ('3'): {
    mode |= S_IWOTH | S_IXOTH;
    break;
  }
  case ('2'): {
    mode |= S_IWOTH;
    break;
  }
  case ('1'): {
    mode |= S_IXOTH;
    break;
  }
  }
  return mode;
}

bool file_exists(const char *path) {
  struct stat sb;
  return lstat(path, &sb) == 0;
}

char *resolve_path(const char *name, char *resolved, int new) {
  size_t length = 0;

  if (name == NULL || (length = strlen(name)) == 0) {
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
    if (name[0] == '.' && length >= 2 && name[1] == '/') {
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

int make_file(const char *filename, const char *modes) {
  int fd = creat(filename, resolve_modes(modes));
  if (fd == -1) {
    return 1;
  }

  close(fd);
  return 0;
}

int print_file(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    return 1;
  }

  const size_t buffer_size = MB;
  char buffer[buffer_size];
  ssize_t bytes_read;

  while ((bytes_read = read(fd, buffer, MB)) > 0) {
    write(1, buffer, bytes_read);
  }

  if (bytes_read < 0) {
    return 1;
  }

  close(fd);
  return 0;
}

int remove_file(const char *filename) {
  int r = unlink(filename);
  if (r != 0) {
    return r;
  }
  return 0;
}

int remove_dir(const char *dirname, bool recursive) {
  int r = 0;

  if (!recursive) {
    r = rmdir(dirname);
    return r;
  }

  DIR *dir = opendir(dirname);
  if (!dir) {
    return 1;
  }

  struct dirent *entry = NULL;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char entry_path[PATH_MAX];
    sprintf(entry_path, "%s/%s", dirname, entry->d_name);

    if (entry->d_type == DT_DIR) {
      r = remove_dir(entry_path, recursive);
    } else {
      r = remove_file(entry_path);
    }

    if (r != 0) {
      return r;
    }
  }

  return rmdir(dirname);
}

int print_dir(const char *dirname, int depth, int shift, int step) {
  if (depth == 0) {
    return 0;
  }

  DIR *dir = opendir(dirname);
  if (!dir) {
    return 1;
  }

  struct dirent *entry = NULL;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    printf("%*s%s%s\n", shift, "", entry->d_name,
           entry->d_type == DT_DIR ? "/" : "");

    if (entry->d_type == DT_DIR) {
      char subpath[PATH_MAX];
      sprintf(subpath, "%s/%s", dirname, entry->d_name);
      print_dir(subpath, depth - 1, shift + step, step);
    }
  }

  return 0;
}

char *read_symlink(const char *linkname) {
  struct stat sb;
  if (lstat(linkname, &sb) != 0) {
    fprintf(stderr, "Failed to get link stat\n");
    return NULL;
  }

  char *link = malloc(sizeof(char) * (sb.st_size + 1));
  ssize_t bytes_read = readlink(linkname, link, sb.st_size);

  if (bytes_read != sb.st_size) {
    fprintf(stderr, "Failed to read link\n");
    free(link);
    return NULL;
  }
  link[bytes_read] = '\0';

  return link;
}

char *get_symlink_target(const char *linkname) {
  char *link = read_symlink(linkname);
  char *target;

  if (link[0] == '/') {
    target = realpath(link, NULL);
    free(link);
    return target;
  }

  char *path = malloc(sizeof(char) * (strlen(link) + 1 + PATH_MAX));
  strncpy(path, linkname, PATH_MAX);
  char *end = strrchr(path, '/');
  *(end + 1) = '\0';
  strcat(path, link);

  target = realpath(path, NULL);

  free(path);
  return target;
}

int print_symlink(const char *linkname) {
  char *link = read_symlink(linkname);
  if (link == NULL) {
    return 1;
  }

  printf("Link %s -> %s\n", linkname, link);

  free(link);
  return 0;
}

int print_symlink_file(const char *linkname) {
  char *target = get_symlink_target(linkname);
  if (target == NULL) {
    return 1;
  }

  struct stat sb;
  int r = stat(target, &sb);
  if (r != 0) {
    free(target);
    return r;
  }

  if (S_ISREG(sb.st_mode)) {
    return print_file(target);
  } else if (S_ISDIR(sb.st_mode)) {
    return print_dir(target, -1, 0, 2);
  } else {
    fprintf(stderr, "Non-printable file\n");
  }

  free(target);
  return 1;
}

int print_modes(const char *filename) {
  struct stat sb;
  if (lstat(filename, &sb) == -1) {
    perror("lstat");
    return 1;
  }

  char d = '.', u = 0, g = 0, o = 0;

  (S_ISDIR(sb.st_mode)) && (d = 'd');
  (sb.st_mode & S_IRUSR) && (u += 4);
  (sb.st_mode & S_IWUSR) && (u += 2);
  (sb.st_mode & S_IXUSR) && (u += 1);
  (sb.st_mode & S_IRGRP) && (g += 4);
  (sb.st_mode & S_IWGRP) && (g += 2);
  (sb.st_mode & S_IXGRP) && (g += 1);
  (sb.st_mode & S_IROTH) && (o += 4);
  (sb.st_mode & S_IWOTH) && (o += 2);
  (sb.st_mode & S_IXOTH) && (o += 1);

  printf("%s | %c%d%d%d | %lu links\n", filename, d, u, g, o, sb.st_nlink);

  return 0;
}

int handle_mkdir(int argc, char **argv) {
  const char *dirname = argv[1];
  const char *modes = argc > 2 ? argv[2] : DEFAULT_MODE;

  int r = mkdir(dirname, resolve_modes(modes));
  if (r != 0) {
    perror("Failed to make directory:");
    return 1;
  }

  return 0;
}

int handle_rmdir(int argc, char **argv) {
  const char *dirname = argv[1];
  const bool recursive = argc > 2;

  int r = remove_dir(dirname, recursive);
  if (r != 0) {
    perror("Failed to delete directory:");
    return r;
  }

  return 0;
}

int handle_prndir(int argc, char **argv) {
  const char *dirname = argv[1];
  int depth = -1;
  if (argc > 2) {
    char *endptr = NULL;
    depth = strtol(argv[2], &endptr, 10);
    if (endptr && *endptr != '\0' && (*endptr < '0' || *endptr > '9')) {
      fprintf(stderr, "Depth is not integer\n");
      return 1;
    }
  }

  int r = print_dir(dirname, depth, 0, 2);
  if (r != 0) {
    perror("Failed to print directory:");
    return r;
  }

  return 0;
}

int handle_mkfl(int argc, char **argv) {
  const char *filename = argv[1];
  const char *modes = argc > 2 ? argv[2] : DEFAULT_MODE;

  int r = make_file(filename, modes);
  if (r != 0) {
    perror("Failed to make file:");
    return r;
  }

  return 0;
}

int handle_rmfl(int argc, char **argv) {
  const char *filename = argv[1];

  int r = remove_file(filename);
  if (r != 0) {
    perror("Failed to delete file:");
    return r;
  }

  return 0;
}

int handle_prnfl(int argc, char **argv) {
  const char *filename = argv[1];

  int r = print_file(filename);
  if (r != 0) {
    perror("Failed to print file:");
    return r;
  }

  return 0;
}

int handle_mksl(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing link name");
    return 1;
  }

  const char *target = argv[1];
  const char *linkname = argv[2];

  int r = symlink(target, linkname);
  if (r != 0) {
    perror("Failed to make symbolic link:");
    return r;
  }

  return 0;
}

int handle_rml(int argc, char **argv) {
  const char *linkname = argv[1];

  int r = remove_file(linkname);
  if (r != 0) {
    perror("Failed to remove symbolic link:");
    return r;
  }

  return 0;
}

int handle_prnsl(int argc, char **argv) {
  const char *linkname = argv[1];

  int r = print_symlink(linkname);
  if (r != 0) {
    perror("Failed to print symbolic link:");
    return r;
  }

  return 0;
}

int handle_prnslf(int argc, char **argv) {
  const char *linkname = argv[1];

  int r = print_symlink_file(linkname);
  if (r != 0) {
    perror("Failed to print symbolic link file");
    return r;
  }

  return 0;
}

int handle_mkhl(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing link name");
    return 1;
  }

  const char *target = argv[1];
  const char *linkname = argv[2];

  int r = link(target, linkname);
  if (r != 0) {
    perror("Failed to make hard link");
    return r;
  }

  return 0;
}

int handle_prnmod(int argc, char **argv) {
  const char *filename = argv[1];

  int r = print_modes(filename);
  if (r != 0) {
    perror("Failed to print modes");
    return r;
  }

  return 0;
}

int handle_chmod(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing file modes\n");
    return 1;
  }

  const char *filename = argv[1];
  const char *modes = argv[2];

  struct stat sb;
  if (lstat(filename, &sb) == -1) {
    perror("lstat");
    return 1;
  }

  if (S_ISLNK(sb.st_mode)) {
    fprintf(stderr, "Cannot change symbolic link mods\n");
    return 1;
  }

  int r = chmod(filename, resolve_modes(modes));

  if (r != 0) {
    perror("Failed to print modes");
    return r;
  }

  return 0;
}

int handle_unknown(int argc, char **argv) {
  printf("Unknown command: %s\n", argv[0]);
  return 1;
}
