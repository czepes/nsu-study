GCC = gcc
MPIC = mpicc
MPEC = mpecc
FLAGS = -O2 -lm
TRACE = -mpilog

.PHONY: all clean

all: life life-trace

clean:
	rm -f life life-trace liblife.o

liblife.o: liblife.c
	$(GCC) $(FLAGS) $< -c -o $@

life: life.c liblife.o
	$(MPIC) $(FLAGS) $^ -o $@

life-trace: life.c liblife.o
	$(MPEC) $(FLAGS) $(TRACE) $^ -o $@
