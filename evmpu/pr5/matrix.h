#ifndef MATRIX
#define MATRIX

#include <math.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_N 8
#define MAX_N 2048
#define MIN_M 1
#define MAX_M 100
#define COMPACT 2

extern const int BLOCK;
extern float *T;
extern float *E;

void minit(float **A, float **B, float **R, float **C, int dim);
void calculate(int dim, int iterations, bool log);
void calculate_b(float *A, float *B, int dim);
void calculate_r(float *A, float *B, float *R, int dim);
void calculate_ar(float *A, float *B, float *R, float *C, int dim, int i,
                  bool log);
void mquit(float *A, float *B, float *R, float *C);

float max(float a, float b);
void mnorms(float *A, int dim, float *col_norm, float *row_norm);
void mtranspose(float *S, float *D, int dim);
void msdiv(float *S, float *D, float div, int dim);
void mmul(float *A, float *B, float *D, int dim);
void msum(float *A, float *B, float *D, float mul, int dim);
void mcopy(float *S, float *D, int dim);
float *mmalloc(int dim);
void mfree(float *A);
void mprint(float *A, int dim);

#endif
