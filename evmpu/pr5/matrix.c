#include "matrix.h"

float *T = NULL;
float *E = NULL;

float max(float a, float b) { return a > b ? a : b; }

void mnorms(float *A, int dim, float *col_norm, float *row_norm) {
  float row_max = 0;
  float col_max = 0;

  for (int i = 0; i < dim; i++) {
    float col_sum = 0;
    float row_sum = 0;

    for (int j = 0; j < dim; j++) {
      col_sum += fabs(A[j * dim + i]);
      row_sum += fabs(A[i * dim + j]);
    }

    col_max = max(col_sum, col_max);
    row_max = max(row_sum, row_max);
  }

  *col_norm = col_max;
  *row_norm = row_max;
}

void mtranspose(float *S, float *D, int dim) {
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      D[j * dim + i] = S[i * dim + j];
    }
  }
}

void mcopy(float *S, float *D, int dim) {
  memcpy(D, S, sizeof(float) * dim * dim);
}

void minit(float **A, float **B, float **R, float **C, int dim) {
  *A = mmalloc(dim);
  *B = mmalloc(dim);
  *R = mmalloc(dim);
  *C = mmalloc(dim);
  T = mmalloc(dim);
  E = mmalloc(dim);

  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      (*A)[i * dim + j] = (i == j) ? dim + 1 : 1;
    }
    E[i * dim + i] = 1;
  }
}

void mquit(float *A, float *B, float *R, float *C) {
  mfree(A);
  mfree(B);
  mfree(R);
  mfree(C);
  mfree(T);
  mfree(E);
}

void calculate(int dim, int iterations, bool log) {
  float *A;
  float *B;
  float *R;
  float *C;

  minit(&A, &B, &R, &C, dim);

  if (log) {
    printf("A = (\n");
    mprint(A, dim);
    printf(")\n");
  }

  if (log) {
    printf("Calculating B = A^T / (||A||_1 * ||A||_∞)\n");
  }
  calculate_b(A, B, dim);

  if (log) {
    printf("Calculating R = I - B*A\n");
  }
  calculate_r(A, B, R, dim);

  if (log) {
    printf("Calculating A^-1 = (I + R * (I + R * (...))) * B\n");
  }
  calculate_ar(A, B, R, C, dim, iterations, log);

  if (log) {
    printf("A^-1 = (\n");
    mprint(C, dim);
    printf(")\n");
  }

  if (log) {
    printf("Calcualting A * A^-1 ≈ I\n");
  }
  mmul(A, C, B, dim);

  if (log) {
    printf("A * A^-1 = (\n");
    mprint(B, dim);
    printf(")\n");
  }

  mquit(A, B, R, C);
}

void mprint(float *A, int dim) {
  if (dim <= COMPACT) {
    for (int i = 0; i < dim; i++) {
      for (int j = 0; j < dim; j++) {
        printf("%8.3f ", A[i * dim + j]);
      }
      printf("\n");
    }
    return;
  }

  const int half = COMPACT;

  for (int i = 0; i < half; i++) {
    for (int j = 0; j < half; j++) {
      printf("%8.3f ", A[i * dim + j]);
    }
    printf("%8s ", "...");
    for (int j = dim - half; j < dim; j++) {
      printf("%8.3f ", A[i * dim + j]);
    }
    printf("\n");
  }

  for (int i = 0; i <= half * 2; i++) {
    printf("%8s ", "...");
  }
  printf("\n");

  for (int i = dim - half; i < dim; i++) {
    for (int j = 0; j < half; j++) {
      printf("%8.3f ", A[i * dim + j]);
    }
    printf("%8s ", "...");
    for (int j = dim - half; j < dim; j++) {
      printf("%8.3f ", A[i * dim + j]);
    }
    printf("\n");
  }
}
