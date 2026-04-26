#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liblife.h"

int main(int argc, char **argv) {
  int rank, size;
  int rows, cols;
  char *field = NULL;
  char *next_field = NULL;
  hash_entry_t *history = NULL;
  char *local_flags = NULL;
  char *global_flags = NULL;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc >= 3) {
    rows = atoi(argv[1]);
    cols = atoi(argv[2]);
  } else {
    rows = MIN_ROWS;
    cols = MIN_COLS;
  }

  rows = median(rows, MIN_ROWS, MAX_ROWS);
  cols = median(cols, MIN_COLS, MAX_COLS);

  int sendcounts[size];
  int displs[size];

  int base_rows = rows / size;
  int remainder = rows % size;
  int offset = 0;

  for (int i = 0; i < size; i++) {
    sendcounts[i] = ((i < remainder) ? base_rows + 1 : base_rows) * cols;
    displs[i] = offset;
    offset += sendcounts[i];
  }

  int local_rows = (rank < remainder) ? base_rows + 1 : base_rows;
  int local_size = (local_rows + 2) * cols;

  field = calloc(local_size, sizeof(char));
  next_field = calloc(local_size, sizeof(char));
  history = malloc(MAX_AGES * sizeof(hash_entry_t));
  local_flags = calloc(MAX_AGES, sizeof(char));
  global_flags = calloc(size * MAX_AGES, sizeof(char));

  char *full_field = NULL;
  if (rank == 0) {
    full_field = malloc(rows * cols * sizeof(char));
    memset(full_field, DEAD, rows * cols * sizeof(char));
    put_glider(full_field, 0, 0, rows, cols);
    printf("Age 0:\n");
    print_field(full_field, rows, cols, 20);
  }

  MPI_Scatterv(full_field, sendcounts, displs, MPI_CHAR, field + cols,
               local_rows * cols, MPI_CHAR, 0, MPI_COMM_WORLD);

  int prev_rank = (rank - 1 + size) % size;
  int next_rank = (rank + 1) % size;

  int age = 0;
  int game_over = 0;

  double start_time = MPI_Wtime();

  while (!game_over && age < MAX_AGES) {
    history[age] = hash_field(field + cols, local_rows, cols);

    MPI_Request requests[5];
    MPI_Status statuses[5];
    int req_count = 0;

    // 1&2. Send first and last rows
    MPI_Isend(field + cols, cols, MPI_CHAR, prev_rank, 123, MPI_COMM_WORLD,
              &requests[req_count++]);
    MPI_Isend(field + local_rows * cols, cols, MPI_CHAR, next_rank, 321,
              MPI_COMM_WORLD, &requests[req_count++]);

    // 3&4. Receive upper (prev last) and lower (next first) rows
    MPI_Irecv(field, cols, MPI_CHAR, prev_rank, 321, MPI_COMM_WORLD,
              &requests[req_count++]);
    MPI_Irecv(field + (local_rows + 1) * cols, cols, MPI_CHAR, next_rank, 123,
              MPI_COMM_WORLD, &requests[req_count++]);

    // 5. Compare current field to history fields
    for (int i = 0; i < age; i++) {
      local_flags[i] = match_hashes(history[age], history[i]);
    }

    // 6. Flags barter
    MPI_Iallgather(local_flags, age, MPI_CHAR, global_flags, age, MPI_CHAR,
                   MPI_COMM_WORLD, &requests[req_count++]);

    // 7. Inner field
    for (int row = 2; row < local_rows; row++) {
      for (int col = 0; col < cols; col++) {
        int neighbours =
            count_alive_neighbours(field, row, col, local_rows + 2, cols);
        int pos = row * cols + col;
        next_field[pos] = next_cell_state(field[pos], neighbours);
      }
    }

    // 8-12. Waiting for communications to end
    MPI_Waitall(req_count, requests, statuses);

    // 10. Upper field
    for (int col = 0; col < cols; col++) {
      int row = 1;
      int neighbours =
          count_alive_neighbours(field, row, col, local_rows + 2, cols);
      int pos = row * cols + col;
      next_field[pos] = next_cell_state(field[pos], neighbours);
    }

    // 13. Lower field
    for (int col = 0; col < cols; col++) {
      int row = local_rows;
      int neighbours =
          count_alive_neighbours(field, row, col, local_rows + 2, cols);
      int pos = row * cols + col;
      next_field[pos] = next_cell_state(field[pos], neighbours);
    }

    memcpy(field + cols, next_field + cols, local_rows * cols * sizeof(char));

    // 15. Check history flags
    for (int a = 0; a < age; a++) {
      int all_same = 1;
      for (int p = 0; p < size; p++) {
        if (!global_flags[p * age + a]) {
          all_same = 0;
          break;
        }
      }
      if (all_same) {
        game_over = 1;
        break;
      }
    }

    if (game_over) {
      break;
    }

    age++;
  }

  double end_time = MPI_Wtime();

  MPI_Gatherv(history[age].data, local_rows * cols, MPI_CHAR, full_field,
              sendcounts, displs, MPI_CHAR, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    printf("Age %d:\n", age);
    print_field(full_field, rows, cols, 20);
    printf("Game finished at age %d\n", age);
    printf("Total time: %.3f sec.\n", end_time - start_time);
    free(full_field);
  }

  free(field);
  free(next_field);
  for (int i = 0; i < age; i++) {
    free((void *)history[i].data);
  }
  free(history);
  free(local_flags);
  free(global_flags);

  MPI_Finalize();

  return 0;
}
