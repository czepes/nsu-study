GCC = gcc
FLAGS = -O2 -lm -fopenmp

.PHONY: all clean

all: gen omp schedule

clean:
	rm -f gen omp schedule slae.o A b

omp: omp.c slae.o
	$(GCC) $^ $(FLAGS) -o $@

schedule: schedule.c
	$(GCC) $^ $(FLAGS) -o $@

gen: gen.c slae.o
	$(GCC) $^ $(FLAGS) -o $@

slae.o: slae.c
	$(GCC) $^ $(FLAGS) -c -o $@
