GCC = gcc
MPIC = mpicc
FLAGS = -O1 -lm

.PHONY: all clean

all: gen seq par

clean:
	rm -f gen seq par slae.o A b

seq: seq.c slae.o
	$(MPIC) $^ $(FLAGS) -o $@

par: par.c slae.o
	$(MPIC) $^ $(FLAGS) -o $@

gen: gen.c slae.o
	$(GCC) $^ $(FLAGS) -o $@

slae.o: slae.c
	$(GCC) $^ $(FLAGS) -c -o $@
