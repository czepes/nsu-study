#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define KB 1024
#define MB (KB * KB)

const char *DEFAULT_MODE = "755";

mode_t imode(const char modes[3]) {
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

int printdir(const char *dirname, int depth, int step) {
  DIR *dir = opendir(dirname);
  if (!dir) {
    return 1;
  }
  struct dirent *entry = NULL;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    printf("%*s%s\n", depth, "", entry->d_name);
    if (entry->d_type == DT_DIR) {
      char subpath[PATH_MAX];
      sprintf(subpath, "%s/%s", dirname, entry->d_name);
      printdir(subpath, depth + step, step);
    }
  }
  return 0;
}
int makefile(const char *filename, const char *modes) {
  int fd = creat(filename, imode(modes));
  if (fd == -1) {
    return 1;
  }
  close(fd);
  return 0;
}
int printfile(const char *filename) {
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
int get_link_target(const char *linkname, char *target) {
  ssize_t bytes_read = readlink(linkname, target, sizeof(target));
  if (bytes_read == -1) {
    fprintf(stderr, "Failed to locate linked file\n");
    return 1;
  }
  target[bytes_read] = '\0';
  return 0;
}
int printsymlink(const char *linkname) {
  char target[PATH_MAX];
  if (get_link_target(linkname, target) != 0) {
    return 1;
  }
  printf("Link %s -> %s\n", linkname, target);
  return 0;
}
int printsymlinkfile(const char *linkname) {
  char target[PATH_MAX];
  if (get_link_target(linkname, target) != 0) {
    return 1;
  }
  struct stat st;
  if (stat(target, &st) == -1) {
    perror("Failed to get linked file stat");
    return 1;
  }
  if (S_ISREG(st.st_mode)) {
    return printfile(target);
  } else if (S_ISDIR(st.st_mode)) {
    return printdir(target, 0, 2);
  } else {
    fprintf(stderr, "Non-printable file\n");
  }

  return 1;
}
int printmodes(const char *filename) {
  struct stat st;
  if (stat(filename, &st) == -1) {
    return -1;
  }
  char d = '-', u = 0, g = 0, o = 0;
  (st.st_mode & DT_DIR) && (d = 'd');
  (st.st_mode & S_IRUSR) && (u += 4);
  (st.st_mode & S_IWUSR) && (u += 2);
  (st.st_mode & S_IXUSR) && (u += 1);
  (st.st_mode & S_IRGRP) && (g += 4);
  (st.st_mode & S_IWGRP) && (g += 2);
  (st.st_mode & S_IXGRP) && (g += 1);
  (st.st_mode & S_IROTH) && (o += 4);
  (st.st_mode & S_IWOTH) && (o += 2);
  (st.st_mode & S_IXOTH) && (o += 1);
  printf("%s: %c%d%d%d\n", filename, d, u, g, o);
  return 0;
}

int handle_mkdir(int argc, char **argv) {
  const char *dirname = argv[0];
  const char *modes = argc >= 2 ? argv[1] : DEFAULT_MODE;
  int r = mkdir(dirname, imode(modes));
  if (r != 0) {
    perror("Failed to make directory:");
    return 1;
  }
  return 0;
}
int handle_rmdir(int argc, char **argv) {
  const char *dirname = argv[0];
  int r = rmdir(dirname);
  if (r != 0) {
    perror("Failed to remove directory:");
    return 1;
  }
  return 0;
}
int handle_prndir(int argc, char **argv) {
  const char *dirname = argv[0];
  int r = printdir(dirname, 0, 2);
  if (r != 0) {
    perror("Failed to print directory:");
    return 1;
  }
  return 0;
}
int handle_mkfl(int argc, char **argv) {
  const char *filename = argv[0];
  const char *modes = argc >= 2 ? argv[1] : DEFAULT_MODE;
  int r = makefile(filename, modes);
  if (r != 0) {
    perror("Failed to make file:");
    return 1;
  }
  return 0;
}
int handle_rmfl(int argc, char **argv) {
  const char *filename = argv[0];
  int r = remove(filename);
  if (r != 0) {
    perror("Failed to remove file:");
    return 1;
  }
  return 0;
}
int handle_prnfl(int argc, char **argv) {
  const char *filename = argv[0];
  int r = printfile(filename);
  if (r != 0) {
    perror("Failed to print file:");
    return 1;
  }
  return 0;
}
int handle_mksl(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Missing link name");
    return 1;
  }
  const char *target = argv[0];
  const char *linkname = argv[1];
  int r = symlink(target, linkname);
  if (r != 0) {
    perror("Failed to make symbolic link:");
    return 1;
  }
  return 0;
}
int handle_rml(int argc, char **argv) {
  const char *linkname = argv[0];
  int r = unlink(linkname);
  if (r != 0) {
    perror("Failed to remove symbolic link:");
    return 1;
  }
  return 0;
}
int handle_prnsl(int argc, char **argv) {
  const char *linkname = argv[0];
  int r = printsymlink(linkname);
  if (r != 0) {
    perror("Failed to print symbolic link:");
    return 1;
  }
  return 0;
}
int handle_prnslf(int argc, char **argv) {
  const char *linkname = argv[0];
  int r = printsymlinkfile(linkname);
  if (r != 0) {
    perror("Failed to print symbolic link file");
    return 1;
  }
  return 0;
}
int handle_mkhl(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Missing link name");
    return 1;
  }
  const char *target = argv[0];
  const char *linkname = argv[1];
  int r = link(target, linkname);
  if (r != 0) {
    perror("Failed to make hard link");
    return 1;
  }
  return 0;
}
int handle_prnmod(int argc, char **argv) {
  const char *filename = argv[0];
  int r = printmodes(filename);
  if (r != 0) {
    perror("Failed to print modes");
    return 1;
  }
  return 0;
}
int handle_chmod(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Missing file modes\n");
    return 1;
  }
  const char *filename = argv[0];
  const char *modes = argv[1];
  int r = chmod(filename, imode(modes));
  if (r != 0) {
    perror("Failed to print modes");
    return 1;
  }
  return 0;
}
int handle_unknown(int argc, char **argv) {
  printf("Unknown command: %s\n", argv[0]);
  return 1;
}

typedef enum {
  CMD_MKDIR,
  CMD_RMDIR,
  CMD_PRNDIR,
  CMD_MKFL,
  CMD_RMFL,
  CMD_PRNFL,
  CMD_MKSL,
  CMD_MKHL,
  CMD_RML,
  CMD_PRNSL,
  CMD_PRNSLF,
  CMD_PRNMOD,
  CMD_CHMOD,
  CMD_UNKNOWN
} command_id_t;

typedef struct {
  char *name;
  command_id_t id;
} command_lookup_t;

command_id_t get_command_id(char *name) {
  command_lookup_t lookup[] = {
      {"mkdir", CMD_MKDIR},   {"rmdir", CMD_RMDIR}, {"prndir", CMD_PRNDIR},
      {"mkfl", CMD_MKFL},     {"rmfl", CMD_RMFL},   {"prnfl", CMD_PRNFL},
      {"mksl", CMD_MKSL},     {"rmsl", CMD_RML},    {"prnsl", CMD_PRNSL},
      {"prnslf", CMD_PRNSLF}, {"mkhl", CMD_MKHL},   {"rmhl", CMD_RML},
      {"prnmod", CMD_PRNMOD}, {"chmod", CMD_CHMOD},
  };
  int commands = sizeof(lookup) / sizeof(lookup[0]);
  for (int i = 0; i < commands; i++) {
    if (strcmp(name, lookup[i].name) == 0) {
      return lookup[i].id;
    }
  }
  return CMD_UNKNOWN;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Usage: %s [arguments]\n", argv[0]);
    return 0;
  }

  char *path = argv[0];
  char *command = strrchr(path, '/');
  if (command) {
    command++;
  } else {
    command = path;
  }

  int (*handlers[])(int, char **) = {
      [CMD_MKDIR] = &handle_mkdir,   [CMD_RMDIR] = &handle_rmdir,
      [CMD_PRNDIR] = &handle_prndir, [CMD_MKFL] = &handle_mkfl,
      [CMD_RMFL] = &handle_rmfl,     [CMD_PRNFL] = &handle_prnfl,
      [CMD_MKSL] = &handle_mksl,     [CMD_RML] = &handle_rml,
      [CMD_PRNSL] = &handle_prnsl,   [CMD_PRNSLF] = &handle_prnslf,
      [CMD_MKHL] = &handle_mkhl,     [CMD_PRNMOD] = &handle_prnmod,
      [CMD_CHMOD] = &handle_chmod,   [CMD_UNKNOWN] = &handle_unknown};

  return handlers[get_command_id(command)](argc - 1, argv + 1);
}
