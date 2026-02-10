#include "matrix.h"

const int BLOCK = 1;

void msdiv(float *S, float *D, float div, int dim) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      D[i * dim + j] = S[i * dim + j] / div;
    }
  }
}

void mmul(float *A, float *B, float *D, int dim) {
  for (int i = 0; i < dim * dim; i++) {
    D[i] = 0;
  }

  for (int i = 0; i < dim; i++) {
    for (int k = 0; k < dim; k++) {
      float a = A[i * dim + k];
      for (int j = 0; j < dim; j++) {
        D[i * dim + j] += a * B[k * dim + j];
      }
    }
  }
}

void msum(float *A, float *B, float *D, float mul, int dim) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      D[i * dim + j] = A[i * dim + j] + (mul * B[i * dim + j]);
    }
  }
}

float *mmalloc(int dim) {
  float *matrix = (float *)malloc(sizeof(float) * dim * dim);
  memset(matrix, 0, sizeof(float) * dim * dim);
  return matrix;
}

void mfree(float *A) {
  if (A != NULL) {
    free(A);
  }
}

void calculate_b(float *A, float *B, int dim) {
  float row_norm = 0;
  float col_norm = 0;

  mnorms(A, dim, &col_norm, &row_norm);
  mtranspose(A, T, dim);
  msdiv(T, B, col_norm * row_norm, dim);
}

void calculate_r(float *A, float *B, float *R, int dim) {
  mmul(B, A, T, dim);
  msum(E, T, R, -1, dim);
}

void calculate_ar(float *A, float *B, float *R, float *C, int dim,
                  int iterations, bool log) {
  mcopy(E, C, dim);
  for (int i = 1; i < iterations; i++) {
    if (log) {
      printf("Iteration %d...\n", i);
    }
    mmul(R, C, T, dim);
    msum(E, T, C, 1, dim);
  }
  mmul(C, B, T, dim);
  mcopy(T, C, dim);
}
