#ifndef LIFE_HEADER
#define LIFE_HEADER

#include <stdint.h>
#include <stdio.h>

#define DEAD '.'
#define ALIVE 'o'
#define MAX_AGES 10000

#define MIN_ROWS 100
#define MIN_COLS 100
#define MAX_ROWS 10000
#define MAX_COLS 10000

typedef unsigned long long ull;
typedef struct {
  ull hash1;
  ull hash2;
  size_t size;
  const void *data;
} hash_entry_t;

int median(int value, int lower, int upper);
void print_field(const char *field, int rows, int cols, int show);
int count_alive_neighbours(const char *field, int row, int col, int rows,
                           int cols);
char next_cell_state(char cell, int neighbours);
void put_glider(char *field, int col, int row, int rows, int cols);
int hash32(const char *data, size_t size);
ull hash64(const void *data, size_t size, ull seed);
hash_entry_t hash_field(const char *field, int rows, int cols);
int match_hashes(hash_entry_t h1, hash_entry_t h2);
int match_fields(char *field1, char *field2, int rows, int cols);

#endif
