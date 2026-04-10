GCC = gcc
MPIC = mpicc
FLAGS = -O2 -lm

.PHONY: all clean

all: matrix.o gen grid

clean:
	rm -f matrix.o gen grid A B

grid: grid.c matrix.o
	$(MPIC) $(FLAGS) $^ -o $@

gen: gen.c matrix.o
	$(GCC) $(FLAGS) $^ -o $@

matrix.o: matrix.c
	$(GCC) $(FLAGS) $^ -c -o $@
