#ifndef MATRIX_HEADER
#define MATRIX_HEADER

#define N1 5400
#define N2 4000
#define N3 3600

#define MAXL 10
#define INTL 4
#define REAL 6
#define FULL INTL + REAL + 1

void printmd(double *M, int rows, int cols);
void generatem(double *A, double *B);
void writem(double *A, double *B);
void readm(double **A, double **B);

#endif
