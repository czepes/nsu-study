#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printmd(double *M, int rows, int cols) {
  char skip_rows = 0;
  char skip_cols = 0;
  char skipped_cols = 0;
  char skipped_rows = 0;
  const int half = MAXL / 2;

  if (rows > MAXL) {
    skip_rows = 1;
  }
  if (cols > MAXL) {
    skip_cols = 1;
  }

  for (int i = 0; i < rows; i++) {
    if (skip_cols) {
      skipped_cols = 0;
    }
    if (skip_rows && !skipped_rows && i >= half) {
      i = rows - half;
      for (int j = 0; j < MAXL + skip_cols; j++) {
        printf("%*s ", FULL, "...");
      }
      printf("\n");
      skipped_rows = 1;
    }
    for (int j = 0; j < cols; j++) {
      if (skip_cols && !skipped_cols && j >= half) {
        j = cols - half;
        printf("%*s ", FULL, "...");
        skipped_cols = 1;
      }
      printf("%*.*f ", FULL, REAL, M[i * cols + j]);
    }
    printf("\n");
  }
}

double random_in_range(double min, double max) {
  return ((double)rand() * (max - min) / (double)RAND_MAX) + min;
}

void generatem(double *A, double *B) {
  srand(time(NULL));

  for (int i = 0; i < N1; i++) {
    for (int j = 0; j < N2; j++) {
      A[i * N2 + j] = sin(i * 0.01) * cos(j * 0.01);
    }
  }
  for (int i = 0; i < N2; i++) {
    for (int j = 0; j < N3; j++) {
      B[i * N3 + j] = exp(-(i * i + j * j) / (double)N3);
    }
  }
}

void writem(double *A, double *B) {
  if (A) {
    FILE *A_file = fopen("A", "wb");

    for (int i = 0; i < N1; i++) {
      fwrite(A + i * N2, sizeof(double), N2, A_file);
    }

    fclose(A_file);
  }
  if (B) {
    FILE *B_file = fopen("B", "wb");

    for (int i = 0; i < N2; i++) {
      fwrite(B + i * N3, sizeof(double), N3, B_file);
    }

    fclose(B_file);
  }
}

void readm(double **A_ptr, double **B_ptr) {
  if (A_ptr) {
    FILE *A_file = fopen("A", "rb");

    double *A = malloc(N1 * N2 * sizeof(double));
    for (int i = 0; i < N1; i++) {
      fread(A + i * N2, sizeof(double), N2, A_file);
    }
    *A_ptr = A;

    fclose(A_file);
  }
  if (B_ptr) {
    FILE *B_file = fopen("B", "rb");

    double *B = malloc(N2 * N3 * sizeof(double));
    for (int i = 0; i < N2; i++) {
      fread(B + i * N3, sizeof(double), N3, B_file);
    }
    *B_ptr = B;

    fclose(B_file);
  }
}
