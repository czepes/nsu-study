CC = gcc
CFLAGS = -Wall -O1
LINKS = -march=native -lm
LIBS = $(shell pkg-config --cflags --libs openblas)
TARGETS = naive sse avx blas
SRC = $(TARGETS:=.c)
OBJ = $(TARGETS:=.o)

.PHONY: all clean oclean

all: $(TARGETS)

naive: naive.o matrix.o main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LINKS)

sse: sse.o matrix.o main.o
	$(CC) $(CFLAGS) -msse $^ -o $@ $(LINKS)

avx: avx.o matrix.o main.o
	$(CC) $(CFLAGS) -mavx $^ -o $@ $(LINKS)

blas: blas.o matrix.o main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LINKS) $(LIBS)

naive.o: naive.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

sse.o: sse.c matrix.h
	$(CC) $(CFLAGS) -msse -c $< -o $@

avx.o: avx.c matrix.h
	$(CC) $(CFLAGS) -mavx -c $< -o $@

blas.o: blas.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJ) main.o matrix.o

oclean:
	rm -f $(OBJ) main.o matrix.o
