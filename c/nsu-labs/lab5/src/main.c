#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "huffman.h"

void help(char *program) {
    fprintf(stderr, "Usage: %s [-m c/d] [-i inputFile] [-o outputFile]\n", program);
}

int main(int argc, char *argv[]) {
    int opt = '?';
    char mode = '?';
    char *inputFile = "in.txt";
    char *outputFile = "out.txt";

    bool cli = argc > 1;

    while (cli && (opt = getopt(argc, argv, "hm:i:o:")) != -1) {
        switch (opt) {
            case 'm': {
                mode = *optarg;
                break;
            }
            case 'i': {
                inputFile = optarg;
                break;
            }
            case 'o': {
                outputFile = optarg;
                break;
            }
            case 'h': {
                help(argv[0]);
                return EXIT_SUCCESS;
            }
            case '?': {
                help(argv[0]);
                return EXIT_SUCCESS;
            }
        }
    }

    FILE *in = freopen(inputFile, "rb", stdin);
    FILE *out = freopen(outputFile, "wb", stdout);

    if (!in || !out) {
        fprintf(stderr, "Failed to open files\n");
        if (in) fclose(in); 
        if (out) fclose(out);
        return EXIT_FAILURE;
    }

    if (!cli && scanf("%c", &mode) != 1) {
        return EXIT_FAILURE;
    }

    switch (mode) {
        case 'c': {
            compress();
            fprintf(stderr, "Successfully compressed %s\n", inputFile);
            break;
        }
        case 'd': {
            decompress();
            fprintf(stderr, "Successfully decompressed %s\n", inputFile);
            break;
        }
        default: {
            fprintf(stderr, "Operation %c is not supported\n", mode);
        }
    }

    fclose(in);
    fclose(out);
    return EXIT_SUCCESS;
}
