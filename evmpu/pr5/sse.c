#include "immintrin.h"
#include "matrix.h"

#define ALIGN 16

const int BLOCK = 4;

void msdiv(float *S, float *D, float div, int dim) {
  __m128 divv = _mm_set1_ps(div);
  for (int i = 0; i < dim * dim; i += BLOCK) {
    _mm_store_ps(&D[i], _mm_div_ps(_mm_load_ps(&S[i]), divv));
  }
}

void mmul(float *A, float *B, float *D, int dim) {
  const __m128 zeros = _mm_setzero_ps();
  for (int i = 0; i < dim * dim; i += BLOCK) {
    _mm_store_ps(&D[i], zeros);
  }

  for (int i = 0; i < dim; i++) {
    for (int k = 0; k < dim; k++) {
      __m128 a = _mm_set1_ps(A[i * dim + k]);

      for (int j = 0; j < dim; j += BLOCK) {
        __m128 b = _mm_load_ps(&B[k * dim + j]);
        __m128 d = _mm_load_ps(&D[i * dim + j]);
        _mm_store_ps(&D[i * dim + j], _mm_add_ps(d, _mm_mul_ps(a, b)));
      }
    }
  }
}

void msum(float *A, float *B, float *D, float mul, int dim) {
  const __m128 mulv = _mm_set1_ps(mul);
  for (int i = 0; i < dim * dim; i += BLOCK) {
    __m128 a = _mm_load_ps(&A[i]);
    __m128 b = _mm_load_ps(&B[i]);
    _mm_store_ps(&D[i], _mm_add_ps(a, _mm_mul_ps(b, mulv)));
  }
}

float *mmalloc(int dim) {
  float *matrix = (float *)aligned_alloc(ALIGN, sizeof(float) * dim * dim);

  __m128 zeros = _mm_setzero_ps();
  for (int i = 0; i < dim * dim; i += BLOCK) {
    _mm_store_ps(&matrix[i], zeros);
  }

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
