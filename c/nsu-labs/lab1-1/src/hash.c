#include "hash.h"

int get_pw(int n) {
    int pw = 1;

    for (int i = 0; i < n - 1; i++) {
        pw *= P;
    }

    return pw;
}

int poly_hash(char *string, int length) {
    int hash = 0;
    int pw = 1;

    for (int i = 0; i < length; i++) {
        hash += (((unsigned char) string[i] % P)) * pw;
        pw *= P;
    }

    return hash;
}

int next_hash(int hash, int pw, char prev, char next) {
    return (hash - ((unsigned char) prev % P)) / P 
        + ((unsigned char) next % P) * pw;
}
