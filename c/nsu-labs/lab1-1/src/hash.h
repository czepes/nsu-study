#ifndef HASH_HEADER
#define HASH_HEADER

#include <stdlib.h>

#define WRONG_HASH -1
#define P 3

int get_pw(int n);
int poly_hash(char *string, int length);
int next_hash(int hash, int pw, char prev, char next);

#endif
