CC = gcc
CFLAGS = -Wall -O1
TARGETS = main
SRC = $(TARGETS:=.c)
OBJ = $(TARGETS:=.o) cpu-heat.o matrix.o

.PHONY: all clean

all: $(TARGETS)

main: main.o cpu-heat.o matrix.o
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

cpu-heat.o: ../pr5/naive.c ../pr5/matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

matrix.o: ../pr5/matrix.c ../pr5/matrix.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJ)
