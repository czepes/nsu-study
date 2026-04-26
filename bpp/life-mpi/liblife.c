#include "liblife.h"
#include <stdlib.h>
#include <string.h>

int median(int value, int lower, int upper) {
  return value < lower ? lower : value > upper ? upper : value;
}

void print_field(const char *field, int rows, int cols, int show) {
  char skip_rows = show > 0 && rows > show;
  char skip_cols = show > 0 && cols > show;
  int half = show / 2;

  for (int i = 0; i < rows; i++) {
    if (skip_rows && i == half) {
      for (int j = 0; j < show + skip_cols; j++) {
        printf(j == half && skip_cols ? "+" : "=");
      }
      printf("\n");
      i = rows - half - 1;
      continue;
    }
    for (int j = 0; j < cols; j++) {
      if (skip_cols && j == half) {
        printf("|");
        j = cols - half - 1;
        continue;
      }
      printf("%c", field[i * cols + j]);
    }
    printf("\n");
  }
}

int count_alive_neighbours(const char *field, int row, int col, int rows,
                           int cols) {
  int count = 0;
  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      if (dr == 0 && dc == 0) {
        continue;
      }
      int nr = (row + dr + rows) % rows;
      int nc = (col + dc + cols) % cols;
      count += field[nr * cols + nc] == ALIVE ? 1 : 0;
    }
  }
  return count;
}

char next_cell_state(char cell, int neighbours) {
  switch (cell) {
  case ALIVE:
    return neighbours == 2 || neighbours == 3 ? ALIVE : DEAD;
  case DEAD:
    return neighbours == 3 ? ALIVE : DEAD;
  default:
    return cell;
  }
}

void put_glider(char *field, int col, int row, int rows, int cols) {
  field[(row + 0) * cols + col + 1] = ALIVE;
  field[(row + 1) * cols + col + 2] = ALIVE;
  field[(row + 2) * cols + col + 0] = ALIVE;
  field[(row + 2) * cols + col + 1] = ALIVE;
  field[(row + 2) * cols + col + 2] = ALIVE;
}

int hash32(const char *data, size_t size) {
  int hash = 5381;
  for (size_t i = 0; i < size; i++) {
    hash = ((hash << 5) + hash) + data[i]; // djb2 hash
  }
  return hash;
}

ull hash64(const void *data, size_t size, ull seed) {
  const char *bytes = (const char *)data;
  ull hash = seed;
  const ull PRIME = 1099511628211ULL;

  for (size_t i = 0; i < size; i++) {
    hash ^= bytes[i];
    hash *= PRIME;
  }

  return hash;
}

hash_entry_t hash_field(const char *field, int rows, int cols) {
  size_t size = rows * cols;
  char *copy = malloc(size * sizeof(char));
  memcpy(copy, field, size * sizeof(char));
  hash_entry_t entry = {
      .hash1 = hash64(field, size, 0x1234567890ABCDEFULL),
      .hash2 = hash64(field, size, 0xFEDCBA0987654321ULL),
      .size = size * sizeof(char),
      .data = copy,
  };
  return entry;
}

int match_hashes(hash_entry_t h1, hash_entry_t h2) {
  return h1.hash1 == h2.hash1 && h1.hash2 == h2.hash2 && h1.size == h2.size;
}

int match_fields(char *field1, char *field2, int rows, int cols) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (field1[i * cols + j] != field2[i * cols + j]) {
        return 0;
      }
    }
  }
  return 1;
}
