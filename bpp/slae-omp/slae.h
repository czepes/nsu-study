#ifndef SLAE_HEADER
#define SLAE_HEADER

#include <stddef.h>

#define SHOW 10
#define MIN_ITER 5
#define MAX_ITER 1000

#define N 10000
#define H MIN_ITER
#define TAU 0.001
#define EPS 1e-8
#define EPS2 (EPS * EPS)

void init_slae(double **A, double **b, double **x, size_t size, size_t history);

void gen_slae(double *A, double *b, size_t size);
void write_slae(double *A, double *b, size_t size);
int read_slae(double *A, double *b, size_t size);
void print_slae(double *A, double *b, size_t size, size_t show);

void print_matrix(double *M, size_t rows, size_t cols, size_t show);
void print_vector(double *v, size_t size, size_t show);

void matrix_vector_mul(double *M, double *v, size_t size, double *r);
void vector_scalar_mul(double *v, size_t size, double scalar, double *r);
void vector_vector_sum(double *v1, double *v2, size_t size, double scalar,
                       double *r);

#endif
