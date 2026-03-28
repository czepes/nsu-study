#include "slae.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double random_in_range(double min, double max) {
  return ((double)rand() * (max - min) / (double)RAND_MAX) + min;
}

void init_slae(double **A, double **b, double **x, size_t size,
               size_t history) {
  *A = malloc(size * size * sizeof(double));
  *b = malloc(1 * size * sizeof(double));
  *x = calloc(history * size, sizeof(double));
}

void gen_slae(double *A, double *b, size_t size) {
  srand(time(NULL));

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      A[i * size + j] = random_in_range(-1, 1);
    }
    A[i * size + i] += size / 100.;
  }

  double *u = calloc(size, sizeof(double));

  for (int i = 0; i < size; i++) {
    u[i] = sin(2. * M_PI * (i + 1) / N);
  }

  matrix_vector_mul(A, u, size, b);

  free(u);
}

void write_slae(double *A, double *b, size_t size) {
  if (A) {
    FILE *A_file = fopen("A", "wb");
    for (int i = 0; i < size; i++) {
      fwrite(A + i * size, sizeof(double), size, A_file);
    }
    fclose(A_file);
  }
  if (b) {
    FILE *b_file = fopen("b", "wb");
    fwrite(b, sizeof(double), size, b_file);
    fclose(b_file);
  }
}

int read_slae(double *A, double *b, size_t size) {
  if (A) {
    FILE *A_file = fopen("A", "rb");
    if (A == NULL) {
      return 1;
    }
    for (int i = 0; i < size; i++) {
      fread(A + i * size, sizeof(double), size, A_file);
    }
    fclose(A_file);
  }
  if (b) {
    FILE *b_file = fopen("b", "rb");
    if (b == NULL) {
      return 1;
    }
    fread(b, sizeof(double), size, b_file);
    fclose(b_file);
  }
  return 0;
}

void print_vector(double *v, size_t size, size_t show) {
  if (size <= show) {
    for (int i = 0; i < size; i++) {
      printf("%8.3f ", v[i]);
    }
    printf("\n");
    return;
  }

  const int half = show / 2;

  for (int i = 0; i < half; i++) {
    printf("%8.3f ", v[i]);
  }
  printf("%8s ", "...");
  for (int i = size - half; i < size; i++) {
    printf("%8.3f ", v[i]);
  }
  printf("\n");
}

void print_matrix(double *M, size_t rows, size_t cols, size_t show) {
  if (rows <= show && cols <= show) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        printf("%8.3f ", M[i * cols + j]);
      }
      printf("\n");
    }
    return;
  }

  const size_t half = show / 2;

  for (int i = 0; i < half; i++) {
    for (int j = 0; j < half; j++) {
      printf("%8.3f ", M[i * cols + j]);
    }
    printf("%8s ", "...");
    for (int j = cols - half; j < cols; j++) {
      printf("%8.3f ", M[i * cols + j]);
    }
    printf("\n");
  }

  for (int i = 0; i <= half * 2; i++) {
    printf("%8s ", "...");
  }
  printf("\n");

  for (int i = rows - half; i < rows; i++) {
    for (int j = 0; j < half; j++) {
      printf("%8.3f ", M[i * cols + j]);
    }
    printf("%8s ", "...");
    for (int j = cols - half; j < cols; j++) {
      printf("%8.3f ", M[i * cols + j]);
    }
    printf("\n");
  }
}

void print_slae(double *A, double *b, size_t size, size_t show) {
  printf("A [%zu x %zu]:\n", size, size);
  print_matrix(A, size, size, show);
  printf("b [%zu]:\n", size);
  print_vector(b, size, show);
}

void matrix_vector_mul(double *M, double *v, size_t size, double *r) {
  for (int i = 0; i < size; i++) {
    r[i] = 0;
    for (int j = 0; j < size; j++) {
      r[i] += M[i * size + j] * v[j];
    }
  }
}

void vector_scalar_mul(double *v, size_t size, double scalar, double *r) {
  for (int i = 0; i < size; i++) {
    r[i] = v[i] * scalar;
  }
}

void vector_vector_sum(double *v1, double *v2, size_t size, double scalar,
                       double *r) {
  for (int i = 0; i < size; i++) {
    r[i] = v1[i] + scalar * v2[i];
  }
}
