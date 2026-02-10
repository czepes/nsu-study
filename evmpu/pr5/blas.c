#include "cblas.h"
#include "matrix.h"

const int BLOCK = 1;

void mmul(float *A, float *B, float *D, int dim) {
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, dim, dim, dim, 1, A,
              dim, B, dim, 0, D, dim);
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
  cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, dim, dim, dim,
              1 / (col_norm * row_norm), A, dim, E, dim, 0, B, dim);
}

void calculate_r(float *A, float *B, float *R, int dim) {
  mcopy(E, R, dim);
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, dim, dim, dim, -1, B,
              dim, A, dim, 1, R, dim);
}

void calculate_ar(float *A, float *B, float *R, float *C, int dim,
                  int iterations, bool log) {
  mcopy(E, T, dim);
  for (int i = 1; i < iterations; i++) {
    if (log) {
      printf("Iteration %d...\n", i);
    }
    mcopy(E, C, dim);
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, dim, dim, dim, 1, R,
                dim, T, dim, 1, C, dim);
    mcopy(C, T, dim);
  }
  cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, dim, dim, dim, 1, T,
              dim, B, dim, 0, C, dim);
}
